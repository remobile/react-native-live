#ifndef __VC_SIMPLE_SESSION_H__
#define __VC_SIMPLE_SESSION_H__

#include <string>
#include <memory>

#include <videocore/rtmp/RTMPSession.h>
#include <videocore/transforms/RTMP/AACPacketizer.h>
#include <videocore/transforms/RTMP/H264Packetizer.h>
#include <videocore/transforms/Split.h>
#include <videocore/transforms/AspectTransform.h>
#include <videocore/transforms/PositionTransform.h>

#include <videocore/mixers/Android/AudioMixer.h>
#include <videocore/transforms/Android/H264Encode.h>
#include <videocore/sources/Android/PixelBufferSource.h>
#include <videocore/sources/Android/CameraSource.h>
#include <videocore/sources/Android/MicSource.h>
#include <videocore/mixers/Android/GLESVideoMixer.h>
#include <videocore/transforms/Android/AACEncode.h>
#include <videocore/transforms/Android/H264Encode.h>

#include <videocore/RtmpManager/LibRtmpSessionMgr.hpp>

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

class PixelBufferOutput;

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
    
    std::shared_ptr<PixelBufferOutput> m_pbOutput;
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
    
    std::shared_ptr<videocore::IOutputSession> m_outputSession;
    
    
    // properties
    
    videocore::JobQueue *_graphManagementQueue;
    
    CGSize _videoSize;
    int    _bitrate;
    
    int    _fps;
    int    _bpsCeiling;
    int    _estimatedThroughput;
    
    bool   _useInterfaceOrientation;
    float  _videoZoomFactor;
    int    _audioChannelCount;
    float  _audioSampleRate;
    float  _micGain;
    
    VCCameraState _cameraState;
    VCAspectMode _aspectMode;
    VCSessionState _rtmpSessionState;
    bool   _orientationLocked;
    bool   _torch;
    
    bool _useAdaptiveBitrate;
    bool _continuousAutofocus;
    bool _continuousExposure;
    CGPoint _focusPOI;
    CGPoint _exposurePOI;
    
    VCFilter _filter;
public:
    void setVideoSize(CGSize videoSize);
    void setOrientationLocked(bool orientationLocked);
    void setTorch(bool torch);
    void setAspectMode(VCAspectMode aspectMode);
    void setCameraState(VCCameraState cameraState);
    void setRtmpSessionState(VCSessionState rtmpSessionState);
    void setVideoZoomFactor(float videoZoomFactor);
    void setAudioChannelCount(int channelCount);
    void setAudioSampleRate(float sampleRate);
    void setMicGain(float micGain);
    void setContinuousAutofocus(bool continuousAutofocus);
    void setContinuousExposure(bool continuousExposure);
    void setFocusPointOfInterest(CGPoint focusPointOfInterest);
    void setExposurePointOfInterest(CGPoint exposurePointOfInterest);
    void setUseAdaptiveBitrate(bool useAdaptiveBitrate);
    void setFilter(VCFilter filterToChange);
public:
    VCSimpleSession(void *jvm, void *jcamera);
    void setupGraph();
    void initWithVideoSize(CGSize videoSize, int fps, int bps, bool useInterfaceOrientation, VCCameraState cameraState, VCAspectMode aspectMode);
    void startRtmpSessionWithURL(std::string rtmpUrl);
    void endRtmpSession();
};

#endif //__VC_SIMPLE_SESSION_H__
