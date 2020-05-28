/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvas2dContext.h"
#include "GShaderManager.h"
#include "../GCanvas.hpp"
#include "../support/GLUtil.h"

#include <assert.h>
#include <string.h>

#define SIZE_EPSILON 1.f

using namespace gcanvas;

#define FontTextureWidth 2048
#define FontTextureHeight 2048

GBlendOperationFuncs GCompositeOperationFuncs(int index)
{
    static GBlendOperationFuncs funcs[] = {
        {GL_ONE, GL_ONE_MINUS_SRC_ALPHA},                 //0
        {GL_SRC_ALPHA, GL_ONE},                           //1
        {GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA},           //2
        {GL_ZERO, GL_ONE_MINUS_SRC_ALPHA},                //3
        {GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA},           //4
        {GL_DST_ALPHA, GL_ZERO},                          //5
        {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}, //6
        {GL_ONE, GL_ZERO},                                //7
        {GL_SRC_ALPHA, GL_DST_ALPHA},                     //8
        {GL_ONE, GL_ONE},                                 //9
        {GL_ONE_MINUS_DST_ALPHA, GL_ZERO},                //10
        {GL_ZERO, GL_SRC_ALPHA},                          //11
    };

    if (index < COMPOSITE_OP_SOURCE_OVER || index >= COMPOSITE_OP_NONE)
    {
        index = COMPOSITE_OP_SOURCE_OVER;
    }

    return funcs[index];
}

GColorRGBA BlendColor(GCanvasContext *context, GColorRGBA color)
{
    float alpha = context->GlobalAlpha() * color.rgba.a;
    GColorRGBA c;
    c.rgba.r = color.rgba.r * alpha;
    c.rgba.g = color.rgba.g * alpha;
    c.rgba.b = color.rgba.b * alpha;
    c.rgba.a = alpha;
    return c;
}

GColorRGBA BlendWhiteColor(GCanvasContext *context)
{
    GColorRGBA c = {1, 1, 1, 1};
    return BlendColor(context, c);
}

GColorRGBA BlendFillColor(GCanvasContext *context)
{
    return BlendColor(context, context->FillStyle());
}

GColorRGBA BlendStrokeColor(GCanvasContext *context)
{
    return BlendColor(context, context->StrokeStyle());
}

void GCanvasContext::FillText(const unsigned short *text, unsigned int text_length,
                              float x, float y, bool isStroke, float scaleWidth)
{
    ApplyFillStylePipeline(isStroke);

    if (mCurrentState->mFont == nullptr)
    {
        mCurrentState->mFont = new GFontStyle(nullptr, mDevicePixelRatio);
    }
    mCurrentState->mShader->SetOverideTextureColor(1);

    SendVertexBufferToGPU();
    Save();
    DoTranslate(x, y);
    DoScale(1 / mDevicePixelRatio * scaleWidth, 1 / mDevicePixelRatio);
    mFontManager->DrawText(text, text_length, 0, 0, isStroke, mCurrentState->mFont);
    Restore();

    mCurrentState->mShader->SetOverideTextureColor(0);
}

GCanvasContext::GCanvasContext(short w, short h, const GCanvasConfig &config, GCanvasHooks *hooks) : mIsFboSupported(true), mWidth(w), mHeight(h), mCurrentState(nullptr),
                                                                                                     mHasClipRegion(false), mHiQuality(false), mVertexBufferIndex(0),
                                                                                                     mSaveShader(nullptr),
                                                                                                     mClearColor(GColorTransparentWhite),
                                                                                                     mDevicePixelRatio(1.f),
                                                                                                     mContextType(0),
                                                                                                     mConfig(config)
{

    mHooks = hooks;
    if (mWidth > 0 && mHeight > 0)
    {
        UpdateProjectTransform();
        InitFBO();
    }
    mFontManager = GFontManager::NewInstance(this);
}

bool GCanvasContext::IsUseFbo()
{
    return mConfig.useFbo;
}

float GCanvasContext::GetCanvasDimensionWidthScale()
{
    if (mWidth <= 0)
    {
        return mDevicePixelRatio;
    }
    else
    {
        int canvasWidth = GetCanvasWidth();
        return mDevicePixelRatio * mWidth / canvasWidth;
    }
}

float GCanvasContext::GetCanvasDimensionHeightScale()
{
    if (mHeight <= 0)
    {
        return mDevicePixelRatio;
    }
    else
    {
        int canvasHeight = GetCanvasHeight();
        return mDevicePixelRatio * mHeight / canvasHeight;
    }
}

float GCanvasContext::GetCurrentScaleX()
{
    int w = GetCanvasWidth();
    if (mCurrentState != nullptr && w > 0)
    {
        return mCurrentState->mTransform.a / (2.f * mDevicePixelRatio / GetCanvasWidth());
    }
    else
    {
        return 1;
    }
}

float GCanvasContext::GetCurrentScaleY()
{
    int h = GetCanvasHeight();
    if (mCurrentState != nullptr && h > 0)
    {
        return mCurrentState->mTransform.d / (-2.f * mDevicePixelRatio / h);
    }
    else
    {
        return 1;
    }
}

void GCanvasContext::SetDevicePixelRatio(const float ratio)
{
    mDevicePixelRatio = ratio;
    UpdateProjectTransform();
}

float GCanvasContext::GetDevicePixelRatio()
{
    return mDevicePixelRatio;
}

void GCanvasContext::SetCanvasDimension(const int w, const int h, bool resetStatus)
{
    mCanvasWidth = w;
    mCanvasHeight = h;

    if (mContextType != 0)
    {
        return;
    }

    UpdateProjectTransform();

    if (mCurrentState != nullptr)
    {
        GTransform old = mCurrentState->mTransform;
        mCurrentState->mTransform = mProjectTransform;
        mCurrentState->mUserTransform = GTransformIdentity;
        if (!GTransformEqualToTransform(old, mCurrentState->mTransform))
        {
            // update shader transform
            SetTransformOfShader(mCurrentState->mTransform);
        }
    }

    // reset status and clear screen
    if (resetStatus)
    {
        mVertexBufferIndex = 0;
        ResetStateStack();
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        UseDefaultRenderPipeline();
        ClearScreen();
    }
}

int GCanvasContext::GetCanvasWidth()
{
    return mCanvasWidth <= 0 ? mWidth : mCanvasWidth;
}

int GCanvasContext::GetCanvasHeight()
{
    return mCanvasHeight <= 0 ? mHeight : mCanvasHeight;
}

short GCanvasContext::GetWidth() const
{
    return mWidth;
}

short GCanvasContext::GetHeight() const
{
    return mHeight;
}

void GCanvasContext::UpdateProjectTransform()
{
    int w = GetCanvasWidth();
    int h = GetCanvasHeight();
    if (w <= 0 || h <= 0)
    {
        return;
    }

    mProjectTransform = CalculateProjectTransform(w, h);
}

GTransform GCanvasContext::CalculateProjectTransform(int w, int h)
{
    GTransform t = GTransformIdentity;
    if (mConfig.flip)
    {
        t = GTransformScale(t, 2.f * mDevicePixelRatio / w,
                            2.f * mDevicePixelRatio / h);

        t = GTransformTranslate(t, -1.f, -1.f);
    }
    else
    {
        t = GTransformScale(t, 2.f * mDevicePixelRatio / w,
                            -2.f * mDevicePixelRatio / h);

        t = GTransformTranslate(t, -1.f, 1.f);
    }
    return t;
}

