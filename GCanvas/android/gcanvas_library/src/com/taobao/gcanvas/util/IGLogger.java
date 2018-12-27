/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

package com.taobao.gcanvas.util;

public interface IGLogger {
    void d(String tag, String msg);
    void d(String tag, String msg, Throwable e);
    void i(String tag, String msg);
    void i(String tag, String msg, Throwable e);
    void e(String tag, String msg);
    void e(String tag, String msg, Throwable e);
    void v(String tag, String msg);
    void v(String tag, String msg, Throwable e);
    void w(String tag, String msg);
    void w(String tag, String msg, Throwable e);
}
