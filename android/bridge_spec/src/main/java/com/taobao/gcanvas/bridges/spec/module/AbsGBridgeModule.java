package com.taobao.gcanvas.bridges.spec.module;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.text.TextUtils;
import android.util.Log;

import com.taobao.gcanvas.GCanvasJNI;
import com.taobao.gcanvas.adapters.img.IGImageLoader;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;
import com.taobao.gcanvas.util.GLog;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Abstract bridge module.
 * @author ertong
 */
public abstract class AbsGBridgeModule<JSCallback> implements IGBridgeModule<JSCallback> {

    public static final String TAG = AbsGBridgeModule.class.getSimpleName();

    protected HashMap<String, GImageLoadInfo> mImageIdCache = new HashMap<>();

    protected HashMap<String, ArrayList<JSCallback>> mCallbacks = new HashMap<>();

    protected IGImageLoader mImageLoader;

    public void setImageLoader(IGImageLoader mImageLoader) {
        this.mImageLoader = mImageLoader;
    }

    private abstract static class AbsImageCallback implements IGImageLoader.ImageCallback {

        private final Object sync = new Object();
        final AtomicBoolean finished = new AtomicBoolean(false);


        @Override
        public void onSuccess(Bitmap bitmap) {

            doSuccessAction(bitmap);

            synchronized (sync) {
                sync.notifyAll();
                finished.set(true);
            }
        }

        @Override
        public void onFail(Object error) {
            synchronized (sync) {
                sync.notifyAll();
                finished.set(true);
            }
        }

        @Override
        public void onCancel() {
            synchronized (sync) {
                sync.notifyAll();
                finished.set(true);
            }
        }

        protected void waitTillFinish() {
            synchronized (sync) {
                while (!finished.get()) {
                    try {
                        sync.wait();
                    } catch (InterruptedException e) {
                        break;
                    }
                }
            }
        }

        protected abstract void doSuccessAction(Bitmap bitmap);
    }

    @Override
    public void bindImageTexture(final String canvasId, final String src, final int id, final JSCallback callback) {
        if (!TextUtils.isEmpty(src)) {
            try {
                if (src.startsWith("data:image")) {
                    Bitmap bmp = handleBase64Texture(src.substring(src.indexOf("base64,") + "base64,".length()));
                    if (bmp != null) {
                        GCanvasJNI.bindTexture(canvasId, bmp, 0, GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE);
                    } else {
                        GLog.d("decode base64 texture failed,bitmap is null.");
                    }

                } else {
                    final IJSCallbackMap callbackMap = getDataFactory().createJSCallbackMap();
                    AbsImageCallback imgCb = new AbsImageCallback() {
                        @Override
                        protected void doSuccessAction(Bitmap bitmap) {
                            if (null != bitmap) {
                                GCanvasJNI.bindTexture(canvasId, bitmap, id, GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE);
                            } else {
                                GLog.d("bitmap is null in teximage2D.");
                            }

                            if (null != callback && bitmap != null) {
                                callbackMap.putString("url", src);
                                callbackMap.putInt("width", bitmap.getWidth());
                                callbackMap.putInt("height", bitmap.getHeight());
                            }
                        }
                    };
                    mImageLoader.load(getContext(), src, imgCb);

                    imgCb.waitTillFinish();

                    invokeCallback(callback, callbackMap);
                }
            } catch (Throwable e) {
                GLog.e(TAG, e.getMessage(), e);
            }
        }
    }

    @Override
    public void setLogLevel(int level) {
        String logLevel = "debug";
        // 0-debug;1-info;2-warn;3-error;
        try {
            switch (level) {
                case 0:
                    logLevel = "debug";
                    break;
                case 1:
                    logLevel = "info";
                    break;
                case 2:
                    logLevel = "warn";
                    break;
                case 3:
                    logLevel = "error";
                    break;
            }
        } catch (NumberFormatException ex) {

        }
        GCanvasJNI.setLogLevel(logLevel);
    }

    @Override
    public void texSubImage2D(final String canvasid, final int target, final int level, final int xoffset, final int yoffset,
                              final int format, final int type, String path) {
        if (!TextUtils.isEmpty(path)) {
            try {
                if (path.startsWith("data:image")) {
                    Bitmap bmp = handleBase64Texture(path.substring(path.indexOf("base64," + "base64,".length())));
                    if (bmp != null) {
                        GCanvasJNI.texSubImage2D(canvasid, bmp, 0, target, level, xoffset, yoffset, format, type);
                    } else {
                        GLog.d("[texSubImage2D] decode base64 texture failed,bitmap is null.");
                    }
                } else {
                    AbsImageCallback imgCb = new AbsImageCallback() {
                        @Override
                        protected void doSuccessAction(Bitmap bitmap) {
                            if (null != bitmap) {
                                GLog.d("[texSubImage2D] start to bindtexture in 3dmodule.");
                                GCanvasJNI.texSubImage2D(canvasid, bitmap, 0, target, level, xoffset, yoffset, format, type);
                            } else {
                                GLog.d("[texSubImage2D] bitmap is null.");
                            }
                        }
                    };

                    mImageLoader.load(getContext(), path, imgCb);
                    imgCb.waitTillFinish();
                }
            } catch (Throwable e) {
                GLog.e(TAG, e.getMessage(), e);
            }
        }
    }

