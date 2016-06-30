#ifndef __LIVE_VIDEO_CORE_SDK_H__
#define __LIVE_VIDEO_CORE_SDK_H__

#include "./RtmpLivePushSDK/VideoCore/api/Android/VCSimpleSession.h"

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

typedef enum {
    LIVE_BITRATE_1Mbps=1500000,
    LIVE_BITRATE_800Kbps=800000,
    LIVE_BITRATE_500Kbps=500000
}LIVE_BITRATE;

typedef enum {
    LIVE_FRAMERATE_30=30,
    LIVE_FRAMERATE_25=25,
    LIVE_FRAMERATE_20=20,
    LIVE_FRAMERATE_15=15
}LIVE_FRAMERATE;

class LiveVideoCoreSDK : VCSessionDelegate
{
public:
    float micGain;//0~1.0
    VCSimpleSession _livesession;
    std::string _rtmpUrl;
    void *_jcamera;
public:
    LiveVideoCoreSDK(void *jvm, void *jcamera);
    void LiveInit(std::string rtmpUrl, void* previewView, CGSize videSize, LIVE_BITRATE iBitRate, LIVE_FRAMERATE iFrameRate, VCCameraState cameraState, VCAspectMode aspectMode);
    void LiveRelease();

    void connect();
    void disconnect();

    void setCameraFront(bool bCameraFrontFlag);
    void setFilter(VCFilter type);

    void focuxAtPoint(CGPoint point);
    
    //VCSessionDelegate protocal
    void connectionStatusChanged(VCSessionState sessionState);
    void didAddCameraSource(VCSimpleSession *session);
    void detectedThroughput(int throughputInBytesPerSecond, int videoRate);
    
};
#endif //__LIVE_VIDEO_CORE_SDK_H__
