package com.remobile.live;

import com.facebook.react.uimanager.*;
import com.facebook.react.uimanager.annotations.ReactProp;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import javax.annotation.Nullable;

public class RCTLiveViewManager extends ViewGroupManager<RCTLiveView> {
    @Override
    public String getName() {
        return "RCTLive";
    }

    @Override
    public RCTLiveView createViewInstance(ThemedReactContext context) {
        return new RCTLiveView(context);
    }
}