void GCanvasContext::InitFBO()
{
    if (0 != mContextType)
        return;

    if (!mConfig.useFbo)
    {
        return;
    }

    if (!mIsFboSupported)
    {
        return;
    }

    if (mFboMap.find(DefaultFboName) == mFboMap.end())
    {
        std::vector<GCanvasLog> logVec;
        mIsFboSupported = mFboMap[DefaultFboName].InitFBO(mWidth, mHeight,
                                                          mClearColor, false, &logVec);
        LOG_EXCEPTION_VECTOR(mHooks, mContextId.c_str(), logVec);
    }
}

void GCanvasContext::BindFBO()
{
    mFboMap[DefaultFboName].BindFBO();
}

void GCanvasContext::UnbindFBO()
{
    mFboMap[DefaultFboName].UnbindFBO();
}

long GCanvasContext::DrawCallCount()
{
    return mDrawCallCount;
}

void GCanvasContext::ClearDrawCallCount()
{
    mDrawCallCount = 0;
}

GTexture *GCanvasContext::GetFboTexture()
{
    return &mFboMap[DefaultFboName].mFboTexture;
}

GCanvasContext::~GCanvasContext()
{
    if (mFontTexture != nullptr)
    {
        GLuint textureId = mFontTexture->GetTextureID();
        glDeleteTextures(1, &textureId);
    }

    delete mFontManager;
}

bool GCanvasContext::InitializeGLEnvironment()
{
    if (0 != mContextType)
    { // only 2D need to init
        return true;
    }
    LOG_I("initializeGLEnvironment");

    if (mWidth > 0 && mHeight > 0)
    {
        InitFBO();
    }

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glActiveTexture(GL_TEXTURE0);

    glViewport(0, 0, mWidth, mHeight);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    UpdateProjectTransform();

    // gl initialize
    if (!mIsGLInited)
    {
        ResetStateStack();
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        InitializeGLShader();

        mIsGLInited = true;
    }

    return true;
}

bool GCanvasContext::InitializeGLShader()
{
    UsePatternRenderPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseLinearGradientPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseRadialGradientPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseTextureRenderPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseShadowRenderPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseDefaultRenderPipeline();
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    return true;
}

void GCanvasContext::ClearGeometryDataBuffers()
{
    mPath.Reset();
    mVertexBufferIndex = 0;
}

/**
 * 重置gcanvas绘制配置为默认状态，只在初始化和dimension变化时才能触发
 * 1.重置canvas配置有：fillStyle/strokeStyle,lineJoin/miter,textAlign,字体配置，阴影配置,
 * globalCompositeOperation,clip等
 * 2.夹带更新变换的逻辑
*/
void GCanvasContext::ResetStateStack()
{
    mPath.Reset();

    bool hasOldState = (nullptr != mCurrentState);
    GTransform oldTransfrom = {0, 0, 0, 0, 0, 0};

    if (hasOldState)
    {
        oldTransfrom = mCurrentState->mTransform;
        if (mCurrentState->mClipPath != nullptr)
        {
            ResetClip();
        }
    }

    mStateStack.clear();
    GCanvasState state;
    mStateStack.push_back(state);
    mCurrentState = &mStateStack.back();
    mCurrentState->mTransform = mProjectTransform;
    mCurrentState->mUserTransform = GTransformIdentity;

    if (!hasOldState)
    {
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
    }

    if (!hasOldState ||
        !GTransformEqualToTransform(oldTransfrom, mCurrentState->mTransform))
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }

    SetClipFlag(false);
    if (mCurrentState->mClipPath != nullptr)
    {
        mCurrentState->mClipPath->DrawPolygons2DToContext(this,
                                                          mCurrentState->mClipPath->mFillRule,
                                                          FILL_TARGET_DEPTH);
        SetClipFlag(true);
    }
}

void GCanvasContext::BindVertexBuffer()
{
    BindPositionVertexBuffer();

    if (mCurrentState->mShader->GetTexcoordSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetTexcoordSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetTexcoordSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *)CanvasVertexBuffer) + 2);
    }
    if (mCurrentState->mShader->GetColorSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetColorSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetColorSlot(), 4,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *)CanvasVertexBuffer) + 4);
    }
}

GLuint GCanvasContext::PositionSlot()
{
    return (GLuint)mCurrentState->mShader->GetPositionSlot();
}

void GCanvasContext::BindPositionVertexBuffer()
{
    if (mCurrentState->mShader->GetPositionSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetPositionSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetPositionSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              CanvasVertexBuffer);
    }
}

void GCanvasContext::SetTexture(int textureId)
{
    if (mCurrentState->mTextureId != textureId)
    {
        SendVertexBufferToGPU();
        mCurrentState->mTextureId = textureId;
    }
}

//| a b c |      | a b 0 c |
//| d e f |  =>  | d e 0 f |
//| g h i |      | 0 0 1 0 |
//               | g h 0 i |
void GCanvasContext::SendVertexBufferToGPU(const GLenum geometry_type)
{
    if (mVertexBufferIndex == 0)
    {
        return;
    }
    mCurrentState->mShader->SetTransform(mCurrentState->mTransform);
    mCurrentState->mShader->SetHasTexture(mCurrentState->mTextureId != InvalidateTextureId);
    if (mCurrentState->mTextureId != InvalidateTextureId)
    {
        glBindTexture(GL_TEXTURE_2D, mCurrentState->mTextureId);
    }
    //draw call
    mDrawCallCount++;
    glDrawArrays(geometry_type, 0, mVertexBufferIndex);

    mVertexBufferIndex = 0;
}

void GCanvasContext::PushTriangle(GPoint v1, GPoint v2, GPoint v3,
                                  GColorRGBA color, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    if (vec)
    {
        // push to std::vector
        GVertex vertex[3];
        vb = vertex;
    }
    else
    {
        // push to vertexBuffer
        if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 3)
        {
            SendVertexBufferToGPU();
        }

        vb = &CanvasVertexBuffer[mVertexBufferIndex];
    }
    GPoint p = {0, 0};
    vb[0].pos = v1;
    vb[1].pos = v2;
    vb[2].pos = v3;
    vb[0].uv = vb[1].uv = vb[2].uv = p;
    vb[0].color = vb[1].color = vb[2].color = color;

    if (vec)
    {
        // push to std::vector
        for (int i = 0; i < 3; i++)
        {
            vec->push_back(vb[i]);
        }
    }
    else
    {
        // push to vertexBuffer
        mVertexBufferIndex += 3;
    }
}

