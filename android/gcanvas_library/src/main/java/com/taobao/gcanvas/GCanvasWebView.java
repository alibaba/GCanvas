/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas;

import android.view.View;

public abstract class GCanvasWebView {

    private View mView;

    public GCanvasWebView(View view) {
        mView = view;
    }

    public View getWebView() {
        return mView;
    }

    public void setWebView(View view) {
        mView = view;
    }

    public abstract String getUserAgentString();

    public abstract void setUserAgentString(String ua);

    public abstract String getOriginalUrl();

    public abstract String getUrl();

}
