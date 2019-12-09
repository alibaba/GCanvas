/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvasAndroid.h"
#include "GCanvas2DContextAndroid.h"
#include "GCanvas.hpp"


GCanvasAndroid::GCanvasAndroid(std::string contextId, const GCanvasConfig& config, GCanvasHooks *hooks) :
    GCanvas(contextId, config, hooks)
{
    CreateContext();
}


void GCanvasAndroid::CreateContext()
{
    mCanvasContext = new GCanvas2DContextAndroid(0, 0, mConfig);
    mCanvasContext->mContextId = this->mContextId;
//#ifndef GCANVAS_WEEX
//    if (mConfig.contextType == 0) {
//        ResetStateStack();
//        InitializeGLShader();
//    }
//#endif
}


GCanvas2DContextAndroid* GCanvasAndroid::GetCanvasContextAndroid()
{
    return (GCanvas2DContextAndroid*)mCanvasContext;
}