void GCanvasContext::PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                              GColorRGBA color, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    if (vec)
    {
        // push to std::vector
        GVertex vertex[6];
        vb = vertex;
    }
    else
    {
        // push to vertexBuffer
        if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 6)
        {
            SendVertexBufferToGPU();
        }

        vb = &CanvasVertexBuffer[mVertexBufferIndex];
    }
    GPoint p = {0, 0};

    vb[0].pos = v1;
    vb[1].pos = v2;
    vb[2].pos = v3;
    vb[3].pos = v3;
    vb[4].pos = v4;
    vb[5].pos = v1;
    vb[0].uv = vb[1].uv = vb[2].uv = vb[3].uv = vb[4].uv = vb[5].uv = p;
    vb[0].color = vb[1].color = vb[2].color = vb[3].color = vb[4].color =
        vb[5].color = color;

    if (vec)
    {
        // push to std::vector
        for (int i = 0; i < 6; i++)
        {
            vec->push_back(vb[i]);
        }
    }
    else
    {
        // push到vertexBuffer
        mVertexBufferIndex += 6;
    }
}

void GCanvasContext::PushRectangle(float x, float y, float w, float h,
                                   float tx, float ty, float tw, float th,
                                   GColorRGBA color, bool flipY)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 6)
    {
        SendVertexBufferToGPU();
    }

    if (flipY)
    {
        ty = 1 - ty;
        th *= -1;
    }

    GPoint p11 = PointMake(x, y);
    GPoint p21 = PointMake(x + w, y);
    GPoint p12 = PointMake(x, y + h);
    GPoint p22 = PointMake(x + w, y + h);

    GPoint t11 = PointMake(tx, ty);
    GPoint t21 = PointMake(tx + tw, ty);
    GPoint t12 = PointMake(tx, ty + th);
    GPoint t22 = PointMake(tx + tw, ty + th);

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    vb[0].pos = p11;
    vb[0].uv = t11;
    vb[1].pos = p21;
    vb[1].uv = t21;
    vb[2].pos = p12;
    vb[2].uv = t12;

    vb[3].pos = p21;
    vb[3].uv = t21;
    vb[4].pos = p12;
    vb[4].uv = t12;
    vb[5].pos = p22;
    vb[5].uv = t22;
    vb[0].color = vb[1].color = vb[2].color = vb[3].color = vb[4].color =
        vb[5].color = color;

    mVertexBufferIndex += 6;
}

void GCanvasContext::PushRectangle4TextureArea(float x, float y, float w, float h, float tx, float ty,
                                               float tw, float th, GColorRGBA color, bool flipY)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 6)
    {
        SendVertexBufferToGPU();
    }

    if (flipY)
    {
        ty = th - ty; //th is ratio to texture
        th *= -1;
    }

    GPoint p11 = PointMake(x, y);
    GPoint p21 = PointMake(x + w, y);
    GPoint p12 = PointMake(x, y + h);
    GPoint p22 = PointMake(x + w, y + h);

    GPoint t11 = PointMake(tx, ty);
    GPoint t21 = PointMake(tx + tw, ty);
    GPoint t12 = PointMake(tx, ty + th);
    GPoint t22 = PointMake(tx + tw, ty + th);

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    vb[0].pos = p11;
    vb[0].uv = t11;
    vb[1].pos = p21;
    vb[1].uv = t21;
    vb[2].pos = p12;
    vb[2].uv = t12;

    vb[3].pos = p21;
    vb[3].uv = t21;
    vb[4].pos = p12;
    vb[4].uv = t12;
    vb[5].pos = p22;
    vb[5].uv = t22;
    vb[0].color = vb[1].color = vb[2].color = vb[3].color = vb[4].color =
        vb[5].color = color;

    mVertexBufferIndex += 6;
}

void GCanvasContext::PushReverseRectangle(float x, float y, float w, float h,
                                          float tx, float ty, float tw, float th,
                                          GColorRGBA color)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 6)
    {
        SendVertexBufferToGPU();
    }

    GPoint p11 = PointMake(x, y);
    GPoint p21 = PointMake(x + w, y);
    GPoint p12 = PointMake(x, y + h);
    GPoint p22 = PointMake(x + w, y + h);

    GPoint t11 = PointMake(tx, ty + th);
    GPoint t21 = PointMake(tx + tw, ty + th);
    GPoint t12 = PointMake(tx, ty);
    GPoint t22 = PointMake(tx + tw, ty);

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    vb[0].pos = p11;
    vb[0].uv = t11;
    vb[1].pos = p21;
    vb[1].uv = t21;
    vb[2].pos = p12;
    vb[2].uv = t12;

    vb[3].pos = p21;
    vb[3].uv = t21;
    vb[4].pos = p12;
    vb[4].uv = t12;
    vb[5].pos = p22;
    vb[5].uv = t22;
    vb[0].color = vb[1].color = vb[2].color = vb[3].color = vb[4].color =
        vb[5].color = color;

    mVertexBufferIndex += 6;
}

void GCanvasContext::PushPoints(const std::vector<GPoint> &points,
                                GColorRGBA color)
{
    GPoint uv = PointMake(0, 0);
    for (int i = 0; i + 2 < (int)points.size(); i += 3)
    {
        if (mVertexBufferIndex + 3 > GCANVAS_VERTEX_BUFFER_SIZE)
            SendVertexBufferToGPU();

        for (int j = 0; j < 3; ++j)
        {
            CanvasVertexBuffer[mVertexBufferIndex].pos = points[i + j];
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;
        }
    }
}

void GCanvasContext::PushVertexs(const std::vector<GVertex> &vertexs)
{
    for (int i = 0; i + 2 < (int)vertexs.size(); i += 3)
    {
        if (mVertexBufferIndex + 3 > GCANVAS_VERTEX_BUFFER_SIZE)
            SendVertexBufferToGPU();

        for (int j = 0; j < 3; ++j)
        {
            CanvasVertexBuffer[mVertexBufferIndex] = vertexs[i + j];
            mVertexBufferIndex++;
        }
    }
}

void GCanvasContext::PushTriangleFanPoints(const std::vector<GPoint> &points, GColorRGBA color)
{
    GPoint uv = PointMake(0, 0);
    GPoint head = points[0];
    int segmentStride = 3;
    int segmentSize;
    int curCountToTail;
    int pointSize = (int)points.size();

    for (int i = 0; i < pointSize; i += segmentStride)
    {
        curCountToTail = pointSize - i;
        segmentSize = curCountToTail > segmentStride ? segmentStride : curCountToTail;

        if (mVertexBufferIndex + segmentStride >= (GCANVAS_VERTEX_BUFFER_SIZE - 1))
        {
            //LOG_D("SendVertexBufferToGPU, vertex count=%d, i=%d, segmentSize=%d", mVertexBufferIndex,
            //    i, segmentSize);

            //first add head and SendVertexBufferToGPU
            CanvasVertexBuffer[mVertexBufferIndex].pos = head;
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;

            SendVertexBufferToGPU(GL_TRIANGLE_FAN);

            // second add head
            CanvasVertexBuffer[mVertexBufferIndex].pos = head;
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;
        }

        for (int j = 0; j < segmentSize; ++j)
        {
            CanvasVertexBuffer[mVertexBufferIndex].pos = points[i + j];
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;
        }
    }
}

