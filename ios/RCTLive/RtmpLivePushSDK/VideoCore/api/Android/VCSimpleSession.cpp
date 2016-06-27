/*

 Video Core
 Copyright (c) 2014 James G. Hurley

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

#import <videocore/api/iOS/VCSimpleSession.h>
#import <videocore/api/iOS/VCPreviewView.h>

#include <videocore/rtmp/RTMPSession.h>
#include <videocore/transforms/RTMP/AACPacketizer.h>
#include <videocore/transforms/RTMP/H264Packetizer.h>
#include <videocore/transforms/Split.h>
#include <videocore/transforms/AspectTransform.h>
#include <videocore/transforms/PositionTransform.h>

#include <videocore/mixers/Apple/AudioMixer.h>
#include <videocore/transforms/Apple/MP4Multiplexer.h>
#include <videocore/transforms/Apple/H264Encode.h>
#include <videocore/sources/Apple/PixelBufferSource.h>
#include <videocore/sources/iOS/CameraSource.h>
#include <videocore/sources/iOS/MicSource.h>
#include <videocore/mixers/iOS/GLESVideoMixer.h>
#include <videocore/transforms/iOS/AACEncode.h>
#include <videocore/transforms/iOS/H264Encode.h>
#include <videocore/mixers/GenericAudioMixer.h>

#include "LibRtmpSessionMgr.hpp"

static const int kMinVideoBitrate = 32000;

namespace videocore { namespace simpleApi {

    using PixelBufferCallback = std::function<void(const uint8_t* const data, size_t size)>;

    class PixelBufferOutput : public IOutput
    {
    public:
        PixelBufferOutput(PixelBufferCallback callback)
        : m_callback(callback) {};

        void pushBuffer(const uint8_t* const data,
                        size_t size,
                        IMetadata& metadata)
        {
            m_callback(data, size);
        }

    private:

        PixelBufferCallback m_callback;
    };
}}

// -----------------------------------------------------------------------------
//  VCSessionDelegate Methods
// -----------------------------------------------------------------------------
void VCSessionDelegate::didAddCameraSource(VCSimpleSession *session) {
}
void VCSessionDelegate::detectedThroughput(int throughputInBytesPerSecond, int videoRate) {
}

// -----------------------------------------------------------------------------
//  Properties Methods
// -----------------------------------------------------------------------------

VCSimpleSession::VCSimpleSession()
:_graphManagementQueue("com.videocore.session.graph", 0)
{
}

void VCSimpleSession::setVideoSize(CGSize videoSize)
{
    _videoSize = videoSize;
    if(m_aspectTransform) {
        m_aspectTransform->setBoundingSize(videoSize.width, videoSize.height);
    }
    if(m_positionTransform) {
        m_positionTransform->setSize(videoSize.width * _videoZoomFactor, videoSize.height * _videoZoomFactor);
    }
}

void VCSimpleSession::setOrientationLocked(BOOL orientationLocked)
{
    _orientationLocked = orientationLocked;
    if(m_cameraSource) {
        m_cameraSource->setOrientationLocked(orientationLocked);
    }
}

void VCSimpleSession::setTorch(BOOL torch)
{
    if(m_cameraSource) {
        _torch = m_cameraSource->setTorch(torch);
    }
}

void VCSimpleSession::setAspectMode(VCAspectMode aspectMode)
{
    _aspectMode = aspectMode;
    switch (aspectMode) {
        case VCAscpectModeFill:
            m_aspectMode = videocore::AspectTransform::AspectMode::kAspectFill;
            break;
        case VCAspectModeFit:
            m_aspectMode = videocore::AspectTransform::AspectMode::kAspectFit;
            break;
        default:
            break;
    }
}
void VCSimpleSession::setCameraState(VCCameraState cameraState)
{
    if(_cameraState != cameraState) {
        _cameraState = cameraState;
        if(m_cameraSource) {
            m_cameraSource->toggleCamera();
        }
    }
}

void VCSimpleSession::setRtmpSessionState(VCSessionState rtmpSessionState)
{
    _rtmpSessionState = rtmpSessionState;
    if(delegate) {
        delegate->connectionStatusChanged(rtmpSessionState);
    }
}

void VCSimpleSession::setVideoZoomFactor(float videoZoomFactor)
{
    _videoZoomFactor = videoZoomFactor;
    if(m_positionTransform) {
        // We could use AVCaptureConnection's zoom factor, but in reality it's
        // doing the exact same thing as this (in terms of the algorithm used),
        // but it is not clear how CoreVideo accomplishes it.
        // In this case this is just modifying the matrix
        // multiplication that is already happening once per frame.
        m_positionTransform->setSize(_videoSize.width * videoZoomFactor,
                                     _videoSize.height * videoZoomFactor);
    }
}

void VCSimpleSession::setAudioChannelCount(int channelCount)
{
    _audioChannelCount = MIN(2, MAX(channelCount,2)); // We can only support a channel count of 2 with AAC

    if(m_audioMixer) {
        m_audioMixer->setChannelCount(channelCount);
    }
}

void VCSimpleSession::setAudioSampleRate(float sampleRate)
{
    _audioSampleRate = (sampleRate > 33075 ? 44100 : 22050); // We can only support 44100 / 22050 with AAC + RTMP
    if(m_audioMixer) {
        m_audioMixer->setFrequencyInHz(sampleRate);
    }
}

void VCSimpleSession::setMicGain(float micGain)
{
    if(m_audioMixer) {
        m_audioMixer->setSourceGain(m_micSource, micGain);
        _micGain = micGain;
    }
}

void* getPreviewView()
{
    return _previewView;
}


void VCSimpleSession::setContinuousAutofocus(BOOL continuousAutofocus)
{
    _continuousAutofocus = continuousAutofocus;
    if( m_cameraSource ) {
        m_cameraSource->setContinuousAutofocus(continuousAutofocus);
    }
}

void VCSimpleSession::setContinuousExposure(BOOL continuousExposure)
{
    _continuousExposure = continuousExposure;
    if(m_cameraSource) {
        m_cameraSource->setContinuousExposure(continuousExposure);
    }
}

void VCSimpleSession::setFocusPointOfInterest(CGPoint focusPointOfInterest)
{
    _focusPOI = focusPointOfInterest;

    if(m_cameraSource) {
        m_cameraSource->setFocusPointOfInterest(focusPointOfInterest.x, focusPointOfInterest.y);
    }
}

void VCSimpleSession::setExposurePointOfInterest(CGPoint exposurePointOfInterest)
{
    _exposurePOI = exposurePointOfInterest;
    if(m_cameraSource) {
        m_cameraSource->setExposurePointOfInterest(exposurePointOfInterest.x, exposurePointOfInterest.y);
    }
}

void VCSimpleSession::setUseAdaptiveBitrate(BOOL useAdaptiveBitrate)
{
    _useAdaptiveBitrate = useAdaptiveBitrate;
    _bpsCeiling = _bitrate;
}

// -----------------------------------------------------------------------------
//  Public Methods
// -----------------------------------------------------------------------------
void VCSimpleSession::initWithVideoSize(int width, int height, int fps, int bps, BOOL useInterfaceOrientation,
                        VCCameraState cameraState, VCAspectMode aspectMode)
{
    _bps = bps;
    setVideoSize(width, height);
    _fps = fps;
    _useInterfaceOrientation = useInterfaceOrientation;
    setMicGain(1.f);
    setAudioChannelCount(2);
    setAudioSampleRate(44100.);
    setUseAdaptiveBitrate(NO);
    setAspectMode(aspectMode);
    setVideoZoomFactor(1.f);

    setCameraState(cameraState);
    _exposurePOI = _focusPOI = CGPointMake(0.5f, 0.5f);
    _continuousExposure = _continuousAutofocus = YES;
    
    _graphManagementQueue = new JobQueue();
    _graphManagementQueue->enqueue_sync([=]{
        setupGraph();
    });
}

void VCSimpleSession::release()
{
    endRtmpSession();
    m_audioMixer.reset();
    m_videoMixer.reset();
    m_videoSplit.reset();
    m_aspectTransform.reset();
    m_positionTransform.reset();
    m_micSource.reset();
    m_cameraSource.reset();
    m_pbOutput.reset();
    
    delete _graphManagementQueue;
}

void VCSimpleSession::startRtmpSessionWithURL(std::string rtmpUrl)
{
    _graphManagementQueue->enqueue_sync([=]{
        startSessionInternal(rtmpUrl);
    });
}

void VCSimpleSession::startSessionInternal(std::string rtmpUrl)
{
    m_outputSession.reset(new videocore::LibRtmpSessionMgr (rtmpUrl, [=](videocore::LibRtmpSessionMgr& session, ClientState_t state) {
        DLog("ClientState: %d\n", state);
        switch(state) {
            case kClientStateConnected:
                rtmpSessionState = VCSessionStateStarting;
                break;
            case kClientStateHandshake0:
                rtmpSessionState = VCSessionStatePreviewStarted;
                break;
            case kClientStateSessionStarted:
            {
                _graphManagementQueue->enqueue_sync([=]{
                    addEncodersAndPacketizers();
                });
            }
                rtmpSessionState = VCSessionStateStarted;
                break;
            case kClientStateError:
                rtmpSessionState = VCSessionStateError;
                endRtmpSession();
                m_outputSession.reset();
                break;
            case kClientStateNotConnected:
                rtmpSessionState = VCSessionStateEnded;
                endRtmpSession();
                break;
            default:
                break;
        }
    }));
    _bpsCeiling = _bitrate;
    if ( useAdaptiveBitrate ) {
        _bitrate = 500000;
    }
    m_outputSession->setBandwidthCallback([=](float vector, float predicted, int inst) {
        _estimatedThroughput = predicted;
        auto video = std::dynamic_pointer_cast<videocore::IEncoder>(m_h264Encoder );
        auto audio = std::dynamic_pointer_cast<videocore::IEncoder>(m_aacEncoder );
        if(video && audio && _useAdaptiveBitrate) {
            if (delegate) {
                delegate->detectedThroughput(predicted);
                delegate->detectedThroughput(predicted, video->bitrate());
            }
            int videoBr = 0;
            if(vector != 0) {
                vector = vector < 0 ? -1 : 1 ;
                videoBr = video->bitrate();
                if (audio) {
                    if ( videoBr > 500000 ) {
                        audio->setBitrate(128000);
                    } else if (videoBr <= 500000 && videoBr > 250000) {
                        audio->setBitrate(96000);
                    } else {
                        audio->setBitrate(80000);
                    }
                }
                if(videoBr > 1152000) {
                    video->setBitrate(std::min(int((videoBr / 384000 + vector )) * 384000, bSelf->_bpsCeiling) );
                } else if( videoBr > 512000 ) {
                    video->setBitrate(std::min(int((videoBr / 128000 + vector )) * 128000, bSelf->_bpsCeiling) );
                } else if( videoBr > 128000 ) {
                    video->setBitrate(std::min(int((videoBr / 64000 + vector )) * 64000, bSelf->_bpsCeiling) );
                } else {
                    video->setBitrate(std::max(std::min(int((videoBr / 32000 + vector )) * 32000, bSelf->_bpsCeiling), kMinVideoBitrate) );
                }
                DLog("\n(%f) AudioBR: %d VideoBR: %d (%f)\n", vector, audio->bitrate(), video->bitrate(), predicted);
            } /* if(vector != 0) */
            
        } /* if(video && audio && m_adaptiveBREnabled) */
    });

    videocore::LibRTMPSessionParameters_t sp(0.);
    sp.setData(_videoSize.width,
               _videoSize.height,
               1. / static_cast<double>(fps),
               _bitrate,
               _audioSampleRate,
               (_audioChannelCount == 2));

    m_outputSession->setSessionParameters(sp);
}

