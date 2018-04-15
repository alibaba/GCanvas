package com.taobao.gcanvas.adapters;

import android.content.Context;

import com.taobao.gcanvas.adapters.img.IGImageLoader;

/**
 * @author ertong
 * create at 2018/4/12
 */
public class GCanvasAdapterManager {

    private static volatile GCanvasAdapterManager sInstance = null;

    private Context mAppCtx;

    private GCanvasAdapterManager(Context context) {
        mAppCtx = context.getApplicationContext();
    }


    private IGImageLoader mImageLoader;

    public static GCanvasAdapterManager with(Context context) {
        if (null == context) {
            throw new IllegalArgumentException("Input parameter context is null");
        }

        if (null == sInstance) {
            synchronized (GCanvasAdapterManager.class) {
                if (null == sInstance) {
                    sInstance = new GCanvasAdapterManager(context);
                }
            }
        }

        return sInstance;
    }

    public void setGImageLoader(IGImageLoader loader) {
        if (null == loader) {
            return;
        }

        mImageLoader = loader;
    }

    public IGImageLoader getGImageLoader() {
        return mImageLoader;
    }

    private void clear() {
        mAppCtx = null;
        mImageLoader = null;
    }

    public static void dispose() {
        if (null == sInstance) {
            return;
        }
        synchronized (GCanvasAdapterManager.class) {
            if (null != sInstance) {
                sInstance.clear();
                sInstance = null;
            }

        }
    }
}
