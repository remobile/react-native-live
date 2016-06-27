#import "RCTBridge.h"
#import "RCTLive.h"
#import "RCTLog.h"
#import "RCTUtils.h"
#import "RCTEventDispatcher.h"
#import "UIView+React.h"
#import <AVFoundation/AVFoundation.h>

#define MAX_DEFAULT_PROPS_INDEX (3)
#define CHECK_METHOD(t) \
do { \
    if (__hasAllPropertySet < MAX_DEFAULT_PROPS_INDEX) { \
        __hasAllPropertySet++; \
    } else if (!__hasStart) { \
        [self start]; \
    } else { \
        [self set##t##Method]; \
    } \
} while (0)

@interface RCTLive ()
@property (nonatomic, weak) RCTBridge *bridge;
@end

@implementation RCTLive
{
    NSInteger __hasAllPropertySet;
    BOOL __hasStart;
    LiveVideoCoreSDK *sdk;
}

- (id)initWithBridge:(RCTBridge *)bridge
{
    if ((self = [super init])) {
        self.bridge = bridge;
        self.videoSize = 0;
        self.horizontal = NO;
        self.bitRate = LIVE_BITRATE_500Kbps;
        self.frameRate = LIVE_FRAMERATE_20;
        self.url = @"rtmp://192.168.1.129:5920/rtmplive/test";
        sdk = [LiveVideoCoreSDK sharedinstance];
    }
    return self;
}

- (void) LiveConnectionStatusChanged: (LIVE_VCSessionState) sessionState {
}

- (CGSize) getVideoSize {
    CGSize videosize;
    if (self.horizontal) {
        if (self.videoSize == 0) {
            videosize = LIVE_VIEDO_SIZE_HORIZONTAL_CIF;
        } else if (self.videoSize == 1) {
            videosize = LIVE_VIEDO_SIZE_HORIZONTAL_D1;
        } else {
            videosize = LIVE_VIEDO_SIZE_HORIZONTAL_720P;
        }
    } else {
        if (self.videoSize == 0) {
            videosize = LIVE_VIEDO_SIZE_CIF;
        } else if (self.videoSize == 1) {
            videosize = LIVE_VIEDO_SIZE_D1;
        } else {
            videosize = LIVE_VIEDO_SIZE_720P;
        }
    }
    return videosize;
}

- (void)start {
    [sdk LiveInit:self.url Preview:self VideSize:[self getVideoSize] BitRate:self.bitRate FrameRate:self.frameRate];
    sdk.delegate = self;
    [sdk connect];
    sdk.micGain = 5;
}

- (void)stop {
    [sdk disconnect];
    [sdk LiveRelease];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
}

- (void)didMoveToSuperview {
    [super didMoveToSuperview];
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
