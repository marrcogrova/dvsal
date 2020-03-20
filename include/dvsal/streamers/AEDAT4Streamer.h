//---------------------------------------------------------------------------------------------------------------------
//  DVSAL
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2020 - Marco Montes Grova (a.k.a. mgrova) marrcogrova@gmail.com 
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

#ifndef AEDAT_4_STREAMER_H_
#define AEDAT_4_STREAMER_H_

#include <dvsal/streamers/Streamer.h>
#include <dvsal/utils/FileDataTable.hpp>
#include <dvsal/utils/filebuffer.hpp>
#include <dvsal/utils/IOHeader.hpp>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"
#include "zstd.h"

#include <dv-sdk/data/event_base.hpp>

namespace dvsal{

    struct InputStream {
    	int32_t id_;
    	std::string name_;
    	std::string typeIdentifier_;
    	dv::Types::Type type_;
    };

    struct InputInformation {
    	size_t fileSize_;
    	dv::CompressionType compression_;
    	int64_t dataTablePosition_;
    	size_t dataTableSize_;
    	int64_t timeLowest_;
    	int64_t timeHighest_;
    	int64_t timeDifference_;
    	int64_t timeShift_;
    	std::vector<InputStream> streams_;
    	std::unique_ptr<dv::FileDataTable> dataTable_;
    };


    class AEDAT4Streamer : public Streamer {
    public:
        AEDAT4Streamer(std::istream *_inStream, const InputInformation *_inInfo);

        ~AEDAT4Streamer(){};

        static InputInformation parseHeader(std::ifstream &_fStream);
    	static std::ifstream openFile(const std::filesystem::path &_fPath);
    public:
		bool init();
		bool events(dv::EventStore &_events);
        bool image(cv::Mat &_image); // Fake image using events
        bool step();
        bool cutUsingTime(int _microseconds);

    private:
        std::ifstream datasetFile_;

        dv::EventStore events_;

        /// Input stream to read data from.
        std::istream *inputStream_;
        /// Input information from header.
        const InputInformation *inputInfo_;
        /// Support LZ4 compression.
        std::shared_ptr<struct LZ4F_dctx_s> compressionLZ4_;
        /// Support Zstd compression.
        std::shared_ptr<struct ZSTD_DCtx_s> compressionZstd_;
        /// Buffer to hold data while decoding.
        std::vector<char> readBuffer_;
        std::vector<char> decompressBuffer_;
        /// cache for data decompressed
        dv::FileBuffer cacheBuffer_;
        /// limit consecutive log messages
        bool slowdownWarningSent_;

        static constexpr size_t lz4CompressionChunkSize_ = 64 * 1024;
    
    private:
        static std::unique_ptr<dv::FileDataTable> loadFileDataTable(std::ifstream &_fStream, InputInformation &_fInfo);
        static std::shared_ptr<struct LZ4F_dctx_s> lz4InitDecompressionContext();
	    static std::shared_ptr<struct ZSTD_DCtx_s> zstdInitDecompressionContext();    
        static void decompressLZ4(const char *_dataPtr, size_t _dataSize, std::vector<char> &_decompressBuffer,
		                            struct LZ4F_dctx_s *_decompressContext);
    
        static void decompressZstd(const char *_dataPtr, size_t _dataSize, std::vector<char> &_decompressBuffer,
		                            struct ZSTD_DCtx_s *_decompressContext);
        void decompressData(const char *_dataPtr, size_t _dataSize);

    };
}


#endif