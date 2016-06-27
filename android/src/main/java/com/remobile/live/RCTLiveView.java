package com.remobile.live;

import android.content.Context;
import android.hardware.SensorManager;
import android.view.OrientationEventListener;
import android.view.ViewGroup;
import android.view.WindowManager;

public class RCTLiveView extends ViewGroup implements LiveVideoCoreSDK.VCSessionDelegate {
    private final Context _context;
    private LiveVideoCoreSDK sdk;
    private String url;
    private  boolean horizontal;
    private int videoSize; //0:cif 1:d1 2:720p
    private int frameRate;
    private int bitRate;

    public RCTLiveView(Context context) {
        super(context);
        this._context = context;
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {

    }

    private void start() {
        sdk.LiveInit(url, this, videoSize, bitRate, frameRate);
        sdk.delegate = this;
        sdk.connect();
    }

    private void stop() {
        sdk.disconnect();
        sdk.LiveRelease();
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        start();
    }

    @Override
    protected void onDetachedFromWindow() {
        stop();
        super.onDetachedFromWindow();
    }

    @Override
    public void connectionStatusChanged(int sessionState) {

    }
}
