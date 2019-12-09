package com.taobao.gcanvas.adapters.img.impl.fresco;

import android.content.Context;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import com.facebook.common.executors.CallerThreadExecutor;
import com.facebook.common.references.CloseableReference;
import com.facebook.datasource.DataSource;
import com.facebook.datasource.DataSubscriber;
import com.facebook.drawee.backends.pipeline.DraweeConfig;
import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.imagepipeline.common.RotationOptions;
import com.facebook.imagepipeline.core.ImagePipelineConfig;
import com.facebook.imagepipeline.image.CloseableBitmap;
import com.facebook.imagepipeline.image.CloseableImage;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;
import com.taobao.gcanvas.adapters.img.IGImageLoader;

/**
 * Image loader implemented with Fresco.
 *
 * @author ertong
 */
public class GCanvasFrescoImageLoader implements IGImageLoader {
    private static final String TAG = GCanvasFrescoImageLoader.class.getSimpleName();
    private ImagePipelineConfig imagePipelineConfig;
    private DraweeConfig draweeConfig;
    private Handler mHandler;

    public GCanvasFrescoImageLoader() {
        this(null, null);
    }

    public GCanvasFrescoImageLoader(DraweeConfig draweeConfig) {
        this(null, draweeConfig);
    }

    public GCanvasFrescoImageLoader(ImagePipelineConfig imagePipelineConfig) {
        this(imagePipelineConfig, null);
    }

    public GCanvasFrescoImageLoader(ImagePipelineConfig imagePipelineConfig, DraweeConfig draweeConfig) {
        this.imagePipelineConfig = imagePipelineConfig;
        this.draweeConfig = draweeConfig;
    }

    @Override
    public void load(Context context, String url, final ImageCallback callback) {
        if (TextUtils.isEmpty(url)) {
            Log.i(TAG, "url is null or empty");
            return;
        }

        if (!Fresco.hasBeenInitialized()) {
            Log.i(TAG, "fresco not init, do initialization");
            Fresco.initialize(context, imagePipelineConfig, draweeConfig);
        }

        ImageLoadRunnable runnable = new ImageLoadRunnable(context, url, callback);

        if (Looper.myLooper() != Looper.getMainLooper()) {
            if (mHandler == null) {
                mHandler = new Handler(Looper.getMainLooper());
            }
            mHandler.post(runnable);
        } else {
            runnable.run();
        }

    }

    private static class ImageLoadRunnable implements Runnable {
        private Context context;
        private String url;
        private final ImageCallback callback;

        public ImageLoadRunnable(Context context, String url, ImageCallback callback) {
            this.context = context;
            this.url = url;
            this.callback = callback;
        }

        @Override
        public void run() {
            Uri uri = Uri.parse(url);

            ImageRequestBuilder builder = ImageRequestBuilder.newBuilderWithSource(uri);
            builder.setAutoRotateEnabled(false);
            builder.setRotationOptions(RotationOptions.disableRotation());

            ImageRequest request = builder.build();

            DataSource<CloseableReference<CloseableImage>> ds;
            if (Fresco.getImagePipeline().isInBitmapMemoryCache(uri)) {
                ds = Fresco.getImagePipeline().fetchImageFromBitmapCache(request, this);
            } else {
                ds = Fresco.getImagePipeline().fetchDecodedImage(request, this);
            }

            ds.subscribe(new DataSubscriber<CloseableReference<CloseableImage>>() {
                @Override
                public void onNewResult(DataSource<CloseableReference<CloseableImage>> dataSource) {
                    if (!dataSource.isFinished()) {
                        return;
                    }
                    CloseableReference<CloseableImage> ref = dataSource.getResult();
                    if (ref != null) {
                        try {
                            CloseableImage result = ref.get();
                            if (result instanceof CloseableBitmap) {
                                callback.onSuccess(((CloseableBitmap) result).getUnderlyingBitmap());
                            } else {
                                byte[] array = new byte[result.getSizeInBytes()];
                                callback.onSuccess(BitmapFactory.decodeByteArray(array, 0, array.length));
                            }
                        } catch (Throwable throwable) {
                            callback.onFail(throwable.getMessage());
                        } finally {
                            CloseableReference.closeSafely(ref);
                        }
                    }
                }

                @Override
                public void onFailure(DataSource<CloseableReference<CloseableImage>> dataSource) {
                    String msg = dataSource.getFailureCause() != null ? dataSource.getFailureCause().getMessage() : null;
                    callback.onFail(msg);
                }

                @Override
                public void onCancellation(DataSource<CloseableReference<CloseableImage>> dataSource) {
                    callback.onCancel();
                }

                @Override
                public void onProgressUpdate(DataSource<CloseableReference<CloseableImage>> dataSource) {

                }
            }, CallerThreadExecutor.getInstance());
        }
    }

}
