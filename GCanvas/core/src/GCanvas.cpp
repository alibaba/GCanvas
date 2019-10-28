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
#include "gcanvas/GFontCache.h"
#include "support/CharacterSet.h"
#include "GCanvasLinkNative.h"
#include <3d/gmanager.h>
#endif

using namespace gcanvas;

GCanvas::GCanvas(std::string contextId, bool flip, std::string appInfo, bool useFboFlag) :
        GCanvasContext(0, 0, flip, appInfo, useFboFlag),
        mContextId(contextId)
{
#ifdef GCANVAS_WEEX
    mCurrentTransform = GTransformIdentity,
    mFrames = 0;
    mFps = 0.0f;
    mContextLost = false;
    mResult = "";
#ifdef ANDROID
    mLastTime = clock();
    sem_init(&mSyncSem, 0, 0);
#endif
    
#endif
    LOG_D("Create Canvas");
}

void GCanvas::Clear()
{
    LOG_D("Canvas::DoContextLost start.");
    // No need to clean up GL memory with glDeleteBuffers or glDeleteTextures.
    // It all gets blown away automatically when the context is lost.
#ifdef GCANVAS_WEEX
    mContextLost = true;
    mTextureMgr.Clear();
#ifdef ANDROID
    clearCmdQueue();
#endif
    
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    LOG_D("Canvas::DoContextLost end.");
}

GCanvas::~GCanvas()
{
    Clear();
    LOG_D("Canvas clear");
}

/**
 * View大小变化回调(GCanvas创建时会主动调一次)
 */
void GCanvas::OnSurfaceChanged(int x, int y, int width, int height)
{
    GLint maxRenderbufferSize;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

    if ((maxRenderbufferSize <= width) || (maxRenderbufferSize <= height))
    {
        LOG_EXCEPTION(mAppInfo.c_str(), "surfacesize_exceed_max", "<function:%s, maxSize:%d, width:%d, height:%d>", __FUNCTION__, maxRenderbufferSize, width, height);
        return;
    }

    if (mWidth != width || mHeight != height)
    {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;

        InitializeGLEnvironment();
    }

    mContextLost = false;
    LOG_D("GCanvas::OnSurfaceChanged mContextLost %d", mContextLost);
}

void GCanvas::SetBackgroundColor(float red, float green, float blue)
{
    GColorRGBA color;
    color.rgba.r = red;
    color.rgba.g = green;
    color.rgba.b = blue;
    color.rgba.a = 1.0f;
    GCanvasContext::SetFillStyle(color);
}

void GCanvas::drawFrame(bool clear)
{
    SendVertexBufferToGPU();
    if (clear)
    {
        ClearGeometryDataBuffers();
    }
}

void GCanvas::drawFBO(std::string fboName, GCompositeOperation compositeOp,
                      float sx, float sy, float sw, float sh,
                      float dx, float dy, float dw, float dh)
{
    if (!mIsFboSupported)
    {
        return;
    }

    if (nullptr == mCurrentState || nullptr == mCurrentState->mShader)
    {
        return;
    }

    Save();
    glViewport(mX, mY, mWidth, mHeight);

    GFrameBufferObject &fbo = mFboMap[fboName];

    UseDefaultRenderPipeline();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    SetGlobalCompositeOperation(compositeOp, compositeOp);

    GColorRGBA color = GColorWhite;
    mCurrentState->mShader->SetOverideTextureColor(0);
    mCurrentState->mShader->SetHasTexture(1);
    fbo.mFboTexture.Bind();

    PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;

    if (HasClipRegion())
    {
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    glViewport(0, 0, mWidth, mHeight);

    Restore();
}

//Runtime & Weex 2D 通用的接口
void GCanvas::DrawText(const char *text, float x, float y, float maxWidth,
                       bool isStroke, int strLength)
{
    if (strLength == 0)
    {
        strLength = static_cast<int>(strlen(text));
    }

    DrawTextWithLength(text, strLength, x, y, isStroke, maxWidth);
}

void GCanvas::DrawTextWithLength(const char *text, int strLength, float x, float y,
                                 bool isStroke, float maxWidth)
{

    const GCompositeOperation old_op = mCurrentState->mGlobalCompositeOp;
    SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);

    // scaleWidth
    float scaleWidth = 1.0;
    if (fabs(maxWidth - SHRT_MAX) > 1) {
        // 对maxwidth进行判断，避免默认值导致的每次measure操作
        float measureWidth = execMeasureTextWidth(text);
        if (measureWidth > maxWidth)
        {
            scaleWidth = maxWidth / measureWidth;
        }
    }

#ifdef ANDROID

    Utf8ToUCS2 *lbData = new Utf8ToUCS2(text, strLength);

    GCanvasContext::FillText(lbData->ucs2, lbData->ucs2len, x, y,
                             isStroke, scaleWidth);

    delete lbData;

#endif

#ifdef IOS
    GCanvasContext::FillText((const unsigned short *)text,
                              (unsigned int)strLength, x, y, isStroke, scaleWidth);
#endif

    SetGlobalCompositeOperation(old_op, old_op);
}

float GCanvas::execMeasureTextWidth(const char *text, int strLength)
{
    if (strLength == 0)
    {
        strLength = static_cast<int>(strlen(text));
    }
    if (mCurrentState->mFont == nullptr)
    {
        mCurrentState->mFont = new GFontStyle(nullptr, mDevicePixelRatio);
    }
    
    int width = mFontManager->MeasureText(text, strLength, mCurrentState->mFont);
//    LOG_W("execMeasureTextWidth: %s, %f, font: name=%s, rawWidth=%i, canvasWidth=%i，"
//                  "transform.a=%f, mDevicePixelRatio=%f",
//          text, result, mCurrentState->mFont->GetName().c_str(), width,
//          GetCanvasWidth(), mCurrentState->mTransform.a,
//            mDevicePixelRatio);
    return width / mDevicePixelRatio;
}
