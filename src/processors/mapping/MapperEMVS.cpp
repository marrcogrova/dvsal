//---------------------------------------------------------------------------------------------------------------------
//  EMVS https://github.com/uzh-rpg/rpg_emvs
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2018
//---------------------------------------------------------------------------------------------------------------------
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
//  and associated documentation files (the "Software"), to deal in the Software without restriction,
//  including without limitation the rights to use, copy, modify, merge, publish, distribute,
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------

#include <dvsal/processors/mapping/MapperEMVS.h>
#include <dvsal/processors/mapping/MedianFiltering.h>
#include <pcl/filters/radius_outlier_removal.h>

namespace EMVS {

MapperEMVS::MapperEMVS(const std::vector<float>& _camParams, const ShapeDSI& _dsiShape){
  dsiShape_ = _dsiShape;

  width_  = static_cast<int>(_camParams[4]);
  height_ = static_cast<int>(_camParams[5]);

  K_ << _camParams[0],      0.f     , _camParams[2],
        0.f          , _camParams[1], _camParams[3],
        0.f          ,      0.f     ,     1.f;

  setupDSI();

  precomputeRectifiedPoints();
}


bool MapperEMVS::evaluateDSI(const std::vector<dv::Event>& events,
                             const TrajectoryType& trajectory,
                             const Eigen::Matrix4f& T_rv_w)
{
  if(events.size() < packet_size_){
    std::cout << __FUNCTION__ << "- ERROR - Number of events ( " << events.size() << ") < packet size (" << packet_size_ << ")";
    return false;
  }

  // 2D coordinates of the events transferred to reference view using plane Z = Z_0.
  // We use Vector4f because Eigen is optimized for matrix multiplications with inputs whose size is a multiple of 4
  static std::vector<Eigen::Vector4f> event_locations_z0;
  event_locations_z0.clear();

  // List of camera centers
  static std::vector<Eigen::Vector3f> camera_centers;
  camera_centers.clear();

  // Loop through the events, grouping them in packets of frame_size_ events
  size_t current_event_ = 0;
  while(current_event_ + packet_size_ < events.size())
  {
    // Events in a packet are assigned the same timestamp (mid-point), for efficiency
    // 666
    float frame_ts = events[current_event_ + packet_size_ / 2].timestamp() * 0.000001;

    Eigen::Matrix4f T_w_ev; // from event camera to world
    Eigen::Matrix4f T_rv_ev; // from event camera to reference viewpoint
    if(!trajectory.getPoseAt(frame_ts, T_w_ev))
    {
      current_event_++;
      continue;
    }

    T_rv_ev = T_rv_w * T_w_ev;

    const Eigen::Matrix4f T_ev_rv = T_rv_ev.inverse();
    const Eigen::Matrix3f R = T_ev_rv.block<3,3>(0,0);
    const Eigen::Vector3f t = T_ev_rv.block<3,1>(0,3); 

    // Optical center of the event camera in the coordinate frame of the reference view
    camera_centers.push_back(-R.transpose() * t);

    // Project the points on plane at distance z0
    const float z0 = raw_depths_vec_[0];

    // Planar homography  (H_z0)^-1 that maps a point in the reference view to the event camera through plane Z = Z0 (Eq. (8) in the IJCV paper)
    Eigen::Matrix3f H_z0_inv = R;
    H_z0_inv *= z0;
    H_z0_inv.col(2) += t;

    // Compute H_z0 in pixel coordinates using the intrinsic parameters
    Eigen::Matrix3f H_z0_inv_px = K_ * H_z0_inv * virtualCam_.Kinv_;
    Eigen::Matrix3f H_z0_px = H_z0_inv_px.inverse();

    // Use a 4x4 matrix to allow Eigen to optimize the speed
    Eigen::Matrix4f H_z0_px_4x4;
    H_z0_px_4x4.block<3,3>(0,0) = H_z0_px;
    H_z0_px_4x4.col(3).setZero();
    H_z0_px_4x4.row(3).setZero();

    // For each packet, precompute the warped event locations according to Eq. (11) in the IJCV paper.
    for (size_t i=0; i < packet_size_; ++i)
    {
      const dv::Event& e = events[current_event_++];
      Eigen::Vector4f p;

      p.head<2>() = precomputed_rectified_points_.col(e.y() * width_ + e.x());
      p[2] = 1.;
      p[3] = 0.;

      p = H_z0_px_4x4 * p;
      p /= p[2];

      event_locations_z0.push_back(p);
    }
  }

  dsi_.resetGrid();
  fillVoxelGrid(event_locations_z0, camera_centers);

  return true;
}


void MapperEMVS::fillVoxelGrid(const std::vector<Eigen::Vector4f>& event_locations_z0,
                               const std::vector<Eigen::Vector3f>& camera_centers)
{
  // This function implements Step 2 of Algorithm 1 in the IJCV paper.
  // It maps events from plane Z0 to all the planes Zi of the DSI using Eq. (15)
  // and then votes for the corresponding voxel using bilinear voting.

  // For efficiency reasons, we split each packet into batches of N events each
  // which allows to better exploit the L1 cache
  static const int N = 128;
  typedef Eigen::Array<float, N, 1> Arrayf;

  const float z0 = raw_depths_vec_[0];

  // Parallelize over the planes of the DSI with OpenMP
  // (each thread will process a different depth plane)
  #pragma omp parallel for if (event_locations_z0.size() >= 20000)
  for(size_t depth_plane = 0; depth_plane < raw_depths_vec_.size(); ++depth_plane)
  {
    const Eigen::Vector4f* pe = &event_locations_z0[0];
    float *pgrid = dsi_.getPointerToSlice(depth_plane);

    for (size_t packet=0; packet < camera_centers.size(); ++packet)
    {
      // Precompute coefficients for Eq. (15)
      const Eigen::Vector3f& C = camera_centers[packet];
      const float zi = static_cast<float>(raw_depths_vec_[depth_plane]),
          a = z0 * (zi - C[2]),
          bx = (z0 - zi) * (C[0] * virtualCam_.fx_ + C[2] * virtualCam_.cx_),
          by = (z0 - zi) * (C[1] * virtualCam_.fy_ + C[2] * virtualCam_.cy_),
          d = zi * (z0 - C[2]);

      // Update voxel grid now, N events per iteration
      for(size_t batch=0; batch < packet_size_ / N; ++batch, pe += N)
      {
        // Eq. (15)
        Arrayf X, Y;
        for (size_t i=0; i < N; ++i)
        {
          X[i] = pe[i][0];
          Y[i] = pe[i][1];
        }
        X = (X * a + bx) / d;
        Y = (Y * a + by) / d;

        for (size_t i=0; i < N; ++i)
        {
          // Bilinear voting
          dsi_.accumulateGridValueAt(X[i], Y[i], pgrid);
        }
      }
    }
  }
}


void MapperEMVS::setupDSI()
{
  // CHECK_GT(dsiShape_.min_depth_, 0.0);
  // CHECK_GT(dsiShape_.max_depth_ , dsiShape_.min_depth_);

  depths_vec_ = TypeDepthVector(dsiShape_.min_depth_, dsiShape_.max_depth_, dsiShape_.dimZ_);
  raw_depths_vec_ = depths_vec_.getDepthVector();

  dsiShape_.dimX_ = (dsiShape_.dimX_ > 0) ? dsiShape_.dimX_ : width_; //dvs_cam_.fullResolution().width
  dsiShape_.dimY_ = (dsiShape_.dimY_ > 0) ? dsiShape_.dimY_ : height_;

  float f_virtualCam_;
  if (dsiShape_.fov_ < 10.f)
  {
    std::cout << __FUNCTION__ <<" - Specified DSI FoV < 10 deg. Will use camera FoV instead. \n";
    f_virtualCam_ = K_(0,0);
  }
  else
  {
    const float dsi_fov_rad = dsiShape_.fov_ * CV_PI / 180.0;
    f_virtualCam_ = 0.5 * (float) dsiShape_.dimX_ / std::tan(0.5 * dsi_fov_rad);
  }
  std::cout<< __FUNCTION__  << " - Focal length of virtual camera: " << f_virtualCam_ << " pixels. \n";

  virtualCam_ = dvsal::PinholeCamera(dsiShape_.dimX_, dsiShape_.dimY_,
                               f_virtualCam_, f_virtualCam_,
                               0.5 * (float)dsiShape_.dimX_, 0.5 * (float)dsiShape_.dimY_);
  
  dsi_ = Grid3D(dsiShape_.dimX_, dsiShape_.dimY_, dsiShape_.dimZ_);
}


void MapperEMVS::precomputeRectifiedPoints()
{
  // Create a lookup table that maps pixel coordinates to undistorted pixel coordinates
  precomputed_rectified_points_ = Eigen::Matrix2Xf(2, height_ * width_);
  for(int y=0; y < height_; y++)
  {
    for(int x=0; x < width_; ++x)
    {
      // 666 add rectify point method to geometry utils
      Eigen::Vector2f rectified_point = virtualCam_.rectifyPoint(Eigen::Vector2f(x,y)); //666
      precomputed_rectified_points_.col(y * width_ + x) = rectified_point;
    }
  }
}


void MapperEMVS::convertDepthIndicesToValues(const cv::Mat &depth_cell_indices, cv::Mat &depth_map)
{
  // Convert depth indices to depth values, for all pixels
  depth_map = cv::Mat(depth_cell_indices.rows, depth_cell_indices.cols, CV_32F);
  for(int y=0; y<depth_cell_indices.rows; ++y)
  {
    for(int x=0; x<depth_cell_indices.cols; ++x)
    {
      depth_map.at<float>(y,x) = depths_vec_.cellIndexToDepth(depth_cell_indices.at<uchar>(y,x));
    }
  }
}


void MapperEMVS::removeMaskBoundary(cv::Mat& mask, int border_size)
{
  for(int y=0; y<mask.rows; ++y)
  {
    for(int x=0; x<mask.cols; ++x)
    {
      if(x <= border_size || x >= mask.cols - border_size ||
         y <= border_size || y >= mask.rows - border_size)
      {
        mask.at<uchar>(y,x) = 0;
      }
    }
  }
}


void MapperEMVS::getDepthMapFromDSI(cv::Mat& depth_map, cv::Mat &confidence_map, cv::Mat &mask, const OptionsDepthMap &options_depth_map)
{
  // Reference: Section 5.2.3 in the IJCV paper.

  // Maximum number of votes along optical ray
  cv::Mat depth_cell_indices;
  dsi_.collapseMaxZSlice(&confidence_map, &depth_cell_indices);
  
  // Adaptive thresholding on the confidence map
  cv::Mat confidence_8bit;
  cv::normalize(confidence_map, confidence_8bit, 0.0, 255.0, cv::NORM_MINMAX);
  confidence_8bit.convertTo(confidence_8bit, CV_8U);
  
  cv::adaptiveThreshold(confidence_8bit,
                        mask,
                        1,
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                        cv::THRESH_BINARY,
                        options_depth_map.adaptive_threshold_kernel_size_,
                        -options_depth_map.adaptive_threshold_c_);


  // Clean up depth map using median filter (Section 5.2.5 in the IJCV paper)
  cv::Mat depth_cell_indices_filtered;
  huangMedianFilter(depth_cell_indices,
                    depth_cell_indices_filtered,
                    mask,
                    options_depth_map.median_filter_size_);

  // Remove the outer border to suppress boundary effects
  const int border_size = std::max(options_depth_map.adaptive_threshold_kernel_size_ / 2, 1);
  removeMaskBoundary(mask, border_size);

  // Convert depth indices to depth values
  convertDepthIndicesToValues(depth_cell_indices_filtered, depth_map);
}


void MapperEMVS::getPointcloud(const cv::Mat& depth_map,
                                const cv::Mat& mask,
                                const OptionsPointCloud &options_pc,
                                PointCloud::Ptr &pc_
                              )
{
  if ( !(depth_map.rows == mask.rows) || !(depth_map.cols == mask.cols)){
    return;
  }
  
  // Convert depth map to point cloud
  pc_->clear();
  for(int y=0; y<depth_map.rows; ++y)
  {
    for(int x=0; x<depth_map.cols; ++x)
    {
      if(mask.at<uint8_t>(y,x) > 0)
      {
        Eigen::Vector3d b_rv = virtualCam_.projectPixelTo3dRay(Eigen::Vector2d(x,y));
        b_rv.normalize();
        Eigen::Vector3d xyz_rv = (b_rv / b_rv[2] * depth_map.at<float>(y,x));

        pcl::PointXYZI p_rv; // 3D point in reference view
        p_rv.x = xyz_rv.x();
        p_rv.y = xyz_rv.y();
        p_rv.z = xyz_rv.z();
        p_rv.intensity = 1.0 / p_rv.z;
        pc_->push_back(p_rv);
      }
    }
  }

  // Filter point cloud to remove outliers (Section 5.2.5 in the IJCV paper)
  PointCloud::Ptr cloud_filtered (new PointCloud);
  pcl::RadiusOutlierRemoval<PointType> outlier_rm;
  outlier_rm.setInputCloud(pc_);
  outlier_rm.setRadiusSearch(options_pc.radius_search_);
  outlier_rm.setMinNeighborsInRadius(options_pc.min_num_neighbors_);
  outlier_rm.filter(*cloud_filtered);

  pc_->swap(*cloud_filtered);
}

}
