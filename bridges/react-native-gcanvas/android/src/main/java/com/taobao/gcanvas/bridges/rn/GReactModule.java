package com.taobao.gcanvas.bridges.rn;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.text.TextUtils;
import android.view.Display;
import android.view.View;

import com.facebook.react.bridge.Callback;
import com.facebook.react.bridge.LifecycleEventListener;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableArray;
import com.taobao.gcanvas.GCanvasJNI;
import com.taobao.gcanvas.adapters.img.impl.fresco.GCanvasFrescoImageLoader;
import com.taobao.gcanvas.bridges.rn.bridge.RNJSCallbackArray;
import com.taobao.gcanvas.bridges.rn.bridge.RNJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.rn.bridge.RNJSCallbackMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.spec.module.AbsGBridgeModule;
import com.taobao.gcanvas.bridges.spec.module.IGBridgeModule;
import com.taobao.gcanvas.util.GLog;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import static com.taobao.gcanvas.bridges.spec.module.IGBridgeModule.ContextType._2D;


/**
 * ReactNative bridge.
 * @author ertong
 */
public class GReactModule extends ReactContextBaseJavaModule implements LifecycleEventListener {
    private static final String NAME = "CanvasModule";

    private static final String TAG = GReactModule.class.getSimpleName();

    private HashMap<String, GReactTextureView> mViews = new HashMap<>();

    @Override
    public void onHostResume() {

    }

    @Override
    public void onHostPause() {
    }

    @Override
    public void onHostDestroy() {
        // release resource
        getReactApplicationContext().removeLifecycleEventListener(this);
        Iterator<Map.Entry<String, GReactTextureView>> iter = mViews.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry<String, GReactTextureView> entry = iter.next();
            GReactTextureView val = entry.getValue();
            val.onHostDestroy();
        }

