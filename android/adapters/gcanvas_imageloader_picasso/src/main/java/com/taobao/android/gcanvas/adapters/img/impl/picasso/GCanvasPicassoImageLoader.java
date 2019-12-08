package com.taobao.android.gcanvas.adapters.img.impl.picasso;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.Looper;

import com.squareup.picasso.Picasso;
import com.squareup.picasso.Target;
import com.taobao.gcanvas.adapters.img.IGImageLoader;

import java.util.HashMap;


/**
 * Image loader implemented with Picasso.
 * @author ertong
 */
public class GCanvasPicassoImageLoader implements IGImageLoader {
    private static final String TAG = GCanvasPicassoImageLoader.class.getSimpleName();

    private Handler mMainHandler = null;

    private static HashMap<String, Target> sTargetTrigger = new HashMap<>();

    private ImageTarget mCurrentTarget;

    @Override
    public void load(final Context context, final String url, final ImageCallback callback) {
        mCurrentTarget = new ImageTarget(url, callback);
        sTargetTrigger.put(url, mCurrentTarget);
        if (Looper.myLooper() != Looper.getMainLooper()) {
            if (null == mMainHandler) {
                mMainHandler = new Handler(Looper.getMainLooper());
            }

            mMainHandler.post(new Runnable() {
                @Override
                public void run() {
                    Picasso.with(context).load(url).into(mCurrentTarget);
                }
            });
        } else {
            Picasso.with(context).load(url).into(mCurrentTarget);
        }
    }

    private class ImageTarget implements Target {
        private String mUrl;
        private ImageCallback mCallback;
        private Bitmap mBitmap;

        public ImageTarget(String url, ImageCallback callback) {
            this.mUrl = url;
            this.mCallback = callback;
        }

        @Override
        public void onBitmapLoaded(Bitmap bitmap, Picasso.LoadedFrom from) {
            mBitmap = bitmap;
            mCallback.onSuccess(mBitmap);
            mCallback = null;
            mBitmap = null;
            sTargetTrigger.remove(mUrl);
            mUrl = null;
        }

        @Override
        public void onBitmapFailed(Drawable errorDrawable) {
            mCallback.onFail(null);
            mCallback = null;
            mBitmap = null;
            sTargetTrigger.remove(mUrl);
            mUrl = null;
        }

        @Override
        public void onPrepareLoad(Drawable placeHolderDrawable) {
        }
    }

}