void VCSimpleSession::endRtmpSession()
{
    if(NULL == m_outputSession){
        return;
    }
    m_outputSession->setEndFlag(1);
    
    m_h264Packetizer.reset();
    m_aacPacketizer.reset();
    m_videoSplit->removeOutput(m_h264Encoder);
    m_h264Encoder.reset();
    m_aacEncoder.reset();

    m_outputSession.reset();

    _bitrate = _bpsCeiling;

    rtmpSessionState = VCSessionStateEnded;
}

//Set property filter for the new enum + set dynamically the sourceFilter for the video mixer
void VCSimpleSession::setFilter(VCFilter filterToChange)
{
    std::string filterName = "com.videocore.filters.bgra";
    
    switch (filterToChange) {
        case VCFilterNormal:
            filterName = "com.videocore.filters.bgra";
            break;
        case VCFilterGray:
            filterName = "com.videocore.filters.grayscale";
            break;
        case VCFilterInvertColors:
            filterName = "com.videocore.filters.invertColors";
            break;
        case VCFilterSepia:
            filterName = "com.videocore.filters.sepia";
            break;
        case VCFilterBeauty:
            filterName = "com.videocore.filters.beauty";
            break;
        case VCFilterAntique:
            filterName = "com.videocore.filters.antique";
            break;
        default:
            break;
    }
    
    _filter = filterToChange;
    NSLog(@"FILTER IS : [%d]", (int)_filter);
    m_videoMixer->setSourceFilter(m_cameraSource, dynamic_cast<videocore::IVideoFilter*>(m_videoMixer->filterFactory().filter(filterName))); // default is com.videocore.filters.bgra
}