    @Override
    public void texImage2D(final String canvasId, final int target, final int level, final int internalformat,
                           final int format, final int type, final String path) {
        if (!TextUtils.isEmpty(path)) {
            try {
                if (path.startsWith("data:image")) {
                    Bitmap bmp = handleBase64Texture(path.substring(path.indexOf("base64,") + "base64,".length()));
                    if (bmp != null) {
                        GCanvasJNI.bindTexture(canvasId, bmp, 0, target, level, internalformat, format, type);
                    } else {
                        GLog.d("decode base64 texture failed,bitmap is null.");
                    }
                } else {

                    AbsImageCallback imgCb = new AbsImageCallback() {
                        @Override
                        protected void doSuccessAction(Bitmap bitmap) {
                            if (null != bitmap) {
                                GCanvasJNI.bindTexture(canvasId, bitmap, 0, target, level, internalformat, format, type);
                            } else {
                                GLog.d("bitmap is null in teximage2D.");
                            }
                        }
                    };
                    mImageLoader.load(getContext(), path, imgCb);
                    imgCb.waitTillFinish();
                }
            } catch (Throwable e) {
                GLog.e(TAG, e.getMessage(), e);
            }
        }
    }

    @Override
    public void preLoadImage(JSONArray arrayPara, final JSCallback callback) {
        IJSCallbackMap result = getDataFactory().createJSCallbackMap();
        if (null == arrayPara || arrayPara.length() != 2) {
            result.putString("error", "invalid input param. specify an json array which length is 2, and index 0 is url to load, index 1 is image id.");
            invokeCallback(callback, result);
            return;
        }

        String url;
        int imageId;
        try {
            url = arrayPara.getString(0);
            imageId = arrayPara.getInt(1);
        } catch (JSONException e) {
            result.putString("error", "invalid input param. specify an json array which length is 2, and index 0 is url to load, index 1 is image id.");
            GLog.d(TAG, "error parse preload image data:" + e.getMessage());
            invokeCallback(callback, result);
            return;
        }


        if (TextUtils.isEmpty(url)) {
            result.putString("error", "invalid input param. specify an json array which length is 2, and index 0 is url to load, index 1 is image id.");
            return;
        }
        try {
            final IJSCallbackMap resultMap = getDataFactory().createJSCallbackMap();
            if (url.startsWith("data:image")) {
                Bitmap bmp = handleBase64Texture(url.substring(url.indexOf("base64,") + "base64,".length()));
                if (bmp != null) {
                    resultMap.putInt("id", imageId);
                    resultMap.putString("url", url);
                    resultMap.putInt("width", bmp.getWidth());
                    resultMap.putInt("height", bmp.getHeight());
                } else {
                    resultMap.putString("error", "process base64 failed,url=" + url);
                }

                invokeCallback(callback, resultMap);

                return;
            }

            GImageLoadInfo imgInfo = mImageIdCache.get(url);

            if (null == imgInfo) {
                imgInfo = new GImageLoadInfo();
                mImageIdCache.put(url, imgInfo);
            }
            if (imgInfo.status.get() == GImageLoadInfo.IDLE) {
                imgInfo.status.set(GImageLoadInfo.LOADING);
                imgInfo.id = imageId;
                ArrayList<JSCallback> callbacks = mCallbacks.get(url);
                if (null == callbacks) {
                    callbacks = new ArrayList<>();
                    mCallbacks.put(url, callbacks);
                }
                callbacks.add(callback);

                final int imageIdFinal = imageId;
                final String urlFinal = url;
                mImageLoader.load(getContext(), url, new IGImageLoader.ImageCallback() {
                    @Override
                    public void onSuccess(Bitmap bitmap) {
                        if (null != bitmap) {
                            GImageLoadInfo imageInfo = mImageIdCache.get(urlFinal);
                            imageInfo.width = bitmap.getWidth();
                            imageInfo.height = bitmap.getHeight();
                            resultMap.putInt("id", imageIdFinal);
                            resultMap.putString("url", urlFinal);
                            resultMap.putInt("width", imageInfo.width);
                            resultMap.putInt("height", imageInfo.height);
                            imageInfo.status.set(GImageLoadInfo.LOADED);
                            try {
                                ArrayList<JSCallback> callbackList = mCallbacks.remove(urlFinal);
                                if (null != callbackList) {
                                    for (JSCallback callback : callbackList) {
                                        invokeCallback(callback, resultMap);
                                    }
                                }
                            } catch (Throwable throwable) {
                                imageInfo.status.set(GImageLoadInfo.IDLE);
                                invokeCallback(callback, resultMap);
                            }
                        } else {
                            resultMap.putString("error", "bitmap load failed");
                            try {
                                ArrayList<JSCallback> callbackList = mCallbacks.remove(urlFinal);
                                if (null != callbackList) {
                                    for (JSCallback callback : callbackList) {
                                        invokeCallback(callback, resultMap);
                                    }
                                }
                            } catch (Throwable throwable) {
                                invokeCallback(callback, resultMap);
                            }
                        }
                    }

                    @Override
                    public void onFail(Object error) {
                        resultMap.putString("error", "bitmap load failed");
                        try {
                            ArrayList<JSCallback> callbackList = mCallbacks.remove(urlFinal);
                            if (null != callbackList) {
                                for (JSCallback callback : callbackList) {
                                    invokeCallback(callback, resultMap);
                                }
                            }
                        } catch (Throwable throwable) {
                            invokeCallback(callback, resultMap);
                        }
                    }

                    @Override
                    public void onCancel() {

                    }
                });
            } else if (GImageLoadInfo.LOADING == imgInfo.status.get()) {
                // if same url is on loading, add it to callback lists.
                ArrayList<JSCallback> callbacks = mCallbacks.get(url);
                if (null == callbacks) {
                    callbacks = new ArrayList<>();
                    mCallbacks.put(url, callbacks);
                }

                callbacks.add(callback);
            } else if (GImageLoadInfo.LOADED == imgInfo.status.get()) {
                resultMap.putInt("id", imageId);
                resultMap.putString("url", url);
                resultMap.putInt("width", imgInfo.width);
                resultMap.putInt("height", imgInfo.height);
                ArrayList<JSCallback> callbackList = mCallbacks.remove(url);
                if (null != callbackList) {
                    for (JSCallback cb : callbackList) {
                        invokeCallback(callback, resultMap);
                    }
                }
                invokeCallback(callback, resultMap);
            }
        } catch (Throwable e) {
            Log.e(TAG, e.getMessage(), e);
        }
    }


