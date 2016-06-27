#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "LiveVideoCoreSDK.h"

@interface RCTLive : UIView<LIVEVCSessionDelegate>

@property (atomic, strong) NSString* url;
@property (atomic, assign) BOOL horizontal;
@property (atomic, assign) NSInteger videoSize; //0:cif 1:d1 2:720p
@property (atomic, assign) LIVE_FRAMERATE frameRate;
@property (atomic, assign) LIVE_BITRATE bitRate;


- (id)initWithBridge:(RCTBridge *)bridge;
- (void) LiveConnectionStatusChanged: (LIVE_VCSessionState) sessionState;

@end
