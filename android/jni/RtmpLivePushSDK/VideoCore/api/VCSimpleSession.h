#ifndef __VC_SIMPLE_SESSION_H__
#define __VC_SIMPLE_SESSION_H__

#include <string>

typedef struct {
    float x;
    float y;
} CGPoint;

typedef struct {
    float width;
    float height;
} CGSize;


class VCSimpleSession;

enum VCSessionState
{
    VCSessionStateNone,
    VCSessionStatePreviewStarted,
    VCSessionStateStarting,
    VCSessionStateStarted,
    VCSessionStateEnded,
    VCSessionStateError

};

enum VCCameraState
{
    VCCameraStateFront,
    VCCameraStateBack
};

enum VCAspectMode
{
    VCAspectModeFit,
    VCAscpectModeFill
};

enum VCFilter {
    VCFilterNormal,
    VCFilterGray,
    VCFilterInvertColors,
    VCFilterSepia,
    VCFilterBeauty,
    VCFilterAntique
};  

class VCSessionDelegate {
public:
    virtual void connectionStatusChanged(VCSessionState sessionState) = 0;
    virtual void didAddCameraSource(VCSimpleSession *session);
    virtual void detectedThroughput(int throughputInBytesPerSecond, int videoRate);
};

class VCSimpleSession {
public: //for android
    void *_jvm;
    void *_jcamera;
public:
    CGPoint       focusPointOfInterest;   // (0,0) is top-left, (1,1) is bottom-right
    CGPoint       exposurePointOfInterest;
    VCSessionDelegate *delegate;
public:
    void* _previewView;
    
    std::shared_ptr<videocore::simpleApi::PixelBufferOutput> m_pbOutput;
    std::shared_ptr<videocore::Android::MicSource>               m_micSource;
    std::shared_ptr<videocore::Android::CameraSource>            m_cameraSource;
    std::shared_ptr<videocore::Android::PixelBufferSource>     m_pixelBufferSource;
    std::shared_ptr<videocore::AspectTransform>              m_pbAspect;
    std::shared_ptr<videocore::PositionTransform>            m_pbPosition;
    
    std::shared_ptr<videocore::Split> m_videoSplit;
    std::shared_ptr<videocore::AspectTransform>   m_aspectTransform;
    videocore::AspectTransform::AspectMode m_aspectMode;
    std::shared_ptr<videocore::PositionTransform> m_positionTransform;
    std::shared_ptr<videocore::IAudioMixer> m_audioMixer;
    std::shared_ptr<videocore::IVideoMixer> m_videoMixer;
    std::shared_ptr<videocore::ITransform>  m_h264Encoder;
    std::shared_ptr<videocore::ITransform>  m_aacEncoder;
    std::shared_ptr<videocore::ITransform>  m_h264Packetizer;
    std::shared_ptr<videocore::ITransform>  m_aacPacketizer;
    
    std::shared_ptr<videocore::Split>       m_aacSplit;
    std::shared_ptr<videocore::Split>       m_h264Split;
    std::shared_ptr<videocore::Android::MP4Multiplexer> m_muxer;
    
    std::shared_ptr<videocore::IOutputSession> m_outputSession;
    
    
    // properties
    
    JobQueue *_graphManagementQueue;
    
    CGSize _videoSize;
    int    _bitrate;
    
    int    _fps;
    int    _bpsCeiling;
    int    _estimatedThroughput;
    
    BOOL   _useInterfaceOrientation;
    float  _videoZoomFactor;
    int    _audioChannelCount;
    float  _audioSampleRate;
    float  _micGain;
    
    VCCameraState _cameraState;
    VCAspectMode _aspectMode;
    VCSessionState _rtmpSessionState;
    BOOL   _orientationLocked;
    BOOL   _torch;
    
    BOOL _useAdaptiveBitrate;
    BOOL _continuousAutofocus;
    BOOL _continuousExposure;
    CGPoint _focusPOI;
    CGPoint _exposurePOI;
    
    VCFilter _filter;
public:
    void setVideoSize(CGSize videoSize);
    void setOrientationLocked(BOOL orientationLocked);
    void setTorch(BOOL torch);
    void setAspectMode(VCAspectMode aspectMode);
    void setCameraState(VCCameraState cameraState);
    void setRtmpSessionState(VCSessionState rtmpSessionState);
    void setVideoZoomFactor(float videoZoomFactor);
    void setAudioChannelCount(int channelCount);
    void setAudioSampleRate(float sampleRate);
    void setMicGain(float micGain);
    void setContinuousAutofocus(BOOL continuousAutofocus);
    void setContinuousExposure(BOOL continuousExposure);
    void setFocusPointOfInterest(CGPoint focusPointOfInterest);
    void setExposurePointOfInterest(CGPoint exposurePointOfInterest);
    void setUseAdaptiveBitrate(BOOL useAdaptiveBitrate);
public:
    VCSimpleSession(void *jvm, void *jcamera);
    void setupGraph();
    void initWithVideoSize:(CGSize videoSize, int fps, int bps, BOOL useInterfaceOrientation, VCCameraState cameraState, VCAspectMode aspectMode);
    void startRtmpSessionWithURL(std::string rtmpUrl);
    void endRtmpSession();
};

#endif //__VC_SIMPLE_SESSION_H__
