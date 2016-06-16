#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "LiveVideoCoreSDK.h"

@interface RCTLive : UIView<LIVEVCSessionDelegate>

@property (atomic, copy) NSURL* RtmpUrl;
@property (atomic, assign) Boolean IsHorizontal;

- (id)initWithBridge:(RCTBridge *)bridge;
- (void) LiveConnectionStatusChanged: (LIVE_VCSessionState) sessionState;

@end
