#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "LiveVideoCoreSDK.h"

@interface RCTLive : UIView<VCSessionDelegate>

@property (atomic, strong) NSString* rtmpUrl;
@property (atomic, assign) BOOL horizontal;
@property (atomic, assign) NSInteger videoSize; //0:cif 1:d1 2:720p
@property (atomic, assign) LIVE_FRAMERATE frameRate;
@property (atomic, assign) LIVE_BITRATE bitRate;
@property (atomic, assign) VCCameraState cameraState;
@property (atomic, assign) VCAspectMode aspectMode;

- (id)initWithBridge:(RCTBridge *)bridge;
- (void)connectionStatusChanged:(VCSessionState)sessionState;

@end