#ifdef GCANVAS_WEEX
void GCanvasContext::ApplyTransform(float m11, float m12, float m21, float m22,
                                    float dx, float dy)
{
    mCurrentState->mTransform = GTransformConcat(
        mProjectTransform, GTransformMake(m11, m12, m21, m22, dx, dy));
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::ApplyTransform(GTransform t)
{
    mCurrentState->mTransform = GTransformConcat(mProjectTransform, t);
    SetTransformOfShader(mCurrentState->mTransform);
}
#endif

GShader *GCanvasContext::FindShader(const char *name)
{
    GShader *shader = GShaderManager::getSingleton()->programForKey(name);
    if (shader)
    {
        std::vector<GCanvasLog> log = shader->GetErrorVector();
        LogExceptionVector(mHooks, mContextId, log);
        shader->ClearErrorVector();
    }
    return shader;
}

void GCanvasContext::SetTransformOfShader(const GTransform &trans)
{
    // emit the buffered geometry data
    // before the updating of transform matrix
    SendVertexBufferToGPU();

    GLfloat m[16];
    m[2] = m[6] = m[8] = m[9] = m[11] = m[14] = 0;
    m[10] = 1;
    m[0] = trans.a;
    m[1] = trans.b;
    m[4] = trans.c;
    m[5] = trans.d;
    m[12] = trans.tx;
    m[13] = trans.ty;
    m[3] = m[7] = 0;
    m[15] = 1;

    if (mCurrentState != nullptr && mCurrentState->mShader != nullptr)
    {

        GShader *shader = mCurrentState->mShader;
        if (nullptr == shader)
        {
            LOG_I("SetTransformOfShader ===> shader is null");
            return;
        }

        std::string name = shader->GetName();
        if (nullptr == FindShader(name.data()))
        {
            LOG_I("SetTransformOfShader program for key is null");
            return;
        }

        glUniformMatrix4fv(shader->GetTransformSlot(), 1, GL_FALSE, &(m[0]));
    }
}

GCompositeOperation GCanvasContext::GlobalCompositeOperation()
{
    return mCurrentState->mGlobalCompositeOp;
}

#ifdef IOS

void GCanvasContext::DoSetGlobalCompositeOperation(GCompositeOperation op, GCompositeOperation alphaOp)
{
    if (mCurrentState->mGlobalCompositeOp == op)
    {
        return;
    }
    SendVertexBufferToGPU();

    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);

    glBlendFunc(funcs.source, funcs.destination);

    mCurrentState->mGlobalCompositeOp = op;
}
#endif

void GCanvasContext::DrawFBOToFBO(GFrameBufferObject &src, GFrameBufferObject &dest)
{
    glViewport(0, 0, dest.ExpectedWidth(), dest.ExpectedHeight());

    DoSetGlobalCompositeOperation(COMPOSITE_OP_REPLACE, COMPOSITE_OP_REPLACE);

    PushRectangle4TextureArea(-1, -1, 2, 2, 0, 0,
                              static_cast<float>(src.ExpectedWidth()) / src.Width(),
                              static_cast<float>(src.ExpectedHeight()) / src.Height(),
                              GColorWhite);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
    glBindTexture(GL_TEXTURE_2D, src.mFboTexture.GetTextureID());
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;
}

void GCanvasContext::PrepareDrawElemetToFBO(GFrameBufferObject &fbo)
{
    glViewport(0, 0, fbo.ExpectedWidth(), fbo.ExpectedHeight());
    GTransform &t = mCurrentState->mTransform;
    t = CalculateProjectTransform(fbo.ExpectedWidth(), fbo.ExpectedHeight());
}

void GCanvasContext::DrawFBOToScreen(GFrameBufferObject &fbo, float x, float y, float w, float h,
                                     GColorRGBA color)
{
    SetTexture(fbo.mFboTexture.GetTextureID());
    PushRectangle4TextureArea(x, y, w, h, 0, 0,
                              static_cast<float>(fbo.ExpectedWidth()) / fbo.Width(),
                              static_cast<float>(fbo.ExpectedHeight()) / fbo.Height(),
                              color, true);
}

void GCanvasContext::Blur(const GRectf &rect, float blur,
                          std::function<void()> draw, GFrameBufferObjectPtr &outputFbo,
                          bool isOnScreen, float scale)
{
    const float Step = 5;
    blur /= 2.5f;

    GFrameBufferObjectPtr originFbo;
    float blurStep;
    float scaleFactor = 1;
    if (blur * scale > Step)
    {
        Blur(rect / Step, blur, draw, originFbo, false, scale / Step);
        blurStep = Step;
        scaleFactor = 1 / Step;
    }
    else
    {
        // draw origin
        originFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
        originFbo->BindFBO();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Save();
        PrepareDrawElemetToFBO(*originFbo);
        UseDefaultRenderPipeline();
        DoTranslate(-rect.leftTop.x, -rect.leftTop.y);
        DoScale(scale, scale);
        draw();
        Restore();
        originFbo->UnbindFBO();

        blurStep = blur * scale;
    }

    Save();
    UseShadowRenderPipeline(blurStep * 2.5);

    // horizontal blur
    auto shadowFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
    shadowFbo->BindFBO();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mCurrentState->mShader->SetDelta(scaleFactor / originFbo->Width(), 0);
    DrawFBOToFBO(*originFbo, *shadowFbo);
    shadowFbo->UnbindFBO();

    // vertical blur
    if (isOnScreen)
    {
        // draw screen
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        glViewport(0, 0, mWidth, mHeight);
        mCurrentState->mShader->SetDelta(0, 1.0f / shadowFbo->Height());
        DrawFBOToScreen(*shadowFbo, rect.leftTop.x + mCurrentState->mShadowOffsetX,
                        rect.leftTop.y + mCurrentState->mShadowOffsetY,
                        rect.Width(), rect.Height(), mCurrentState->mShadowColor);
    }
    else
    {
        // draw fbo
        outputFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
        outputFbo->BindFBO();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mCurrentState->mShader->SetDelta(0, 1.0f / shadowFbo->Height());
        DrawFBOToFBO(*shadowFbo, *outputFbo);
        outputFbo->UnbindFBO();
    }

    Restore();
}

void GCanvasContext::DrawBlur(const GRectf &rect, float blur, std::function<void()> draw)
{
    GFrameBufferObjectPtr originFbo;
    Blur(rect, blur, draw, originFbo, true, 1.0);
}

void GCanvasContext::DrawShadow(const GRectf &rect, std::function<void()> drawFun)
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        SendVertexBufferToGPU();
        GColorRGBA fillColor = mCurrentState->mFillColor;
        GColorRGBA strokeColor = mCurrentState->mStrokeColor;
        mCurrentState->mFillColor = mCurrentState->mShadowColor;
        mCurrentState->mFillColor.rgba.a *= fillColor.rgba.a;
        mCurrentState->mStrokeColor = mCurrentState->mShadowColor;
        mCurrentState->mStrokeColor.rgba.a *= strokeColor.rgba.a;
        if (mCurrentState->mShadowBlur < 0.01)
        {
            // draw immediately
            Save();
            UseDefaultRenderPipeline();
            DoTranslate(mCurrentState->mShadowOffsetX, mCurrentState->mShadowOffsetY);
            drawFun();
            Restore();
        }
        else
        {
            float old_dpr = mDevicePixelRatio;
            SetDevicePixelRatio(1.0);
            GRectf shadowRect = rect;
            shadowRect.Enlarge(mCurrentState->mShadowBlur * 4, mCurrentState->mShadowBlur * 4);
            DrawBlur(shadowRect, mCurrentState->mShadowBlur, [=]() {
                drawFun();
            });
            SetDevicePixelRatio(old_dpr);
        }
        mCurrentState->mFillColor = fillColor;
        mCurrentState->mStrokeColor = strokeColor;
    }
}

