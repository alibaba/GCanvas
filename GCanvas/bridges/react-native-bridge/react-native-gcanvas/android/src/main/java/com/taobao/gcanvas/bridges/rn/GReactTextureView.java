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

public class GReactTextureView extends GTextureView implements LifecycleEventListener, TextureView.SurfaceTextureListener {
    private boolean mIsReady = false;

    public GReactTextureView(Context context, String id) {
        super(context, id);
        addSurfaceTextureListener(this);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs) {
        super(context, id, attrs);
        addSurfaceTextureListener(this);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs, int defStyleAttr) {
        super(context, id, attrs, defStyleAttr);
        addSurfaceTextureListener(this);
    }

    public GReactTextureView(Context context, String id, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, id, attrs, defStyleAttr, defStyleRes);
        addSurfaceTextureListener(this);
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

    public boolean isReady() {
        return mIsReady;
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mIsReady = true;
        Log.e("test", "onSurfaceTextureAvailable=========");
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mIsReady = false;
        Log.e("test", "onSurfaceTextureDestroyed=========");
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }
}
