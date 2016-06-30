//
//  LiveVideoCoreSDK.h
//  LiveVideoCoreSDK
//
//  Created by Alex.Shi on 16/3/2.
//  Copyright © 2016年 com.Alex. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "../../RtmpLivePushSDK/VideoCore/api/IOS/VCSimpleSession.h"

#define LIVE_VIDEO_DEF_WIDTH  360
#define LIVE_VIDEO_DEF_HEIGHT 640
#define LIVE_VIDEO_DEF_FRAMERATE 25
#define LIVE_VIDEO_DEF_BITRATE   500000

#define LIVE_VIEDO_SIZE_HORIZONTAL_CIF  (CGSizeMake(640, 360))
#define LIVE_VIEDO_SIZE_HORIZONTAL_D1   (CGSizeMake(960, 540))
#define LIVE_VIEDO_SIZE_HORIZONTAL_720P (CGSizeMake(1280, 720))

#define LIVE_VIEDO_SIZE_CIF  (CGSizeMake(360, 640))
#define LIVE_VIEDO_SIZE_D1   (CGSizeMake(540, 960))
#define LIVE_VIEDO_SIZE_720P (CGSizeMake(720, 1280))

typedef NS_ENUM(NSUInteger, LIVE_BITRATE) {
    LIVE_BITRATE_1Mbps=1500000,
    LIVE_BITRATE_800Kbps=800000,
    LIVE_BITRATE_500Kbps=500000
};

typedef NS_ENUM(NSUInteger, LIVE_FRAMERATE) {
    LIVE_FRAMERATE_30=30,
    LIVE_FRAMERATE_25=25,
    LIVE_FRAMERATE_20=20,
    LIVE_FRAMERATE_15=15
};

@interface LiveVideoCoreSDK : NSObject

+ (instancetype)sharedinstance;

@property (atomic, weak)   id<VCSessionDelegate> delegate;
@property (atomic, assign) float micGain;//0~1.0

- (void)LiveInit:(NSString*)rtmpUrl
         Preview:(UIView*)previewView
        VideSize:(CGSize)videSize
         BitRate:(LIVE_BITRATE)iBitRate
       FrameRate:(LIVE_FRAMERATE)iFrameRate
     CameraState:(VCCameraState) cameraState
      AspectMode:(VCAspectMode)aspectMode
        Delegate:(id<VCSessionDelegate>)delegate;
- (void)LiveRelease;

- (void)setCameraFront:(Boolean)bCameraFrontFlag;
- (void)setFilter:(VCFilter) type;

- (void)focuxAtPoint:(CGPoint)point;

@end
