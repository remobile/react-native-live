package com.remobile.live;

import android.hardware.Camera;
import android.view.Surface;
import android.view.ViewGroup;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class LiveVideoCoreSDK {
    public interface VCSessionDelegate {
        void connectionStatusChanged(int sessionState);
    }
    //VCSessionState
    public static final int VCSessionStateNone = 0;
    public static final int VCSessionStatePreviewStarted = 1;
    public static final int VCSessionStateStarting = 2;
    public static final int VCSessionStateStarted = 3;
    public static final int VCSessionStateEnded = 4;
    public static final int VCSessionStateError = 5;

    //VCCameraState
    public static final int VCCameraStateFront = 0;
    public static final int VCCameraStateBack = 1;

    //VCAspectMode
    public static final int VCAspectModeFit = 0;
    public static final int VCAscpectModeFill = 1;

    //VCFilter
    public static final int VCFilterNormal = 0;
    public static final int VCFilterGray = 1;
    public static final int VCFilterInvertColors = 2;
    public static final int VCFilterSepia = 3;
    public static final int VCFilterBeauty = 4;
    public static final int VCFilterAntique = 5;

    public VCSessionDelegate delegate;
    private VCSimpleSession _livesession;
    private  String _rtmpUrl;
    private String _Dest;
    private String _LiveName;
    private ViewGroup _ShowPreview;
    private float _micGain;


    private static final LiveVideoCoreSDK ourInstance = new LiveVideoCoreSDK();

    public static LiveVideoCoreSDK getInstance() {
        return ourInstance;
    }

    private LiveVideoCoreSDK() {
    }

    public void LiveInit(String url, ViewGroup previewView, int videoSize, int bitRate, int frameRate) {
        _livesession.initWithVideoSize(url, videoSize, bitRate, frameRate);
        _livesession.delegate = this;
        _ShowPreview = previewView;
        previewView.addView(_livesession.previewView);
    }

    public void LiveRelease() {

    }

    public void connect() {
        _livesession.startRtmpSessionWithURL(_Dest, _LiveName);
    }

    public void disconnect() {
        _livesession.endRtmpSession();
    }

}
