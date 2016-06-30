//
//  LiveVideoCoreSDK.m
//  LiveVideoCoreSDK
//
//  Created by Alex.Shi on 16/3/2.
//  Copyright © 2016年 com.Alex. All rights reserved.
//

#include "LiveVideoCoreSDK.h"
#include "JNIHelper.h"

static LiveVideoCoreSDK *liveVideoCoreSDK = NULL;


extern "C" JNIEXPORT void JNICALL __attribute__((visibility("default")))
Java_com_remobile_live_LiveVideoCoreSDK_LiveInit(JNIEnv *env, jobject object, jobject jcamera, jstring jRtmpUrl, jobject jPreviewView, jint jWidth, jint jHeight, jint jBitRate, jint jFrameRate, jint jCameraState, jint jAspectMode) {
    if (NULL == liveVideoCoreSDK) {
        JavaVM* jvm = NULL;
        env->GetJavaVM(&jvm);
        
        std::string rtmpUrl = jstr2str(env, jRtmpUrl);
        void* previewView = (void *)jPreviewView;
        CGSize videSize = {width: (float)jWidth, height: (float)jHeight};
        LIVE_BITRATE iBitRate = (LIVE_BITRATE)jBitRate;
        LIVE_FRAMERATE iFrameRate = (LIVE_FRAMERATE)jFrameRate;
        VCCameraState cameraState = (VCCameraState)jCameraState;
        VCAspectMode aspectMode = (VCAspectMode)jAspectMode;

        liveVideoCoreSDK = new LiveVideoCoreSDK(jvm, env->NewGlobalRef(jcamera));
        liveVideoCoreSDK->LiveInit(rtmpUrl, previewView, videSize, iBitRate, iFrameRate, cameraState, aspectMode);
    }
}

extern "C" JNIEXPORT void JNICALL __attribute__((visibility("default")))
Java_com_remobile_live_LiveVideoCoreSDK_LiveRelease(JNIEnv *env, jobject object) {
    if (NULL != liveVideoCoreSDK) {
        liveVideoCoreSDK->LiveRelease();
        env->DeleteGlobalRef((jobject)liveVideoCoreSDK->_jcamera);
        delete liveVideoCoreSDK;
        liveVideoCoreSDK = NULL;
    }
}

extern "C" JNIEXPORT void JNICALL __attribute__((visibility("default")))
Java_com_remobile_live_RCTCamera_captureVideoStream(JNIEnv* env, jobject object, jlong target, jbyteArray jdata, jint width, jint height)
{
    videocore::Android::CameraSource *dev = reinterpret_cast<videocore::Android::CameraSource *>(target);
    uint8_t *data = (uint8_t *)env->GetByteArrayElements(jdata, 0);
    dev->bufferCaptured(data, width, height);
    env->ReleaseByteArrayElements(jdata, (jbyte *)data, JNI_ABORT);
}


LiveVideoCoreSDK::LiveVideoCoreSDK(void *jvm, void *jcamera)
:_jcamera(jcamera), _livesession(jvm, jcamera)
{
}

void LiveVideoCoreSDK::LiveInit(std::string rtmpUrl, void* previewView, CGSize videSize, LIVE_BITRATE iBitRate, LIVE_FRAMERATE iFrameRate, VCCameraState cameraState, VCAspectMode aspectMode)
{
    _livesession.initWithVideoSize(videSize, iFrameRate, iBitRate, true, cameraState, aspectMode);
    _livesession.delegate = this;
    _rtmpUrl = rtmpUrl;
    
//    [previewView addSubview:_livesession.previewView];
//    _livesession.previewView.frame = previewView.bounds;
    
    _livesession.startRtmpSessionWithURL(_rtmpUrl);
    DLog("rtmpUrl=%s\r\nwidth=%.2f, height=%.2f, bitRate=%lu, frameRate=%lu",
          rtmpUrl.c_str(), videSize.width, videSize.height, (unsigned long)iBitRate, (unsigned long)iFrameRate);
}

void LiveVideoCoreSDK::LiveRelease()
{
    _livesession.endRtmpSession();
    DLog("LiveRelease: %s", _rtmpUrl.c_str());
}

void LiveVideoCoreSDK::setFilter(VCFilter type)
{
    _livesession.setFilter(type);
}

void LiveVideoCoreSDK::focuxAtPoint(CGPoint point)
{
    _livesession.focusPointOfInterest = point;
    _livesession.exposurePointOfInterest = point;
}

void LiveVideoCoreSDK::connectionStatusChanged(VCSessionState sessionState)
{
    DLog("rtmp live state: %i", sessionState);
}

void LiveVideoCoreSDK::didAddCameraSource(VCSimpleSession *session) {
}

void LiveVideoCoreSDK::detectedThroughput(int throughputInBytesPerSecond, int videoRate) {
}

void LiveVideoCoreSDK::setCameraFront(bool bCameraFrontFlag)
{
    _livesession.setCameraState(bCameraFrontFlag?VCCameraStateFront:VCCameraStateBack);
}

