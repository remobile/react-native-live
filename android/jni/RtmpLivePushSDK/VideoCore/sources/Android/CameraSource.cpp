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

#include <videocore/sources/iOS/CameraSource.h>
#include <videocore/mixers/IVideoMixer.hpp>
#include <videocore/system/pixelBuffer/Apple/PixelBuffer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <jni.h>

#define ATTACH_JVM(name, sign)  \
    bool isAttached = false; \
    JNIEnv *_env = NULL; \
    JavaVM *android_jvm = (JavaVM *)m_jvm; \
    jobject _camera = (jobject)m_camera; \
    jmethodID jmethod; \
    jclass _class; \
    do { \
        if (android_jvm->GetEnv((void **)&_env, JNI_VERSION_1_6) != JNI_OK) { \
            jint res = android_jvm->AttachCurrentThread(&_env,NULL); \
            ASSERT_ERROR(!((res<0)||!_env), "Could not attach thread to JVM"); \
            isAttached = true; \
        } \
        _class = _env->GetObjectClass(_camera); \
        ASSERT_ERROR(_class!=NULL, "GetObjectClass"); \
        _method = _env->GetMethodID(_class, name, sign); \
        ASSERT_ERROR(NULL!=_method, "GetMethodID "##name); \
    } while (0)

#define DETACH_JVM() \
    do { \
        if (isAttached) { \
            android_jvm->DetachCurrentThread(); \
        } \
    } while (0)


#define ASSERT_ERROR(trial, info) do {\
    if (!(trial)) { \
        LOGF("%s  <failed>", info); \
        goto on_error; \
    } else { \
        LOGV("%s  <success>", info); \
    } \
} while (0)


jstring string2jstring(JNIEnv* env, const std::string &str)
{
    jclass strClass = env->FindClass("Ljava/lang/String;");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    int len = str.length();
    jbyteArray bytes = env->NewByteArray(len);
    env->SetByteArrayRegion(bytes, 0, len, (jbyte*)str.c_str());
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}

namespace videocore { namespace Android {

    CameraSource::CameraSource(void *jvm, void *jcamera)
    :
    m_jvm(jvm),
    m_camera(jcamera),
    m_matrix(glm::mat4(1.f)),
    m_orientationLocked(false),
    m_torchOn(false),
    m_useInterfaceOrientation(false),
    m_captureSession(nullptr),
    m_bCameraFontFlag(false)
    {
        attachNativeObject();
    }
    
    CameraSource::~CameraSource()
    {
    }
    void
    CameraSource::attachNativeObject()
    {
        ATTACH_JVM("attachNativeObject", "(J)V");
        _env->CallVoidMethod(_camera, _method, (jlong)this);
    on_error:
        DETACH_JVM();
    }
    void
    CameraSource::setupCamera(int fps, bool useFront, std::string presetType)
    {
        ATTACH_JVM("startCamera", "(IZLjava/lang/String;)V");
        
        m_fps = fps;
        m_bCameraFontFlag = useFront;
        
        _env->CallVoidMethod(_camera, _method, fps, useFront, string2jstring(presetType));
    on_error:
        DETACH_JVM();
    }
    void
    CameraSource::toggleCamera()
    {
        ATTACH_JVM("toggleCamera", "()V");
        _env->CallVoidMethod(_camera, _method);
    on_error:
        DETACH_JVM();
    }
    void
    CameraSource::setOutput(std::shared_ptr<IOutput> output)
    {
        m_output = output;
        
        //auto mixer = std::static_pointer_cast<IVideoMixer>(output);
        
    }
    void
    CameraSource::bufferCaptured(uint8_t *data, int width, int height)
    {
        auto output = m_output.lock();
        if(output) {
            
            VideoBufferMetadata md(1.f / float(m_fps));
            
            md.setData(1, m_matrix, false, shared_from_this());
            
            auto pixelBuffer = std::make_shared<Android::PixelBuffer>(data, width, height);
            
            pixelBuffer->setState(kVCPixelBufferStateEnqueued);
            output->pushBuffer((uint8_t*)&pixelBuffer, sizeof(pixelBuffer), md);
        }
    }
    
}
}
