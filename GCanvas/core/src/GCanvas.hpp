/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GCANVAS_H
#define GCANVAS_GCANVAS_H

#include "gcanvas/GCanvas2dContext.h"
#include "support/Log.h"
#include "export.h"

namespace gcanvas
{

class GCanvas
{
public:
    API_EXPORT GCanvas(std::string canvasId, const GCanvasConfig& config, GCanvasHooks *hooks = nullptr);
    API_EXPORT virtual ~GCanvas();
    virtual void CreateContext();
    virtual void Clear();
    
    API_EXPORT void OnSurfaceChanged(int x, int y, int width, int height);
    
    GCanvasContext* GetGCanvasContext() { return mCanvasContext; }
    float GetDevicePixelRatio() { return mCanvasContext->GetDevicePixelRatio();}

    API_EXPORT void drawFrame();


public:
    std::string mContextId;
    GCanvasConfig mConfig;
    GCanvasHooks *mHooks;
    
    GCanvasContext *mCanvasContext;
};
}

#endif /* GCANVAS_GCANVAS_H */
