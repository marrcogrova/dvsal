//---------------------------------------------------------------------------------------------------------------------
//  DVSAL
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2019 - Marco Montes Grova (a.k.a. marrcogrova) 
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

#ifndef BLOCK_DV_CORNERDETECTOR_H_
#define BLOCK_DV_CORNERDETECTOR_H_

#include <flow/flow.h>

#include <dvsal/processors/corner_detectors/Detector.h>
#include <dvsal/processors/corner_detectors/HarrisDetector.h>
#include <dvsal/processors/corner_detectors/FastDetector.h>

#include <mutex>

namespace dvsal{

    class BlockDvCornerDetector: public flow::Block{
    public:
        std::string name() override {return "DV Corner Detector";};
        std::string description() const override {return "Flow wrapper of DVS Corners Detector.\n"
                                                          "- FAST\n"
                                                          "- HARRIS\n";};

        BlockDvCornerDetector();

        virtual QWidget * customWidget(){
            return visualContainer_;
        }

        
    private:
        void initVisualization();

        std::vector<Detector *> detectorList();

        void changeDetector(int _index);

    private:
        bool idle_ = true;

        std::mutex detectorGuard_;
        std::vector<Detector *> detectorList_;
        Detector* currentDetector_;
        
        QComboBox *detectorSelector_ = nullptr;
        QGroupBox *visualContainer_ = nullptr;
        QVBoxLayout *mainLayout_ = nullptr;
    };
}

extern "C" flow::PluginNodeCreator* factory(){
    flow::PluginNodeCreator *creator = new flow::PluginNodeCreator([](){ return std::make_unique<flow::FlowVisualBlock<dvsal::BlockDvCornerDetector>>(); });

    return creator;
}

#endif