    public abstract Context getContext();

    /**
     * invoke javascript callback
     * @param callback
     * @param args
     */
    protected abstract void invokeCallback(JSCallback callback, Object args);


    // TODO may have performance issues. DO NOT recommend.
    public Bitmap handleBase64Texture(String url) {
        try {
            byte[] decodedBytes = decode(url.getBytes());
            Bitmap bmp = BitmapFactory.decodeByteArray(decodedBytes, 0,
                    decodedBytes.length);

            return bmp;
        } catch (Throwable e) {
            Log.e(TAG, "error in processing base64Texture,error=" + e);
        }

        return null;
    }

    public static byte[] decode(byte[] in) {
        return decode(in, in.length);
    }

    public static byte[] decode(byte[] in, int len) {
        // approximate output length
        int length = len / 4 * 3;

        // return an empty array on empty or short input without padding
        if (length == 0) {
            return new byte[0];
        }

        // temporary array
        byte[] out = new byte[length];

        // number of padding characters ('=')
        int pad = 0;
        byte chr;
        // compute the number of the padding characters
        // and adjust the length of the input
        for (; ; len--) {
            chr = in[len - 1];
            // skip the neutral characters
            if ((chr == '\n') || (chr == '\r') || (chr == ' ') || (chr == '\t')) {
                continue;
            }
            if (chr == '=') {
                pad++;
            } else {
                break;
            }
        }

        // index in the output array
        int outIndex = 0;

        // index in the input array
        int inIndex = 0;

        // holds the value of the input character
        int bits = 0;

        // holds the value of the input quantum
        int quantum = 0;

        for (int i = 0; i < len; i++) {
            chr = in[i];
            // skip the neutral characters
            if ((chr == '\n') || (chr == '\r') || (chr == ' ') || (chr == '\t')) {
                continue;
            }
            if ((chr >= 'A') && (chr <= 'Z')) {
                // char ASCII value
                //  A    65    0
                //  Z    90    25 (ASCII - 65)
                bits = chr - 65;
            } else if ((chr >= 'a') && (chr <= 'z')) {
                // char ASCII value
                //  a    97    26
                //  z    122   51 (ASCII - 71)
                bits = chr - 71;
            } else if ((chr >= '0') && (chr <= '9')) {
                // char ASCII value
                //  0    48    52
                //  9    57    61 (ASCII + 4)
                bits = chr + 4;
            } else if (chr == '+') {
                bits = 62;
            } else if (chr == '/') {
                bits = 63;
            } else {
                return null;
            }

            // append the value to the quantum
            quantum = (quantum << 6) | (byte) bits;

            if (inIndex % 4 == 3) {
                // 4 characters were read, so make the output:
                out[outIndex++] = (byte) (quantum >> 16);
                out[outIndex++] = (byte) (quantum >> 8);
                out[outIndex++] = (byte) quantum;
            }

            inIndex++;
        }

        if (pad > 0) {
            // adjust the quantum value according to the padding
            quantum = quantum << (6 * pad);

            // make output
            out[outIndex++] = (byte) (quantum >> 16);

            if (pad == 1) {
                out[outIndex++] = (byte) (quantum >> 8);
            }
        }

        // create the resulting array
        byte[] result = new byte[outIndex];
        System.arraycopy(out, 0, result, 0, outIndex);

        return result;
    }

    protected abstract IJSCallbackDataFactory getDataFactory();
}
