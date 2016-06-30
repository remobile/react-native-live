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


- (void)LiveInit:(NSString*)rtmpUrl
         Preview:(UIView*)previewView
        VideSize:(CGSize)videSize
         BitRate:(LIVE_BITRATE)iBitRate
       FrameRate:(LIVE_FRAMERATE)iFrameRate
     CameraState:(VCCameraState) cameraState
      AspectMode:(VCAspectMode)aspectMode
        Delegate:(id<VCSessionDelegate>)delegate
{
    if (self) {
        _livesession = [[VCSimpleSession alloc] initWithVideoSize:videSize frameRate:iFrameRate bitrate:iBitRate useInterfaceOrientation:YES cameraState:cameraState aspectMode:aspectMode];
        
        _livesession.delegate = delegate;
        
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

- (void)setFilter:(VCFilter) type{
    [_livesession setFilter:type];
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
