package com.taobao.gcanvas.adapters.img;

import android.content.Context;
import android.graphics.Bitmap;

/**
 * An Image loader that loads url into bitmap.
 * Will replace it with C++ image library.
 * @author ertong
 */
public interface IGImageLoader {
    /**
     * Load specified url into bitmap asynchronously, and send results to specified callback.
     * Will return if any parameter is invalid(url is empty / null, or context/callback is null).
     * @param context context for potential use
     * @param url url to load
     * @param callback callback receiving result
     */
    void load(final Context context, final String url, final ImageCallback callback);

    /**
     * Callback receiving loading result.
     */
    interface ImageCallback {
        /**
         * Success callback with loaded bitmap
         * @param bitmap
         */
        void onSuccess(Bitmap bitmap);

        /**
         * Error callback with error message, maybe null.
         * @param error
         */
        void onFail(Object error);

        /**
         * Request was canceled.
         */
        void onCancel();
    }
}
