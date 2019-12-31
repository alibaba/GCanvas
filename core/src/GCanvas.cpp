/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <errno.h>
#include <sstream>

#include "gcanvas/GPoint.h"
#include "GCanvas.hpp"
#include "GCanvas2dContext.h"

#ifdef ANDROID

#include "platform/Android/GFontCache.h"
#include "platform/Android/GCanvas2DContextAndroid.h"
#include "support/CharacterSet.h"

#endif

namespace gcanvas {

GCanvas::GCanvas(std::string canvasId, const GCanvasConfig &config, GCanvasHooks *hooks) :
        mContextId(canvasId),
        mConfig(config),mCanvasContext(nullptr)
{
    mHooks = hooks;
    LOG_D("Create Canvas");
}

void GCanvas::CreateContext() {
    mCanvasContext = new GCanvasContext(0, 0, mConfig, mHooks);
    mCanvasContext->mContextId = this->mContextId;
}

void GCanvas::Clear() {
    LOG_D("Canvas::DoContextLost start.");
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    LOG_D("Canvas::DoContextLost end.");
}

GCanvas::~GCanvas() {
    LOG_D("Canvas clear");
    if (mCanvasContext != NULL) {
        delete mCanvasContext;
        mCanvasContext = NULL;
    }
    Clear();
}


/**
 * View大小变化回调(GCanvas创建时会主动调一次)
 */
void GCanvas::OnSurfaceChanged(int x, int y, int width, int height) {
    GLint maxRenderbufferSize;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

    if ((maxRenderbufferSize <= width) || (maxRenderbufferSize <= height)) {
        
        LOG_EXCEPTION(mHooks, mContextId, "surfacesize_exceed_max",
                          "<function:%s, maxSize:%d, width:%d, height:%d>", __FUNCTION__,
                          maxRenderbufferSize, width, height);
        return;
    }

    if (width == 0 || height == 0) {
        return;
    }
    if (mCanvasContext->mWidth != width || mCanvasContext->mHeight != height) {
        mCanvasContext->mX = x;
        mCanvasContext->mY = y;
        mCanvasContext->mWidth = width;
        mCanvasContext->mHeight = height;
        mCanvasContext->InitializeGLEnvironment();
    }

    mCanvasContext->SetContextLost(false);
}


void GCanvas::drawFrame() {
    mCanvasContext->SendVertexBufferToGPU();
}

}