void GCanvasContext::FillRectBlur(float x, float y, float w, float h)
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        GRectf rect;
        rect.leftTop = {x, y};
        rect.bottomRight = {x + w, y + h};
        DrawShadow(rect, [=]() {
            DoFillRect(x, y, w, h);
        });
    }
}

void GCanvasContext::StrokeRectBlur(float x, float y, float w, float h)
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        GRectf rect;
        rect.leftTop = {x, y};
        rect.bottomRight = {x + w, y + h};
        DrawShadow(rect, [=]() {
            DoStrokeRect(x, y, w, h);
        });
    }
}

void GCanvasContext::ImageBlur(float w, float h, int textureId, float sx,
                               float sy, float sw, float sh, float dx,
                               float dy, float dw, float dh)
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        GRectf rect;
        rect.leftTop = {dx, dy};
        rect.bottomRight = {dx + dw, dy + dh};
        if (mCurrentState->mShadowBlur < 0.01)
        {
            // set blur
            mCurrentState->mShadowBlur = 1;
        }
        DrawShadow(rect, [=]() {
            UseTextureRenderPipeline();

            GColorRGBA color = BlendFillColor(this);
            SetTexture(textureId);
            PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color, false);
        });
    }
}

void GCanvasContext::StrokeBlur()
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        GRectf rect;
        mPath.GetRect(rect);
        DrawShadow(rect, [=]() {
            mPath.DrawLinesToContext(this);
        });
    }
}

void GCanvasContext::FillBlur()
{
    if (mCurrentState->mShadowColor.rgba.a > 0.01)
    {
        GRectf rect;
        mPath.GetRect(rect);
        DrawShadow(rect, [=]() {
            mPath.DrawPolygons2DToContext(this, mPath.mFillRule);
        });
    }
}

void GCanvasContext::ApplyFillStylePipeline(bool isStroke)
{
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style == nullptr || style->IsDefault())
    {
        UseDefaultRenderPipeline();
    }
    else
    {
        if (style->IsPattern())
        {
            UsePatternRenderPipeline(isStroke);
        }
        else if (style->IsLinearGradient())
        {
            UseLinearGradientPipeline(isStroke);
        }
        else if (style->IsRadialGradient())
        {
            UseRadialGradientPipeline(isStroke);
        }
    }
}

void GCanvasContext::ClipRegion()
{
    BeforeClip();

    if (mCurrentState->mClipPath)
    {
        delete mCurrentState->mClipPath;
        mCurrentState->mClipPath = nullptr;
    }

    mPath.Close();
    mCurrentState->mClipPath = new GPath(mPath);
    mCurrentState->mClipPath->ClipRegion(this);
    SetClipFlag(true);

    AfterClip();
}

void GCanvasContext::ClipRegionNew(GFillRule rule)
{
    if (mCurrentState->mClipPath)
    {
        delete mCurrentState->mClipPath;
        mCurrentState->mClipPath = nullptr;
    }

    mPath.Close();
    mCurrentState->mClipPath = new GPath(mPath);
    mCurrentState->mClipPath->mFillRule = rule;
    mCurrentState->mClipPath->DrawPolygons2DToContext(this, rule, FILL_TARGET_DEPTH);
}

void GCanvasContext::BeforeClip()
{
    SendVertexBufferToGPU();
    SaveRenderPipeline();
    UseDefaultRenderPipeline();
    mCurrentState->mShader->SetTransform(mCurrentState->mTransform);
    mCurrentState->mShader->SetHasTexture(0);
}

void GCanvasContext::AfterClip()
{
    RestoreRenderPipeline();
}

void GCanvasContext::SetClearColor(const GColorRGBA &c)
{
    mClearColor = c;
}

void GCanvasContext::SaveRenderPipeline()
{
    mSaveShader = mCurrentState->mShader;
    mSaveIsStroke = mCurrentState->mStrokeStyle != nullptr;
}

void GCanvasContext::RestoreRenderPipeline()
{
    std::string name = mSaveShader->GetName();

    if (name == "PATTERN")
    {
        UsePatternRenderPipeline(mSaveIsStroke);
    }
    else if (name == "TEXTURE")
    {
        UseTextureRenderPipeline();
    }
    else if (name == "SHADOW")
    {
        UseShadowRenderPipeline();
    }
    else if (name == "LINEAR")
    {
        UseLinearGradientPipeline(mSaveIsStroke);
    }
    else if (name == "RADIAL")
    {
        UseRadialGradientPipeline(mSaveIsStroke);
    }
    else
    {
        UseDefaultRenderPipeline();
    }
}

void GCanvasContext::UseDefaultRenderPipeline()
{
    GShader *newShader = FindShader("DEFAULT");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }
    SetTexture(InvalidateTextureId);
}

void GCanvasContext::UseTextureRenderPipeline()
{
    GShader *newShader = FindShader("TEXTURE");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();

        TextureShader *textureShader = (TextureShader *)(mCurrentState->mShader);
        if (textureShader != nullptr)
        {
            textureShader->SetPremultipliedAlpha(true);
        }
    }
}

inline double Gaussian(double x, double sigma)
{
    return 1 / exp(x * x / 2 / (sigma * sigma)) / sigma / sqrt(2 * M_PI);
}

inline void WeightCalculate(double radius, float weight[13])
{
    double sigma = radius / 2.5;
    for (int i = 0; i <= 12; ++i)
    {
        weight[i] = Gaussian(i, sigma);
    }
}

void GCanvasContext::UseShadowRenderPipeline()
{
    GShader *newShader = FindShader("SHADOW");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }
}

void GCanvasContext::UseShadowRenderPipeline(double radius)
{
    GShader *newShader = FindShader("SHADOW");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }

    //Shadow with radius
    float weight[13];
    WeightCalculate(radius, weight);
    mCurrentState->mShader->SetWeight(weight, 13);
}

void GCanvasContext::UsePatternRenderPipeline(bool isStroke)
{
    GShader *newShader = FindShader("PATTERN");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }

    //Pattern
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr && style->IsPattern())
    {
        FillStylePattern *pattern = (FillStylePattern *)(style);
        mCurrentState->mShader->SetRepeatMode(pattern->GetPattern());
        mCurrentState->mTextureId = pattern->GetTextureListID();
        mCurrentState->mShader->SetTextureSize(pattern->GetTextureWidth(),
                                               pattern->GetTextureHeight());
        if (mCurrentState->mTextureId != InvalidateTextureId)
        {
            glBindTexture(GL_TEXTURE_2D, mCurrentState->mTextureId);
        }
        //Pattern Alpha
        PatternShader *patternShader = (PatternShader *)(mCurrentState->mShader);
        if (patternShader)
        {
            patternShader->SetPatternAlpha(GlobalAlpha());
        }
    }
}

