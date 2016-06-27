//
//  LiveVideoCoreSDK.m
//  LiveVideoCoreSDK
//
//  Created by Alex.Shi on 16/3/2.
//  Copyright © 2016年 com.Alex. All rights reserved.
//

#include "LiveVideoCoreSDK.h"

extern "C"
{
    JNIEXPORT void JNICALL Java_com_remobile_live_LiveVideoCoreSDK_LiveInit(JNIEnv * env, jobject object);
    JNIEXPORT void JNICALL Java_com_remobile_live_LiveVideoCoreSDK_LiveRelease(JNIEnv * env, jobject object);
}

static LiveVideoCoreSDK *liveVideoCoreSDK = NULL;

std::string jstring2str(JNIEnv* env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr =  (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if(alen >  0) {
        rtn = (char *)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

JNIEXPORT void JNICALL Java_com_remobile_live_LiveVideoCoreSDK_LiveInit(JNIEnv *env, jobject object, jstring jRtmpUrl, jobject jPreviewView, jint jWidth, jint jHeight, jint jBitRate, jint jFrameRate) {
    if (NULL == liveVideoCoreSDK) {
        JavaVM* jvm = NULL;
        env->GetJavaVM(&jvm);
        
        std::string rtmpUrl = jstring2str(env, jRtmpUrl);
        void* previewView = (void *)jPreviewView;
        CGSize videSize = {width: jWidth, height: jHeight};
        LIVE_BITRATE iBitRate = (LIVE_BITRATE)jBitRate;
        LIVE_FRAMERATE iFrameRate = (LIVE_FRAMERATE)jFrameRate;
        
        liveVideoCoreSDK = new liveVideoCoreSDK(jvm, object);
        liveVideoCoreSDK->LiveInit(rtmpUrl, previewView, videSize, (LIVE_BITRATE)iBitRate, (LIVE_FRAMERATE)iFrameRate);
    }
}

JNIEXPORT void JNICALL Java_com_remobile_live_LiveVideoCoreSDK_LiveRelease(JNIEnv *env, jobject object) {
    if (NULL != liveVideoCoreSDK) {
        liveVideoCoreSDK->LiveRelease();
        delete liveVideoCoreSDK;
        liveVideoCoreSDK = NULL;
    }
}

void LiveVideoCoreSDK::LiveInit(std::string rtmpUrl, void* previewView, CGSize videSize, LIVE_BITRATE iBitRate, LIVE_FRAMERATE iFrameRate)
{
    _livesession.initWithVideoSize:videSize frameRate:iFrameRate bitrate:iBitRate useInterfaceOrientation:YES];
    _livesession.delegate = this;
    _rtmpUrl = rtmpUrl;
    
//    [previewView addSubview:_livesession.previewView];
//    _livesession.previewView.frame = previewView.bounds;
    
    _livesession.startRtmpSessionWithURL(_rtmpUrl);
    DLog("rtmpUrl=%s\r\nwidth=%.2f, height=%.2f, bitRate=%lu, frameRate=%lu",
          rtmpUrl.c_str(), videSize.width, videSize.height, (unsigned long)iBitRate, (unsigned long)iFrameRate);
}

- (void)LiveRelease{
    _livesession.endRtmpSession();
    _livesession.release();
    DLog("LiveRelease: %s", _rtmpUrl.c_str());
}

- (void)setFilter:(LIVE_FILTER_TYPE) type
{
    switch (type) {
        case LIVE_FILTER_ORIGINAL:
            [_livesession setFilter:VCFilterNormal];
            break;
        case LIVE_FILTER_BEAUTY:
            [_livesession setFilter:VCFilterBeauty];
            break;
        case LIVE_FILTER_ANTIQUE:
            [_livesession setFilter:VCFilterSepia];
            break;
        case LIVE_FILTER_BLACK:
            [_livesession setFilter:VCFilterGray];
            break;
        default:
            break;
    }
}

-(void) setMicGain:(float)micGain
{
    _livesession.micGain = micGain;
}

-(float) micGain
{
    return _livesession.micGain;
}

- (void)focuxAtPoint:(CGPoint)point
{
    _livesession.focusPointOfInterest = point;
    _livesession.exposurePointOfInterest = point;
}

//delegate operation
- (void) connectionStatusChanged: (VCSessionState) sessionState
{
    DLog("rtmp live state: %i", sessionState);
    LIVE_VCSessionState state = (LIVE_VCSessionState)sessionState;
    
    [self.delegate LiveConnectionStatusChanged:state];
    
    return;
}

- (void)setCameraFront:(Boolean)bCameraFrontFlag
{
    if (!bCameraFrontFlag) {
        _livesession.cameraState = VCCameraStateBack;
    } else {
        _livesession.cameraState = VCCameraStateFront;
    }
}

@end
