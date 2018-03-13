package com.taobao.gcanvas.bridges.rn;

import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.uimanager.SimpleViewManager;
import com.facebook.react.uimanager.ThemedReactContext;

import java.util.Map;
import java.util.UUID;

import javax.annotation.Nullable;

/**
 * ReactNative bridge view manager.
 * @author ertong
 *         create at 2018/1/17
 */

public class GReactViewManager extends SimpleViewManager<GReactTextureView> {

    @Override
    public String getName() {
        return "GCanvasView";
    }


    @Override
    protected GReactTextureView createViewInstance(ThemedReactContext reactContext) {
        return new GReactTextureView(reactContext, UUID.randomUUID().toString());
    }

    @Override
    public void receiveCommand(GReactTextureView root, int commandId, @Nullable ReadableArray args) {
        super.receiveCommand(root, commandId, args);
    }

    @Nullable
    @Override
    public Map<String, Integer> getCommandsMap() {
        return super.getCommandsMap();
    }

    @Override
    public void onDropViewInstance(GReactTextureView view) {
        super.onDropViewInstance(view);
    }


}