        mViews.clear();
    }

    private class RNModuleImpl extends AbsGBridgeModule<Callback> {

        private final RNJSCallbackDataFactory mFactory = new RNJSCallbackDataFactory();

        @Override
        public String enable(JSONObject data) {
            try {
                final String refId = data.getString("componentId");
                int viewTag = Integer.parseInt(refId);

                Activity activity = getCurrentActivity();

                if (null == activity) {
                    return Boolean.FALSE.toString();
                }

                View v = activity.findViewById(viewTag);
                if (v instanceof GReactTextureView) {
                    mViews.put(refId, (GReactTextureView) v);
                    return Boolean.TRUE.toString();
                }

                return Boolean.FALSE.toString();
            } catch (Throwable e) {
                e.printStackTrace();
                return Boolean.FALSE.toString();
            }
        }

        @Override
        public void setContextType(String canvasId, ContextType type) {
            GReactTextureView textureView = mViews.get(canvasId);
            if (null == textureView) {
                GLog.e(TAG, "can not find canvas with id ===> " + canvasId);
                return;
            }
            GCanvasJNI.setContextType(textureView.getCanvasKey(), type.value());
        }

        @Override
        public void setDevicePixelRatio(String canvasId, double ratio) {
        }

        @Override
        public void render(String canvasId, String cmd) {
            GReactTextureView textureView = mViews.get(canvasId);
            if (null == textureView) {
                GLog.e(TAG, "can not find canvas with id ===> " + canvasId);
                return;
            }
            GCanvasJNI.render(textureView.getCanvasKey(), cmd);
        }

        @Override
        public Context getContext() {
            return getReactApplicationContext();
        }

        @Override
        protected void invokeCallback(Callback callback, Object args) {
            if (null != callback) {
                if (args instanceof RNJSCallbackArray) {
                    callback.invoke(((RNJSCallbackArray) args).getArray());
                } else if (args instanceof RNJSCallbackMap) {
                    callback.invoke(((RNJSCallbackMap) args).getMap());
                } else {
                    callback.invoke(args);
                }
            }
        }

        @Override
        protected IJSCallbackDataFactory getDataFactory() {
            return mFactory;
        }
    }

    private RNModuleImpl mImpl;

    @ReactMethod
    public void bindImageTexture(final ReadableArray array, final String refId, final Callback callback) {
        if (null == array || TextUtils.isEmpty(refId) || array.size() != 2) {
            return;
        }
        GReactTextureView textureView = mViews.get(refId);
        if (null == textureView) {
            GLog.e(TAG, "can not find canvas with id ===> " + refId);
            return;
        }
        String url = array.getString(0);
        int rid = array.getInt(1);
        mImpl.bindImageTexture(textureView.getCanvasKey(), url, rid, callback);
    }

    @ReactMethod
    public void preLoadImage(ReadableArray args, final Callback callback) {
        if (null == args || args.size() != 2) {
            GLog.d(TAG, "invalid input parameter");
            return;
        }
        try {
            JSONArray array = new JSONArray();
            array.put(args.getString(0));
            array.put(args.getInt(1));
            mImpl.preLoadImage(array, callback);
        } catch (Throwable e) {
            GLog.e(TAG, e.getMessage(), e);
            HashMap<String, String> error = new HashMap<>();
            error.put("error", e.getMessage());
            mImpl.invokeCallback(callback, error);
        }
    }


    @ReactMethod
    public void render(String cmd, String canvasId) {
        mImpl.render(canvasId, cmd);
    }

    @ReactMethod
    public void setLogLevel(int level) {
        GLog.d(TAG, "setLogLevel() args: " + level);
        mImpl.setLogLevel(level);
    }

    @ReactMethod
    public String enable(String args) {
        try {
            JSONObject json = new JSONObject(args);
            return mImpl.enable(json);
        } catch (JSONException e) {
            return Boolean.FALSE.toString();
        }
    }

    @ReactMethod
    public void getDeviceInfo(String args, Callback callBack) {
        if (!TextUtils.isEmpty(args)) {

            HashMap<String, Object> hm = new HashMap<>();

            JSONObject data = new JSONObject();
            try {
                data.put("platform", "Android");
            } catch (JSONException e) {
            }
            hm.put("data", data.toString());
            callBack.invoke(hm);
        }
    }

    @ReactMethod
    public void setContextType(int args, String refId) {
        if (TextUtils.isEmpty(refId)) {
            return;
        }

        Activity ctx = getCurrentActivity();
        if (ctx == null) {
            GLog.e(TAG, "setDevicePixelRatio error ctx == null");
            return;
        }

        Display display = ctx.getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        double devicePixelRatio = size.x * 1.0 / size.y;
//
        GLog.d(TAG, "enable size " + size.toString());
        GLog.d(TAG, "enable devicePixelRatio " + devicePixelRatio);

        mImpl.setDevicePixelRatio(refId, devicePixelRatio);


        IGBridgeModule.ContextType type = _2D;
        try {
            type = IGBridgeModule.ContextType.values()[args];
        } catch (Throwable throwable) {
        }

        mImpl.setContextType(refId, type);
    }


    @ReactMethod
    public void setAlpha(String refId, float alpha) {
    }


    @ReactMethod
    public void texImage2D(final String refid, final int target, final int level, final int internalformat, final int format, final int type, String path) {
        if (!TextUtils.isEmpty(path)) {
            mImpl.texImage2D(refid, target, level, internalformat, format, type, path);
        }
    }

    @ReactMethod
    public void texSubImage2D(final String refid, final int target, final int level, final int xoffset, final int yoffset, final int format, final int type, String path) {
        if (!TextUtils.isEmpty(path)) {
            mImpl.texSubImage2D(refid, target, level, xoffset, yoffset, format, type, path);
        }
    }

    public GReactModule(ReactApplicationContext reactContext) {
        super(reactContext);
        mImpl = new RNModuleImpl();
        mImpl.setImageLoader(new GCanvasFrescoImageLoader());
        getReactApplicationContext().addLifecycleEventListener(this);
    }

    @Override
    public String getName() {
        return NAME;
    }
}