// -----------------------------------------------------------------------------
//  Private Methods
// -----------------------------------------------------------------------------

void setupGraph()
{
    const double frameDuration = 1. / static_cast<double>(fps);

    {
        // Add audio mixer
        const double aacPacketTime = 1024. / audioSampleRate;

        m_audioMixer = std::make_shared<videocore::Apple::AudioMixer>(audioChannelCount,
                                                                      audioSampleRate,
                                                                      16,
                                                                      aacPacketTime);


        // The H.264 Encoder introduces about 2 frames of latency, so we will set the minimum audio buffer duration to 2 frames.
        m_audioMixer->setMinimumBufferDuration(frameDuration*2);
    }
    {
        // Add video mixer
        m_videoMixer = std::make_shared<videocore::iOS::GLESVideoMixer>(videoSize.width,
                                                                        videoSize.height,
                                                                        frameDuration);

    }

    {
        auto videoSplit = std::make_shared<videocore::Split>();

        m_videoSplit = videoSplit;
        VCPreviewView* preview = (VCPreviewView*)previewView;

        m_pbOutput = std::make_shared<videocore::simpleApi::PixelBufferOutput>([=](const void* const data, size_t size){
            CVPixelBufferRef ref = (CVPixelBufferRef)data;
            [preview drawFrame:ref];
            if(rtmpSessionState == VCSessionStateNone) {
                rtmpSessionState = VCSessionStatePreviewStarted;
            }
        });

        videoSplit->setOutput(m_pbOutput);

        m_videoMixer->setOutput(videoSplit);

    }

    // Create sources
    {
        // Add camera source
        m_cameraSource = std::make_shared<videocore::iOS::CameraSource>();
        m_cameraSource->setOrientationLocked(orientationLocked);
        auto aspectTransform = std::make_shared<videocore::AspectTransform>(_videoSize.width, _videoSize.height,m_aspectMode);

        auto positionTransform = std::make_shared<videocore::PositionTransform>(_videoSize.width/2, _videoSize.height/2,
                                                                                _videoSize.width * videoZoomFactor, _videoSize.height * videoZoomFactor,
                                                                                _videoSize.width, _videoSize.height
                                                                                );


        std::dynamic_pointer_cast<videocore::iOS::CameraSource>(m_cameraSource)->setupCamera(fps,(cameraState == VCCameraStateFront),useInterfaceOrientation, NULL,^{
            m_cameraSource->setContinuousAutofocus(true);
            m_cameraSource->setContinuousExposure(true);

            m_cameraSource->setOutput(aspectTransform);

            m_videoMixer->setSourceFilter(m_cameraSource, dynamic_cast<videocore::IVideoFilter*>(m_videoMixer->filterFactory().filter("com.videocore.filters.bgra")));
            _filter = VCFilterNormal;
            aspectTransform->setOutput(positionTransform);
            positionTransform->setOutput(m_videoMixer);
            m_aspectTransform = aspectTransform;
            m_positionTransform = positionTransform;

            // Inform delegate that camera source has been added
            if (delegate) {
                delegate->didAddCameraSource(this);
            }
        });
    }
    {
        // Add mic source
        m_micSource = std::make_shared<videocore::iOS::MicSource>(audioSampleRate, audioChannelCount);
        m_micSource->setOutput(m_audioMixer);

        const auto epoch = std::chrono::steady_clock::now();

        m_audioMixer->setEpoch(epoch);
        m_videoMixer->setEpoch(epoch);

        m_audioMixer->start();
        m_videoMixer->start();
    }
}

