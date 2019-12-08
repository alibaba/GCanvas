/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas;


import android.app.Activity;

import org.json.JSONArray;
import org.json.JSONObject;

public class GCanvasResult<T> {
    public enum ResultCode {
        NO_RESULT, OK, ERROR
    }

    public interface Listener {
        void onResult(GCanvasResult result, ResultCode code, Object message);

        void onCallJS(String code);
    }

    protected ResultCode mResultCode = ResultCode.NO_RESULT;
    protected Object mResultMessage;

    protected String mCallbackId;
    protected Listener mResultListener;


    protected T mCustomData;

    protected boolean mFinished;
    protected int mChangingThreads;
    protected Activity mActivity = null;

    public Activity getActivity() {
        return mActivity;
    }

    public void setActivity(Activity act) {
        mActivity = act;
    }



    public GCanvasResult() {
        this.mCallbackId = "";
    }

    public GCanvasResult(T data) {
        this.mCustomData = data;
        this.mCallbackId = "";
    }

    public GCanvasResult(String callbackId, Listener listener) {
        this.mCallbackId = callbackId;
        this.mResultListener = listener;
    }

    public ResultCode getResultCode() {
        return mResultCode;
    }

    public Object getResultMessage() {
        return mResultMessage;
    }

    public boolean isFinished() {
        return mFinished;
    }

    public boolean isChangingThreads() {
        return mChangingThreads > 0;
    }

    public String getCallbackId() {
        return mCallbackId;
    }

    public T getCustomData() {
        return mCustomData;
    }

    public void setResult(ResultCode result, Object message) {
        mResultCode = result;
        mResultMessage = message;
        onResult(result, message);
    }

    protected void onResult(ResultCode resultCode, Object resultMessage) {
        if (mResultListener != null)
            mResultListener.onResult(this, resultCode, resultMessage);
    }

    protected void onCallJS(String code) {
        if (mResultListener != null)
            mResultListener.onCallJS(code);
    }

    /**
     * Helper for success callbacks that just returns the ResultCode.OK by
     * default
     *
     * @param message The message to add to the success result.
     */
    public void success(JSONObject message) {
        setResult(ResultCode.OK, message);
    }

    /**
     * Helper for success callbacks that just returns the ResultCode.OK by
     * default
     *
     * @param message The message to add to the success result.
     */
    public void success(JSONArray message) {
        setResult(ResultCode.OK, message);
    }

    public void success(int message) {
        setResult(ResultCode.OK, message);
    }

    public void success(String message) {
        setResult(ResultCode.OK, message);
    }

    public void success(boolean message) {
        setResult(ResultCode.OK, message);
    }

    public void success(float message) {
        setResult(ResultCode.OK, message);
    }

    public void success(Object message) {
        setResult(ResultCode.OK, message);
    }

    /**
     * Helper for success callbacks that just returns the ResultCode.OK by
     * default
     */
    public void success() {
        setResult(ResultCode.OK, "");
    }

    public void error() {
        setResult(ResultCode.ERROR, "");
    }

    public void error(JSONObject message) {
        setResult(ResultCode.ERROR, message.toString());
    }

    public void error(String message) {
        setResult(ResultCode.ERROR, message);
    }

    public void error(Object message) {
        setResult(ResultCode.ERROR, message);
    }

    public void calljs(String code) {
        onCallJS(code);
    }

}