void GCanvasContext::UseLinearGradientPipeline(bool isStroke)
{
    GShader *newShader = FindShader("LINEAR");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }

    //Linear Gradinet
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr && style->IsLinearGradient())
    {
        FillStyleLinearGradient *grad = (FillStyleLinearGradient *)(style);
        mCurrentState->mShader->SetRange(grad->GetStartPos(), grad->GetEndPos());
        mCurrentState->mShader->SetColorStopCount(grad->GetColorStopCount());

        const int count = grad->GetColorStopCount();
        for (int i = 0; i < count; ++i)
        {
            const FillStyleLinearGradient::ColorStop *stop = grad->GetColorStop(i);
            GColorRGBA c = BlendColor(this, stop->color);
            mCurrentState->mShader->SetColorStop(c.components, stop->pos, i);
        }
    }
}

void GCanvasContext::UseRadialGradientPipeline(bool isStroke)
{
    GShader *newShader = FindShader("RADIAL");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }

    //Radial Gradinet
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr && style->IsRadialGradient())
    {
        FillStyleRadialGradient *grad = (FillStyleRadialGradient *)(style);
        mCurrentState->mShader->SetRange(grad->GetStartPos(), grad->GetEndPos());
        mCurrentState->mShader->SetColorStopCount(grad->GetColorStopCount());

        const int count = grad->GetColorStopCount();
        for (int i = 0; i < count; ++i)
        {
            const FillStyleRadialGradient::ColorStop *stop = grad->GetColorStop(i);
            GColorRGBA c = BlendColor(this, stop->color);
            mCurrentState->mShader->SetColorStop(c.components, stop->pos, i);
        }
    }
}

GTexture *GCanvasContext::GetFontTexture()
{
    if (!mFontTexture)
    {
        std::vector<GCanvasLog> logVec;
        mFontTexture = new GTexture(FontTextureWidth, FontTextureHeight, GL_ALPHA, nullptr, &logVec);
        LOG_EXCEPTION_VECTOR(mHooks, mContextId.c_str(), logVec);
    }

    return mFontTexture;
}

//////////////////////////////////////////////////
/// port API from GCanvas
//////////////////////////////////////////////////

#ifdef IOS
void GCanvasContext::DrawTextWithLength(const char *text, int strLength, float x, float y,
                                        bool isStroke, float maxWidth)
{
    if (strLength == 0)
    {
        strLength = static_cast<int>(strlen(text));
    }
    const GCompositeOperation old_op = mCurrentState->mGlobalCompositeOp;
    DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
    LOG_I("before scalewidth %s", text);
    // scaleWidth
    float scaleWidth = 1.0;
    // check maxWidth
    if (fabs(maxWidth - SHRT_MAX) > 1)
    {
        float measureWidth = MeasureTextWidth(text);
        if (measureWidth > maxWidth)
        {
            scaleWidth = maxWidth / measureWidth;
        }
    }
    FillText((const unsigned short *)text,
             (unsigned int)strLength, x, y, isStroke, scaleWidth);

    DoSetGlobalCompositeOperation(old_op, old_op);
}
#endif

//////////////////////////////////////////////////////////////////////////////
///   Context2D Property
//////////////////////////////////////////////////////////////////////////////
//global
void GCanvasContext::SetGlobalAlpha(float a)
{
    SendVertexBufferToGPU();
    a = std::min<float>(1, std::max<float>(a, 0));
    mCurrentState->mGlobalAlpha = a;
}

void GCanvasContext::SetGlobalCompositeOperation(int op)
{
    DoSetGlobalCompositeOperation(GCompositeOperation(op), GCompositeOperation(op));
}

//font
void GCanvasContext::SetTextAlign(GTextAlign textAlign)
{
    mCurrentState->mTextAlign = textAlign;
}

void GCanvasContext::SetTextBaseline(GTextBaseline textbaseline)
{
    mCurrentState->mTextBaseline = textbaseline;
}

void GCanvasContext::SetFont(const char *font)
{
    if (mCurrentState->mFont != nullptr)
    {
        if (strcmp(mCurrentState->mFont->GetOriginFontName().c_str(), font) == 0)
        {
            return;
        }
        delete mCurrentState->mFont;
    }
    mCurrentState->mFont = new GFontStyle(font, mDevicePixelRatio);
}

//shadow
void GCanvasContext::SetShadowColor(const char *str)
{
    GColorRGBA color = StrValueToColorRGBA(str);
    mCurrentState->mShadowColor = color;
}

void GCanvasContext::SetShadowBlur(float blur)
{
    mCurrentState->mShadowBlur = blur;
}

void GCanvasContext::SetShadowOffsetX(float x)
{
    mCurrentState->mShadowOffsetX = x;
}

void GCanvasContext::SetShadowOffsetY(float y)
{
    mCurrentState->mShadowOffsetY = y;
}

//style
void GCanvasContext::SetFillStyle(const char *str)
{
    if ((str != NULL) && (str[0] == '\0'))
    { // empty str
        return;
    }
    GColorRGBA color = StrValueToColorRGBA(str);
    SetFillStyle(color);
}

void GCanvasContext::SetFillStyle(GColorRGBA c)
{
    if (mCurrentState->mFillStyle != nullptr)
    {
        delete mCurrentState->mFillStyle;
        mCurrentState->mFillStyle = nullptr;
    }

    mCurrentState->mFillColor = c;
    UseDefaultRenderPipeline();
}

void GCanvasContext::SetStrokeStyle(const char *str)
{
    if ((str != NULL) && (str[0] == '\0'))
    { // empty str
        return;
    }
    GColorRGBA color = StrValueToColorRGBA(str);
    SetStrokeStyle(color);
}

void GCanvasContext::SetStrokeStyle(const GColorRGBA &c)
{
    if (mCurrentState->mStrokeStyle != nullptr)
    {
        delete mCurrentState->mStrokeStyle;
        mCurrentState->mStrokeStyle = nullptr;
    }

    mCurrentState->mStrokeColor = c;
    UseDefaultRenderPipeline();
    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->SetHasTexture(0);
    }
}

void GCanvasContext::SetFillStylePattern(int textureId, int width, int height, const char *repeatMode,
                                         bool isStroke)
{
    //change fill style, need send vertex buffer
    SendVertexBufferToGPU();

    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr)
    {
        delete style;
        style = nullptr;
    }

    style = new FillStylePattern(textureId, width, height, repeatMode);
    if (isStroke)
    {
        mCurrentState->mStrokeStyle = style;
    }
    else
    {
        mCurrentState->mFillStyle = style;
    }
}

void GCanvasContext::SetFillStyleLinearGradient(float startArr[], float endArr[], int stop_count,
                                                const float posArray[], const std::string colorArray[],
                                                bool isStroke)
{
    //change fill style, need send vertex buffer
    SendVertexBufferToGPU();

    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;

    if (style != nullptr)
    {
        delete style;
        style = nullptr;
    }

    GPoint start, end;
    start.x = startArr[0];
    start.y = startArr[1];
    end.x = endArr[0];
    end.y = endArr[1];

    style = new FillStyleLinearGradient(start, end);
    if (isStroke)
    {
        mCurrentState->mStrokeStyle = style;
    }
    else
    {
        mCurrentState->mFillStyle = style;
    }

    for (int i = 0; i < stop_count; ++i)
    {
        float pos = posArray[i];
        std::string color = colorArray[i];
        static_cast<FillStyleLinearGradient *>(style)->AddColorStop(pos, color);
    }
}

