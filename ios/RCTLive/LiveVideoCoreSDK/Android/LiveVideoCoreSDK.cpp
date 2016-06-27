//
//  LiveVideoCoreSDK.m
//  LiveVideoCoreSDK
//
//  Created by Alex.Shi on 16/3/2.
//  Copyright © 2016年 com.Alex. All rights reserved.
//

#include "LiveVideoCoreSDK.h"

LiveVideoCoreSDK *sharedinstance()
{
    static LiveVideoCoreSDK *s_instance = NULL;
    if (NULL == s_instance) {
        s_instance = new LiveVideoCoreSDK();
    }
    return s_instance;
}

void LiveVideoCoreSDK::LiveInit(std::string rtmpUrl, void* previewView, CGSize videSize, LIVE_BITRATE iBitRate, LIVE_FRAMERATE iFrameRate)
{
    _livesession.initWithVideoSize:videSize frameRate:iFrameRate bitrate:iBitRate useInterfaceOrientation:YES];
    _livesession.delegate = this;
    _rtmpUrl = rtmpUrl;
    
//    [previewView addSubview:_livesession.previewView];
//    _livesession.previewView.frame = previewView.bounds;
    
    DLog("rtmpUrl=%s\r\nwidth=%.2f, height=%.2f, bitRate=%lu, frameRate=%lu",
          rtmpUrl.c_str(), videSize.width, videSize.height, (unsigned long)iBitRate, (unsigned long)iFrameRate);
}

- (void)LiveRelease{
    _livesession.release();
    DLog("LiveRelease: %s", _rtmpUrl.c_str());
}

- (void)connect {
    DLog("connect: %s", _rtmpUrl.c_str());
    _livesession.startRtmpSessionWithURL(_rtmpUrl);
}

- (void)disconnect {
    DLog("disconnect: %s", _rtmpUrl.c_str());
    _livesession.endRtmpSession();
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
