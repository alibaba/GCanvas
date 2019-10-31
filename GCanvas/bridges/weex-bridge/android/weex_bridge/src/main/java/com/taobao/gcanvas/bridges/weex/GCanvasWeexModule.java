package com.taobao.gcanvas.bridges.weex;

import android.app.Activity;
import android.content.Context;
import android.text.TextUtils;
import android.view.Display;

import com.alibaba.weex.plugin.annotation.WeexModule;
import com.taobao.gcanvas.GCanvasJNI;
import com.taobao.gcanvas.adapters.img.impl.fresco.GCanvasFrescoImageLoader;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.spec.module.AbsGBridgeModule;
import com.taobao.gcanvas.bridges.spec.module.IGBridgeModule;
import com.taobao.gcanvas.bridges.weex.bridge.WeexJSCallbackDataFactory;
import com.taobao.gcanvas.surface.GTextureView;
import com.taobao.gcanvas.util.GLog;
import com.taobao.weex.WXEnvironment;
import com.taobao.weex.WXSDKManager;
import com.taobao.weex.annotation.JSMethod;
import com.taobao.weex.bridge.JSCallback;
import com.taobao.weex.common.Destroyable;
import com.taobao.weex.common.WXModule;
import com.taobao.weex.ui.component.WXComponent;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import static com.taobao.gcanvas.bridges.spec.module.IGBridgeModule.ContextType._2D;


/**
 * @author ertong
 *         create at 2017/8/17
 */

@WeexModule(name = "gcanvas")
public class GCanvasWeexModule extends WXModule implements Destroyable {
    private static final String TAG = "GCanvasWeexModule";

    private HashMap<String, WXGCanvasWeexComponent> mComponentMap = new HashMap<>(1);

    private WeexModuleImpl mImpl;

    private static class WeexModuleImpl extends AbsGBridgeModule<JSCallback> {

        private WeakReference<GCanvasWeexModule> mOutRef;

        private WeexJSCallbackDataFactory mFactory = new WeexJSCallbackDataFactory();

        public WeexModuleImpl(GCanvasWeexModule module) {
            this.mOutRef = new WeakReference<>(module);
        }

        @Override
        public String enable(JSONObject data) {
            GCanvasWeexModule module = this.mOutRef.get();
            if (null == module) {
                return Boolean.FALSE.toString();
            }

            try {
                String refId = data.getString("componentId");
                WXComponent myComponent = WXSDKManager.getInstance().getWXRenderManager().getWXComponent(module.mWXSDKInstance.getInstanceId(), refId);

                if (myComponent instanceof WXGCanvasWeexComponent) {
                    WXGCanvasWeexComponent mWXGCanvasComp = (WXGCanvasWeexComponent) myComponent;
                    module.mComponentMap.put(refId, mWXGCanvasComp);
                }
                return Boolean.TRUE.toString();
            } catch (JSONException e) {
                return Boolean.FALSE.toString();
            }
        }

        @Override
        public void setContextType(String canvasId, ContextType type) {
            GCanvasWeexModule module = this.mOutRef.get();
            if (null == module) {
                return;
            }

            Display display = ((Activity) getContext()).getWindowManager().getDefaultDisplay();

            int width = display.getWidth();
            double devicePixelRatio = width / 750.0;

            GLog.d(TAG, "enable width " + width);
            GLog.d(TAG, "enable devicePixelRatio " + devicePixelRatio);

            /**
             * open high Quality default
             */
            GCanvasJNI.setWrapperHiQuality(canvasId, true);
            GCanvasJNI.setWrapperDevicePixelRatio(canvasId, devicePixelRatio);
            GCanvasJNI.setWrapperContextType(canvasId, type.value());

            if (GCanvasJNI.sendEvent(canvasId)) {
                GLog.d("start to send event in module.");
                WXGCanvasWeexComponent component = module.mComponentMap.get(canvasId);
                if (component != null) {
                    component.sendEvent();
                }
            }

            WXGCanvasWeexComponent component = module.mComponentMap.get(canvasId);
            if (component != null) {
                component.mType = type;
            }
        }

        @Override
        public void setDevicePixelRatio(String canvasId, double ratio) {

        }

        @Override
        public void render(String canvasId, String cmd) {

        }

        @Override
        public Context getContext() {
            GCanvasWeexModule module = this.mOutRef.get();
            if (null == module) {
                return null;
            }
            return module.mWXSDKInstance.getContext();
        }

        @Override
        protected void invokeCallback(JSCallback jsCallback, Object args) {
            if (null != jsCallback) {
                jsCallback.invoke(args);
            }
        }

        @Override
        protected IJSCallbackDataFactory getDataFactory() {
            return mFactory;
        }
    }

    public GCanvasWeexModule() {
        GCanvasJNI.registerWXCallNativeFunc(WXEnvironment.getApplication());
        mImpl = new WeexModuleImpl(this);
        mImpl.setImageLoader(new GCanvasFrescoImageLoader());
    }