void GCanvasContext::SetFillStyleRadialGradient(float startArr[], float endArr[], int stop_count,
                                                const float posArray[], const std::string colorArray[],
                                                bool isStroke)
{
    //change fill style, need send vertex buffer
    SendVertexBufferToGPU();

    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;

    if (style != nullptr)
    {
        delete style;
        style = nullptr;
    }
    style = new FillStyleRadialGradient(startArr, endArr);
    if (isStroke)
    {
        mCurrentState->mStrokeStyle = style;
    }
    else
    {
        mCurrentState->mFillStyle = style;
    }

    for (int i = 0; i < stop_count; ++i)
    {
        float pos = posArray[i];
        std::string color = colorArray[i];
        static_cast<FillStyleRadialGradient *>(style)->AddColorStop(pos, color);
    }
}

//path
void GCanvasContext::SetLineCap(const char *p)
{
    GLineCap lineCap = LINE_CAP_BUTT;
    if (strncmp(p, "butt", 4) == 0)
    {
        lineCap = LINE_CAP_BUTT;
    }
    else if (strncmp(p, "round", 5) == 0)
    {
        lineCap = LINE_CAP_ROUND;
    }
    else if (strncmp(p, "square", 6) == 0)
    {
        lineCap = LINE_CAP_SQUARE;
    }
    SetLineCap(lineCap);
}

void GCanvasContext::SetLineJoin(const char *p)
{
    GLineJoin lineJoin = LINE_JOIN_MITER;
    if (strncmp(p, "miter", 4) == 0)
    {
        lineJoin = LINE_JOIN_MITER;
    }
    else if (strncmp(p, "bevel", 5) == 0)
    {
        lineJoin = LINE_JOIN_BEVEL;
    }
    else if (strncmp(p, "round", 5) == 0)
    {
        lineJoin = LINE_JOIN_ROUND;
    }
    SetLineJoin(lineJoin);
}

std::vector<float> &GCanvasContext::GetLineDash()
{
    return mCurrentState->mLineDash;
}

//////////////////////////////////////////////////////////////////////////////
///   Context2D Method
//////////////////////////////////////////////////////////////////////////////
void GCanvasContext::ClearScreen()
{
    LOG_D("ClearScreen: r:%f, g:%f, b:%f, a:%f", mClearColor.rgba.r, mClearColor.rgba.g, mClearColor.rgba.b,
          mClearColor.rgba.a);
    glClearColor(mClearColor.rgba.r, mClearColor.rgba.g, mClearColor.rgba.b, mClearColor.rgba.a);
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/**
 * change canvas dimension
 * reset the statue to default，and update OpenGL to defualt
 */
void GCanvasContext::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (mContextType == 0)
    {
        mVertexBufferIndex = 0;
        UpdateProjectTransform();
        ResetStateStack();
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        UseDefaultRenderPipeline();
    }

    mFboMap.erase(GCanvasContext::DefaultFboName);
    InitFBO();
    BindFBO();
}

void GCanvasContext::SetTransform(float a, float b, float c, float d, float tx, float ty)
{
    mCurrentState->mscaleFontX = 1.0f * a;
    mCurrentState->mscaleFontY = 1.0f * d;

    SendVertexBufferToGPU();
    GTransform t = GTransform(a, c, b, d, tx, ty);
    mCurrentState->mTransform = GTransformConcat(mProjectTransform, t);
}

void GCanvasContext::Transfrom(float a, float b, float c, float d, float tx, float ty)
{
    mCurrentState->mscaleFontX = mCurrentState->mscaleFontX * a;
    mCurrentState->mscaleFontY = mCurrentState->mscaleFontY * d;

    SendVertexBufferToGPU();
    GTransform t = GTransform(a, c, b, d, tx, ty);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);
}

void GCanvasContext::ResetTransform()
{
    mCurrentState->mscaleFontX = 1.0f;
    mCurrentState->mscaleFontY = 1.0f;

    SendVertexBufferToGPU();
    mCurrentState->mTransform = GTransformConcat(mProjectTransform, GTransformIdentity);
}

void GCanvasContext::DoResetTransform()
{
    SetTransformOfShader(mProjectTransform);
}

void GCanvasContext::Scale(float sx, float sy)
{
    mCurrentState->mscaleFontX = mCurrentState->mscaleFontX * sx;
    mCurrentState->mscaleFontY = mCurrentState->mscaleFontY * sy;

    SendVertexBufferToGPU();
    GTransform t = GTransformMakeScale(sx, sy);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);
}

void GCanvasContext::DoScale(float sx, float sy)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeScale(sx, sy));
}

void GCanvasContext::Rotate(float angle)
{
    SendVertexBufferToGPU();
    GTransform t = GTransformMakeRotation(angle);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);
}

void GCanvasContext::DoRotate(float angle)
{
    mCurrentState->mTransform = GTransformRotate(mCurrentState->mTransform, angle);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::Translate(float tx, float ty)
{
    SendVertexBufferToGPU();
    GTransform t = GTransformMakeTranslation(tx, ty);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);
}

void GCanvasContext::DoTranslate(float tx, float ty)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeTranslation(tx, ty));
}

//GCanvasState save & restore
void GCanvasContext::Save()
{
    mStateStack.push_back(mStateStack.back());
    mCurrentState = &mStateStack.back();
}

bool GCanvasContext::Restore()
{
    SendVertexBufferToGPU();

    if (mStateStack.size() <= 1)
    {
        return false;
    }

    if (mCurrentState->mClipPath)
    {
        ResetClip();
    }

    GShader *oldShader = mCurrentState->mShader;

    mStateStack.pop_back();
    mCurrentState = &mStateStack.back();

    auto op = mCurrentState->mGlobalCompositeOp;
    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);

    glBlendFuncSeparate(funcs.source, funcs.destination,
                        funcs.source, funcs.destination);

    if (mCurrentState->mShader != oldShader)
    {
        mCurrentState->mShader->Bind();
    }

    SetClipFlag(false);
    if (mCurrentState->mClipPath)
    {
        mCurrentState->mClipPath->DrawPolygons2DToContext(this,
                                                          mCurrentState->mClipPath->mFillRule,
                                                          FILL_TARGET_DEPTH);
        SetClipFlag(true);
    }

    return true;
}

//path
void GCanvasContext::BeginPath()
{
    mPath.Reset();
}

void GCanvasContext::ClosePath()
{
    mPath.Close();
}

void GCanvasContext::MoveTo(float x, float y)
{
    mPath.MoveTo(x, y);
}

void GCanvasContext::LineTo(float x, float y)
{
    mPath.LineTo(x, y);
}

void GCanvasContext::Rect(float x, float y, float w, float h)
{
    mPath.MoveTo(x, y);
    mPath.LineTo(x + w, y);
    mPath.LineTo(x + w, y + h);
    mPath.LineTo(x, y + h);
    mPath.Close();
}

void GCanvasContext::Arc(float x, float y, float radius, float startAngle, float endAngle,
                         bool anticlockwise)
{
    mPath.Arc(x, y, radius, startAngle, endAngle, anticlockwise);
}

void GCanvasContext::ArcTo(float x1, float y1, float x2, float y2, float radius)
{
    mPath.ArcTo(x1, y1, x2, y2, radius);
}

