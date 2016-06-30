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


#include <videocore/mixers/iOS/GLESVideoMixer.h>
#include <videocore/sources/iOS/GLESUtil.h>
#include <videocore/filters/FilterFactory.h>

#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/gl.h>
#import <UIKit/UIKit.h>

#include <CoreVideo/CoreVideo.h>

#include <glm/gtc/matrix_transform.hpp>

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)


// Convenience macro to dispatch an OpenGL ES job to the created videocore::JobQueue
#define PERF_GL(x, dispatch) do {\
m_glJobQueue.dispatch([=](){\
EAGLContext* cur = [EAGLContext currentContext];\
if(m_glesCtx) {\
[EAGLContext setCurrentContext:(EAGLContext*)m_glesCtx];\
}\
x ;\
[EAGLContext setCurrentContext:cur];\
});\
} while(0)
// Dispatch and execute synchronously
#define PERF_GL_sync(x) PERF_GL((x), enqueue_sync);
// Dispatch and execute asynchronously
#define PERF_GL_async(x) PERF_GL((x), enqueue);

    void
    GLESVideoMixer::pushBuffer(const uint8_t *const data,
                               size_t size,
                               videocore::IMetadata &metadata)
    {
        if(m_paused.load()) {
            return;
        }
        
        VideoBufferMetadata & md = dynamic_cast<VideoBufferMetadata&>(metadata);
        const int zIndex = md.getData<kVideoMetadataZIndex>();
        
        const glm::mat4 mat = md.getData<kVideoMetadataMatrix>();
        
        if(zIndex < m_zRange.first) {
            m_zRange.first = zIndex;
        }
        if(zIndex > m_zRange.second){
            m_zRange.second = zIndex;
        }
        
        std::weak_ptr<ISource> source = md.getData<kVideoMetadataSource>();
        
        const auto h = hash(source);
        
        
        auto inPixelBuffer = *(Android::PixelBufferRef*)data ;

        m_sourceBuffers[h].setBuffer(inPixelBuffer, this->m_textureCache, m_glJobQueue, m_glesCtx);
        m_sourceBuffers[h].setBlends(md.getData<kVideoMetadataBlends>());
        
        auto it = std::find(this->m_layerMap[zIndex].begin(), this->m_layerMap[zIndex].end(), h);
        if(it == this->m_layerMap[zIndex].end()) {
            this->m_layerMap[zIndex].push_back(h);
        }
        this->m_sourceMats[h] = mat;
    }
    void
    GLESVideoMixer::setOutput(std::shared_ptr<IOutput> output)
    {
        m_output = output;
    }
}
}
