/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 * <p>
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas;
public class GCanvas {
    // View Mode
    public enum ViewMode {
        NONE_MODE, SINGLE_CANVAS_MODE, SWITCH_MODE, HYBRID_MODE, FLOAT_HYBRID_MODE, WEEX_MODE
    }
    public static final ViewMode DEFAULT_VIEW_MODE = ViewMode.HYBRID_MODE;
}