void GCanvasContext::QuadraticCurveTo(float cpx, float cpy, float x, float y)
{
    float sx = mCurrentState->mTransform.a * GetWidth();
    float sy = mCurrentState->mTransform.d * GetHeight();
    float scale = sqrt(sx * sx + sy * sy);
    mPath.QuadraticCurveTo(cpx, cpy, x, y, scale);
}

void GCanvasContext::BezierCurveTo(float cp1x, float cp1y, float cp2x,
                                   float cp2y, float x, float y)
{
    float scale = mCurrentState->mTransform.a * mCurrentState->mTransform.a +
                  mCurrentState->mTransform.d * mCurrentState->mTransform.d;
    scale = 1 / sqrtf(scale);
    mPath.BezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y, scale);
}

//rect
void GCanvasContext::FillRect(float x, float y, float w, float h)
{
    FillRectBlur(x, y, w, h);
    DoFillRect(x, y, w, h);
}

void GCanvasContext::DoFillRect(float x, float y, float w, float h)
{
    GColorRGBA color = BlendFillColor(this);
    ApplyFillStylePipeline();
    PushRectangle(x, y, w, h, 0, 0, 0, 0, color);
}

void GCanvasContext::StrokeRect(float x, float y, float w, float h)
{
    StrokeRectBlur(x, y, w, h);
    DoStrokeRect(x, y, w, h);
}

void GCanvasContext::DoStrokeRect(float x, float y, float w, float h)
{
    ApplyFillStylePipeline(true);
    GPath *tempPath = new GPath();

    tempPath->MoveTo(x, y);
    tempPath->LineTo(x + w, y);
    tempPath->LineTo(x + w, y + h);
    tempPath->LineTo(x, y + h);
    tempPath->Close();

    tempPath->DrawLinesToContext(this);
    delete tempPath;
}

void GCanvasContext::ClearRect(float x, float y, float w, float h)
{
    if (x == 0 && y == 0 && SIZE_EPSILON > fabs(w * GetCanvasDimensionWidthScale() - mWidth) &&
        SIZE_EPSILON > fabs(h * GetCanvasDimensionHeightScale() - mHeight))
    {
        ClearScreen();
        return;
    }

    UseDefaultRenderPipeline();

    GCompositeOperation oldOp = mCurrentState->mGlobalCompositeOp;
    SendVertexBufferToGPU();
    DoSetGlobalCompositeOperation(COMPOSITE_OP_REPLACE, COMPOSITE_OP_REPLACE);

    PushRectangle(x, y, w, h, 0, 0, 0, 0, mClearColor);

    DoSetGlobalCompositeOperation(oldOp, oldOp);
}

//clip strok fill
void GCanvasContext::Clip(GFillRule rule)
{
    ClipRegionNew(rule);
}

void GCanvasContext::ResetClip()
{
    if (mCurrentState->mClipPath)
    {
        delete mCurrentState->mClipPath;
        mCurrentState->mClipPath = nullptr;

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_ALWAYS);

        glClear(GL_STENCIL_BUFFER_BIT);
        glDisable(GL_STENCIL_TEST);

        SetClipFlag(false);
    }
}

void GCanvasContext::Stroke()
{
    StrokeBlur();
    ApplyFillStylePipeline(true);
    mPath.DrawLinesToContext(this);
}

void GCanvasContext::Fill(GFillRule rule)
{
    FillBlur();
    ApplyFillStylePipeline();
    mPath.DrawPolygons2DToContext(this, rule);
}

//text
void GCanvasContext::DrawText(const char *text, float x, float y, float maxWidth)
{
    DrawTextWithLength(text, (int)strlen(text), x, y, false, maxWidth);
}

void GCanvasContext::StrokeText(const char *text, float x, float y, float maxWidth)
{
    DrawTextWithLength(text, (int)strlen(text), x, y, true, maxWidth);
}

float GCanvasContext::MeasureTextWidth(const char *text, int strLength)
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
    return width / mDevicePixelRatio;
}

//image
void GCanvasContext::DrawImage(int textureId, int textureWidth, int textureHeight,
                               float sx, float sy, float sw, float sh,
                               float dx, float dy, float dw, float dh,
                               bool flipY)
{
    ImageBlur(textureWidth, textureHeight, textureId, sx, sy, sw, sh, dx, dy, dw, dh);
    UseTextureRenderPipeline();
    DoDrawImage(textureWidth, textureHeight, textureId, sx, sy, sw, sh, dx, dy, dw, dh, flipY);
}

/**
 * DoDrawImage实现
 * sx, sw 支持为负值，sw, sh必须为正数
 */
void GCanvasContext::DoDrawImage(float w, float h, int TextureId, float sx,
                                 float sy, float sw, float sh, float dx,
                                 float dy, float dw, float dh, bool flipY)
{
    if (TextureId <= 0)
    {
        LOG_EXCEPTION(mHooks, mContextId, "texture invalid", "<function:%s> textureId=%i", __FUNCTION__, TextureId);
        return;
    }

    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(TextureId);
    PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color, flipY);
}

void GCanvasContext::GetImageData(int x, int y, int width, int height, uint8_t *pixels)
{
    SendVertexBufferToGPU();

    std::vector<int> rawPixel;

    //calc real width & height
    float sw = GetCanvasDimensionWidthScale();
    float sh = GetCanvasDimensionHeightScale();

    int realX = x * sw;
    int realY = y * sh;
    int realWidth = width * sw;
    int realHeight = height * sh;

    rawPixel.resize(realWidth * realHeight);

    //read pixels
    glReadPixels(realX, mHeight - (realY + realHeight), realWidth, realHeight, GL_RGBA,
                 GL_UNSIGNED_BYTE, &rawPixel[0]);
    GLenum glerror = glGetError();
    if (glerror)
    {
        LOG_EXCEPTION(mHooks, mContextId, "glReadPixels_fail", "<function:%s, glGetError:%x>", __FUNCTION__,
                      glerror);
    }
    // sample
    gcanvas::PixelsSampler(realWidth, realHeight, &rawPixel[0], width, height, reinterpret_cast<int *>(pixels));
}

void GCanvasContext::PutImageData(const unsigned char *rgbaData, int tw,
                                  int th, int x, int y, int sx, int sy,
                                  int sw, int sh, bool is_src_flip_y)
{
    SendVertexBufferToGPU();

    std::vector<GCanvasLog> logVec;
    GLuint glID = gcanvas::PixelsBindTexture(rgbaData, GL_RGBA, tw, th, &logVec);
    LOG_EXCEPTION_VECTOR(mHooks, mContextId, logVec);

    sw = sw > tw ? tw : sw;
    sh = sh > th ? th : sh;
    sw -= sx;
    sh -= sy;
    DrawImage(glID, tw, th, sx, sy, sw, sh, x + sx, y + sy, sw, sh);

    SendVertexBufferToGPU();
    SetTexture(InvalidateTextureId);
    glDeleteTextures(1, &glID);
}

int GCanvasContext::BindImage(const unsigned char *rgbaData, GLint format, unsigned int width, unsigned int height)
{
    return gcanvas::PixelsBindTexture(rgbaData, format, width, height);
}
