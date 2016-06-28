package com.remobile.live;

import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;
import android.view.ViewGroup;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class LiveVideoCoreSDK {
    private RCTCamera _camera;

    public interface VCSessionDelegate {
        void connectionStatusChanged(int sessionState);
    }

    static {
        System.loadLibrary("live");
    }

    public static native void LiveInit(String url, Object camera, Object previewView, int videoSize, int bitRate, int frameRate, VCSessionDelegate delegate);
    public static native void LiveRelease();

    public LiveVideoCoreSDK() {
        _camera = new RCTCamera();
    }

    public void LiveInit(String url, ViewGroup previewView, int videoSize, int bitRate, int frameRate, VCSessionDelegate delegate)
    {
        LiveInit(url, _camera, previewView, videoSize, bitRate, frameRate, delegate);
    }


}
