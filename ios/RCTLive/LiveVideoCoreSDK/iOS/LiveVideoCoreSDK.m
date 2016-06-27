//
//  LiveVideoCoreSDK.m
//  LiveVideoCoreSDK
//
//  Created by Alex.Shi on 16/3/2.
//  Copyright © 2016年 com.Alex. All rights reserved.
//

#import "LiveVideoCoreSDK.h"

@implementation LiveVideoCoreSDK
{
    VCSimpleSession   *_livesession;
    NSString* _rtmpUrl;
    UIView* _ShowPreview;
}

@dynamic micGain;

+ (instancetype)sharedinstance {
    static LiveVideoCoreSDK *s_instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        s_instance = [[self alloc] init];
    });
    
    return s_instance;
}


- (void)LiveInit:(NSString*)rtmpUrl Preview:(UIView*)previewView VideSize:(CGSize)videSize BitRate:(LIVE_BITRATE)iBitRate FrameRate:(LIVE_FRAMERATE)iFrameRate Delegate:(id<LIVEVCSessionDelegate>)delegate{
    if (self) {
        _livesession = [[VCSimpleSession alloc] initWithVideoSize:videSize frameRate:iFrameRate bitrate:iBitRate useInterfaceOrientation:YES];
        
        _livesession.delegate = self;
        
        _rtmpUrl = rtmpUrl;
        
        _ShowPreview = previewView;
        [previewView addSubview:_livesession.previewView];
        _livesession.previewView.frame = previewView.bounds;
        
        self.delegate = delegate;
        [_livesession startRtmpSessionWithURL:_rtmpUrl];
        NSLog(@"rtmpUrl=%@\r\nwidth=%.2f, height=%.2f, bitRate=%lu, frameRate=%lu", rtmpUrl, videSize.width, videSize.height, (unsigned long)iBitRate, (unsigned long)iFrameRate);
        
    }
    
    return;
}

- (void)LiveRelease{
    [_livesession endRtmpSession];
    _livesession = nil;
    NSLog(@"LiveRelease: %@", _rtmpUrl);
}

- (void)setFilter:(LIVE_FILTER_TYPE) type{
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

-(void) setMicGain:(float)micGain{
    _livesession.micGain = micGain;
}

-(float) micGain{
    return _livesession.micGain;
}

- (void)focuxAtPoint:(CGPoint)point
{
    _livesession.focusPointOfInterest = point;
    _livesession.exposurePointOfInterest = point;
}

//delegate operation
- (void) connectionStatusChanged: (VCSessionState) sessionState{
    NSLog(@"rtmp live state: %i", (int)sessionState);
    LIVE_VCSessionState state = (LIVE_VCSessionState)sessionState;
    
    [self.delegate LiveConnectionStatusChanged:state];
    
    return;
}

- (void)setCameraFront:(Boolean)bCameraFrontFlag {
    if (!bCameraFrontFlag) {
        _livesession.cameraState = VCCameraStateBack;
        //_ShowPreview.transform = CGAffineTransformMakeScale(1.0, 1.0);
    } else {
        _livesession.cameraState = VCCameraStateFront;
        //_ShowPreview.transform = CGAffineTransformMakeScale(-1.0, 1.0);
    }
}

@end
