#import "RCTBridge.h"
#import "RCTLive.h"
#import "RCTLog.h"
#import "RCTUtils.h"
#import "RCTEventDispatcher.h"
#import "UIView+React.h"
#import <AVFoundation/AVFoundation.h>

@interface RCTLive ()

@property (nonatomic, weak) RCTBridge *bridge;

@end

@implementation RCTLive
{
}

- (id)initWithBridge:(RCTBridge *)bridge
{
    if ((self = [super init])) {
        self.bridge = bridge;
    }
    return self;
}

- (void) LiveConnectionStatusChanged: (LIVE_VCSessionState) sessionState {
}

- (void)start {
    CGSize videosize;
    self.IsHorizontal = NO;
    self.RtmpUrl = [NSURL URLWithString:@"rtmp://192.168.1.129:5920/rtmplive/test"];
    
    if (self.IsHorizontal) {
        videosize = LIVE_VIEDO_SIZE_HORIZONTAL_CIF;
    }else{
        videosize = LIVE_VIEDO_SIZE_CIF;
    }
    
    [[LiveVideoCoreSDK sharedinstance] LiveInit:self.RtmpUrl Preview:self VideSize:videosize BitRate:LIVE_BITRATE_800Kbps FrameRate:LIVE_FRAMERATE_20];
    [LiveVideoCoreSDK sharedinstance].delegate = self;
    [[LiveVideoCoreSDK sharedinstance] connect];
    [LiveVideoCoreSDK sharedinstance].micGain = 5;
}

- (void)stop {
    [[LiveVideoCoreSDK sharedinstance] disconnect];
    [[LiveVideoCoreSDK sharedinstance] LiveRelease];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    [self start];
}

- (void)insertReactSubview:(UIView *)view atIndex:(NSInteger)atIndex
{
    [self insertSubview:view atIndex:atIndex + 1];
    return;
}

- (void)removeReactSubview:(UIView *)subview
{
    [subview removeFromSuperview];
    return;
}

- (void)removeFromSuperview
{
    [self stop];
    [super removeFromSuperview];
}

@end
