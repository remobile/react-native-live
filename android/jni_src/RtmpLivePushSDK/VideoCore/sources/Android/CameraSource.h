/*
 
 Video Core
 Copyright (c) 2014 James G. Hurley
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 */
#ifndef __videocore__CameraSource__
#define __videocore__CameraSource__

#include <iostream>
#include <videocore/sources/ISource.hpp>
#include <videocore/transforms/IOutput.hpp>
#include <glm/glm.hpp>


namespace videocore { namespace Android {
    
    /*!
     *  Capture video from the device's cameras.
     */
    class CameraSource : public ISource, public std::enable_shared_from_this<CameraSource>
    {
    public:
        
        
        /*! Constructor */
        CameraSource(void *jvm, void *jcamera);
        
        /*! Destructor */
        ~CameraSource();
        
        /*! ISource::setOutput */
        void setOutput(std::shared_ptr<IOutput> output);
        
        /*!
         *  Setup camera properties
         *
         *  \param fps      Optional parameter to set the output frames per second.
         *  \param useFront Start with the front-facing camera
         *  \param sessionPreset name of the preset to use for the capture session
         */
        void setupCamera(int fps = 15, bool useFront = true, std::string sessionPreset = "medium");
        
        
        /*!
         *  Toggle the camera between front and back-facing cameras.
         */
        void toggleCamera();

    public:
        /*! PreviewCallback::onPreviewFrame */
        void bufferCaptured(uint8_t *data, int width, int height);
        
    private:
        std::weak_ptr<IOutput> m_output;
        
        void *m_jvm;
        void *m_camera;
        
        int  m_fps;
        bool m_bCameraFontFlag;
    };
    
}
}
#endif /* defined(__videocore__CameraSource__) */