    @JSMethod(uiThread = false)
    public void bindImageTexture(final String args, final String refId, final JSCallback callback) {
        String url = null;
        int rid = 0;
        if (!TextUtils.isEmpty(args)) {
            try {
                JSONArray dataArray = new JSONArray(args);
                url = dataArray.getString(0);
                rid = dataArray.getInt(1);
            } catch (Throwable e) {
                GLog.e(TAG, e.getMessage(), e);
            }
        }

        final String src = url;
        final int id = rid;
        mImpl.bindImageTexture(refId, src, id, callback);
    }

    @JSMethod(uiThread = false)
    public void resetComponent(String refId) {
    }

    @JSMethod(uiThread = false)
    public void setup(String args, String refId, JSCallback callback) {
    }


    @JSMethod(uiThread = false)
    public void preLoadImage(String args, final JSCallback callBack) {
        GLog.d(TAG, "preLoadImage() in GCanvasWeexModule,args: " + args);
        if (!TextUtils.isEmpty(args)) {
            try {
                JSONArray dataArray = new JSONArray(args);
                mImpl.preLoadImage(dataArray, callBack);
            } catch (Throwable e) {
                GLog.e(TAG, e.getMessage(), e);
            }
        }
    }

    @JSMethod(uiThread = false)
    public void setHiQuality(String args, String refId) {
    }

    @JSMethod(uiThread = false)
    public void setLogLevel(String args) {
        GLog.d(TAG, "setLogLevel() args: " + args);
        int level = 0;
        try {
            level = Integer.parseInt(args);
        } catch (Throwable throwable) {
            // for old version compatiblity
            switch (args) {
                case "debug":
                    level = 0;
                    break;

                case "info":
                    level = 1;
                    break;

                case "warn":
                    level = 2;
                    break;

                case "error":
                    level = 3;
                    break;
            }
        }
        mImpl.setLogLevel(level);
    }


    @JSMethod(uiThread = false)
    public String enable(String args) {
        try {
            JSONObject jo = new JSONObject(args);
            return mImpl.enable(jo);
        } catch (Throwable e) {
            return Boolean.FALSE.toString();
        }
    }

    @JSMethod(uiThread = false)
    public void render(String cmd, String refId) {
        // Inject the render function "GCanvasCallNative" to weex core .so, empty implementation here.
    }

    @JSMethod(uiThread = false)
    public void getDeviceInfo(String args, JSCallback callBack) {
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

    @JSMethod(uiThread = false)
    public void setContextType(String args, String refId) {
        if (TextUtils.isEmpty(args) || TextUtils.isEmpty(refId)) {
            return;
        }

        Context ctx = (mWXSDKInstance.getContext());
        if (ctx == null) {
            GLog.e(TAG, "setDevicePixelRatio error ctx == null");
            return;
        }

        /**
         * Weex has 750 dpi width.
         */
        Display display = ((Activity) ctx).getWindowManager().getDefaultDisplay();

        int width = display.getWidth();
        double devicePixelRatio = width / 750.0;

        GLog.d(TAG, "enable width " + width);
        GLog.d(TAG, "enable devicePixelRatio " + devicePixelRatio);

        IGBridgeModule.ContextType type = _2D;

        if ("3d".equals(args) || "1".equals(args)) {
            type = IGBridgeModule.ContextType._3D;
        }
        mImpl.setContextType(refId, type);
    }

    @JSMethod(uiThread = false)
    public void setAlpha(String refId, float alpha) {
        GLog.d("start to set alpha in 3dmodule.");
        WXGCanvasWeexComponent component = mComponentMap.get(refId);
        if (component != null) {
            GTextureView view = component.getSurfaceView();
            if (view != null) {
                GLog.d("set alpha success in 3dmodule.");
                view.setAlpha(alpha);
            }
        }
    }

    @JSMethod(uiThread = false)
    public void setDevicePixelRatio(String refId) {
    }

    @JSMethod(uiThread = false)
    public String execGcanvaSyncCMD(String refId, String action, String args) {
        return "";
    }

    @Override
    public void destroy() {
        Iterator iter = mComponentMap.entrySet().iterator();
        while (iter.hasNext()) {
            Map.Entry entry = (Map.Entry) iter.next();
            WXGCanvasWeexComponent val = (WXGCanvasWeexComponent) entry.getValue();
            GLog.d("component destroy id=" + entry.getKey());

            val.onActivityDestroy();
        }

        mComponentMap.clear();
    }

    @JSMethod(uiThread = false)
    public void texImage2D(final String refid, final int target, final int level, final int internalformat, final int format, final int type, String path) {
        if (!TextUtils.isEmpty(path)) {
            mImpl.texImage2D(refid, target, level, internalformat, format, type, path);
        }
    }

    @JSMethod(uiThread = false)
    public void texSubImage2D(final String refid, final int target, final int level, final int xoffset, final int yoffset, final int format, final int type, String path) {
        if (!TextUtils.isEmpty(path)) {
            mImpl.texSubImage2D(refid, target, level, xoffset, yoffset, format, type, path);
        }
    }
}