void addEncodersAndPacketizers()
{
    int ctsOffset = 2000 / fps; // 2 * frame duration
    {
        // Add encoders

        m_aacEncoder = std::make_shared<videocore::iOS::AACEncode>(audioSampleRate, audioChannelCount, 96000);
        if(SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"8.0")) {
            // If >= iOS 8.0 use the VideoToolbox encoder that does not write to disk.
            m_h264Encoder = std::make_shared<videocore::Apple::H264Encode>(videoSize.width,
                                                                           videoSize.height,
                                                                           fps,
                                                                           bitrate,
                                                                           false,
                                                                           ctsOffset);
        } else {
            m_h264Encoder =std::make_shared<videocore::iOS::H264Encode>(videoSize.width,
                                                                        videoSize.height,
                                                                        fps,
                                                                        bitrate);
        }
        m_audioMixer->setOutput(m_aacEncoder);
        m_videoSplit->setOutput(m_h264Encoder);

    }
    {
        m_aacSplit = std::make_shared<videocore::Split>();
        m_h264Split = std::make_shared<videocore::Split>();
        m_aacEncoder->setOutput(m_aacSplit);
        m_h264Encoder->setOutput(m_h264Split);

    }
    {
        m_h264Packetizer = std::make_shared<videocore::rtmp::H264Packetizer>(ctsOffset);
        m_aacPacketizer = std::make_shared<videocore::rtmp::AACPacketizer>(audioSampleRate, audioChannelCount, ctsOffset);

        m_h264Split->setOutput(m_h264Packetizer);
        m_aacSplit->setOutput(m_aacPacketizer);

    }

    m_h264Packetizer->setOutput(m_outputSession);
    m_aacPacketizer->setOutput(m_outputSession);
}
