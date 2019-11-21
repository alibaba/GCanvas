package com.taobao.gcanvas.bridges.rn;

import android.content.Context;
import android.util.AttributeSet;

import com.facebook.react.bridge.LifecycleEventListener;
import com.taobao.gcanvas.surface.GTextureView;

/**
 * View for ReactNative.
 * @author ertong
 *         create at 2018/1/23
 */

public class GReactTextureView extends GTextureView implements LifecycleEventListener{
    public GReactTextureView(Context context, String id) {
        super(context, id);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs) {
        super(context, id, attrs);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs, int defStyleAttr) {
        super(context, id, attrs, defStyleAttr);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, id, attrs, defStyleAttr, defStyleRes);
    }

    @Override
    public void onHostResume() {
       resume();
    }

    @Override
    public void onHostPause() {
        pause();
    }

    @Override
    public void onHostDestroy() {
        setSurfaceTextureListener(null);
        requestExit();
    }
}
