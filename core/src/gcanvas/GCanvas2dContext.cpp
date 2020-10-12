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
#include "GL/GLUtil.h"
#include <stdio.h>
#include <assert.h>

#define SIZE_EPSILON 1.f

using namespace gcanvas;

GBlendOperationFuncs GCompositeOperationFuncs(int index)
{
    static GBlendOperationFuncs funcs[] = {
        {GL_ONE, GL_ONE_MINUS_SRC_ALPHA},                 // 0 source-over
        {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA},           // 1 source-atop
        {GL_DST_ALPHA, GL_ZERO},                          // 2 source-in
        {GL_ONE_MINUS_DST_ALPHA, GL_ZERO},                // 3 source-out
        {GL_ONE_MINUS_DST_ALPHA, GL_ONE},                 // 4 destination-over
        {GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA},           // 5 destination-atop
        {GL_ZERO, GL_SRC_ALPHA},                          // 6 destination-in
        {GL_ZERO, GL_ONE_MINUS_SRC_ALPHA},                // 7 destination-out
        {GL_ONE, GL_ONE},                                 // 8 lighter
        {GL_ONE, GL_ZERO},                                // 9 copy
        {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}, // 10 xor
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
    if (mFontManager == nullptr)
    {
        return;
    }

    ApplyFillStylePipeline(isStroke);

    if (mCurrentState->mFont == nullptr)
    {
        mCurrentState->mFont = new GFontStyle(nullptr, mDevicePixelRatio);
    }
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetOverideTextureColor(1);
    }

    // if current transform scaled, we load scaled font, increase display effect
    // get scale x, y from current transform
    float sx = GTransformGetScaleX(mCurrentState->mTransform);
    float sy = GTransformGetScaleY(mCurrentState->mTransform);

    SendVertexBufferToGPU();
    Save();
    Translate(x, y);
    Scale(1 / mDevicePixelRatio * scaleWidth, 1 / mDevicePixelRatio);
    mFontManager->DrawText(text, text_length, 0, 0, isStroke, this, sx, sy);
    Restore();
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetOverideTextureColor(0);
    }
}

GCanvasContext::GCanvasContext(short w, short h, const GCanvasConfig &config, GCanvasHooks *hooks) : mIsFboSupported(true), mWidth(w), mHeight(h), mIsContextReady(false),
                                                                                                     mHasClipRegion(false), mHiQuality(false), mVertexBufferIndex(0),
                                                                                                     mSaveShader(nullptr),
                                                                                                     mClearColor(GColorTransparentWhite),
                                                                                                     mDevicePixelRatio(1.f),
                                                                                                     mContextType(0),
                                                                                                     mConfig(config)
{
    mHooks = hooks;
    // init state
    mStateStack.clear();
    GCanvasState *state = new GCanvasState();
    mStateStack.push_back(state);
    mCurrentState = mStateStack.back();

    if (mWidth > 0 && mHeight > 0)
    {
        UpdateProjectTransform();
        InitFBO();
    }

    if (!mConfig.sharedFont)
    {
        mFontManager = GFontManager::NewInstance();
    }
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

void GCanvasContext::SetFontManager(GFontManager *fontManager)
{
    mFontManager = fontManager;
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
        mCurrentState->mTransform = GTransformIdentity;
        if (!GTransformEqualToTransform(old, mCurrentState->mTransform))
        {
            // update shader transform
            SetTransformOfShader(mProjectTransform);
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

GTransform GCanvasContext::CalculateProjectTransform(int w, int h, bool needFlipY)
{
    GTransform t = GTransformIdentity;
    if (mConfig.flip || needFlipY)
    {
        t = GTransformTranslate(t, -1.f, -1.f);
        t = GTransformScale(t, 2.f * mDevicePixelRatio / w,
                            2.f * mDevicePixelRatio / h);
    }
    else
    {
        t = GTransformTranslate(t, -1.f, 1.f);
        t = GTransformScale(t, 2.f * mDevicePixelRatio / w,
                            -2.f * mDevicePixelRatio / h);
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

        LOG_EXCEPTION_VECTOR(mHooks, mContextId, logVec);
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
    if (!mConfig.sharedFont)
    {
        delete mFontManager;
        mFontManager = nullptr;
    }
}

bool GCanvasContext::InitializeGLEnvironment()
{
    if (0 != mContextType)
    { // only 2D need to init
        return true;
    }
    LOG_E("initializeGLEnvironment");
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
        // LOG_E("mIsGLInited");
        ResetStateStack();
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        InitializeGLShader();
        mIsGLInited = true;
    }

    // check context status
    CheckContextStatus();

    return true;
}

void GCanvasContext::CheckContextStatus()
{
    // size
    if (mWidth == 0 || mHeight == 0)
    {
        mIsContextReady = false;
    }
    // shader
    GShaderManager *sm = GetShaderManager();
    if (sm == nullptr || !sm->isAllShaderCompleted())
    {
        mIsContextReady = false;
    }
    mIsContextReady = true;
}

bool GCanvasContext::InitializeGLShader()
{
    UsePatternRenderPipeline();
    GShader::TraceErrorIfHas(FindShader("PATTERN"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseLinearGradientPipeline();
    GShader::TraceErrorIfHas(FindShader("LINEAR"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseRadialGradientPipeline();
    GShader::TraceErrorIfHas(FindShader("RADIAL"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseTextureRenderPipeline();
    GShader::TraceErrorIfHas(FindShader("TEXTURE"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseBlurRenderPipeline();
    GShader::TraceErrorIfHas(FindShader("BLUR"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseShadowRenderPipeline();
    GShader::TraceErrorIfHas(FindShader("SHADOW"), mHooks, mContextId);
    if (nullptr == mCurrentState->mShader)
    {
        return false;
    }
    BindVertexBuffer();

    UseDefaultRenderPipeline();
    GShader::TraceErrorIfHas(FindShader("DEFAULT"), mHooks, mContextId);
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
 * reset gcanvas to default state，only called on init and dimension changed
 * states include: fillStyle/strokeStyle,lineJoin/miter,textAlign,font style,
 * shadow, globalCompositeOperation,clip, transform
*/
void GCanvasContext::ResetStateStack()
{
    mPath.Reset();

    bool hasOldState = (nullptr != mCurrentState);
    if (hasOldState)
    {
        ResetClip();
    }

    mStateStack.clear();
    mStateStack.push_back(new GCanvasState());
    mCurrentState = mStateStack.back();

    if (!hasOldState)
    {
        DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
    }

    SetTransformOfShader(mProjectTransform);

    SetClipFlag(false);
}

void GCanvasContext::BindVertexBuffer()
{
    BindPositionVertexBuffer();

    if (mCurrentState->mShader && mCurrentState->mShader->GetTexcoordSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetTexcoordSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetTexcoordSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *)CanvasVertexBuffer) + 2);
    }
    if (mCurrentState->mShader && mCurrentState->mShader->GetColorSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetColorSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetColorSlot(), 4,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *)CanvasVertexBuffer) + 4);
    }
}

GLuint GCanvasContext::PositionSlot()
{
    if (mCurrentState->mShader)
    {
        return (GLuint)mCurrentState->mShader->GetPositionSlot();
    }
    return -1;
}

void GCanvasContext::BindPositionVertexBuffer()
{
    if (mCurrentState->mShader && mCurrentState->mShader->GetPositionSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint)mCurrentState->mShader->GetPositionSlot());
        glVertexAttribPointer((GLuint)mCurrentState->mShader->GetPositionSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              CanvasVertexBuffer);
    }
}

void GCanvasContext::SetTexture(int textureId)
{
    // LOG_E("SetTexture %i, current=%i", textureId, mCurrentState->mTextureId);
    if (mCurrentState->mTextureId != textureId)
    {
        // FIXME optim: 修改纹理 不应该触发flush
        // SendVertexBufferToGPU();
        SendVertexBufferToGPU();
        mCurrentState->mTextureId = textureId;
    }
}

void GCanvasContext::SendVertexBufferToGPU(const GLenum geometry_type)
{
    if (mVertexBufferIndex == 0)
    {
        return;
    }
    if (mCurrentState != nullptr)
    {
        if (mCurrentState->mShader)
        {
            mCurrentState->mShader->SetTransform(mProjectTransform);

            if (mCurrentState->mTextureId != InvalidateTextureId)
            {
                mCurrentState->mShader->SetHasTexture(true);
            }
            else
            {
                mCurrentState->mShader->SetHasTexture(false);
            }
        }
        if (mCurrentState->mTextureId != InvalidateTextureId)
        {
            glBindTexture(GL_TEXTURE_2D, mCurrentState->mTextureId);
        }
        //draw call
        mDrawCallCount++;
        glDrawArrays(geometry_type, 0, mVertexBufferIndex);
    }

    mVertexBufferIndex = 0;
}

bool GCanvasContext::NeedSendVertexBufferToGPUWithSize(size_t size)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - size)
    {
        SendVertexBufferToGPU();
        return true;
    }
    return false;
}

void GCanvasContext::SendVertexBufferToGPUOptim(const GLenum geometry_type)
{
    if (mVertexBufferIndex == 0)
    {
        return;
    }

    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetTransform(mProjectTransform);
        mCurrentState->mShader->SetHasTexture(mCurrentState->mTextureId != InvalidateTextureId);
    }

    auto iter = mVertexShaderProperties.begin();
    GShader *prevShader = nullptr;
    bool shaderChanged = false;
    // LOG_E("SendVertexBufferToGPU: vertex property count=%i", mVertexShaderProperties.size());
    for (; iter != mVertexShaderProperties.end(); ++iter)
    {
        //draw call
        mDrawCallCount++;

        GShader *newShader = FindShader(iter->shaderType.data());
        if (newShader != nullptr && prevShader != newShader)
        {
            prevShader = newShader;
            newShader->Bind();
            shaderChanged = true;
        }

        if (iter->shaderType == "TEXTURE")
        {
            if (shaderChanged)
            {
                TextureShader *textureShader = (TextureShader *)(newShader);
                textureShader->SetPremultipliedAlpha(true);
            }

            if (iter->textureId != InvalidateTextureId)
            {
                glBindTexture(GL_TEXTURE_2D, iter->textureId);
            }
        }
        else
        {
            // TODO switch to other Pipeline
        }

        int start = iter->vertexStartIndex;
        int count = iter->vertexCount;

        glDrawArrays(geometry_type, start, count);
    }

    mVertexShaderProperties.clear();
    mVertexBufferIndex = 0;
}

void GCanvasContext::PushTriangle(GPoint v1, GPoint v2, GPoint v3, GColorRGBA color,
                                  GTransform transform, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    GVertex vertex[3];
    if (vec)
    {
        vb = vertex;
    }
    else
    {
        // push to vertexBuffer
        NeedSendVertexBufferToGPUWithSize(3);
        vb = &CanvasVertexBuffer[mVertexBufferIndex];
    }
    GPoint p = {0, 0};

    if (!GTransformIsIdentity(transform))
    {
        GPointApplyGTransformInPlace(v1, transform);
        GPointApplyGTransformInPlace(v2, transform);
        GPointApplyGTransformInPlace(v3, transform);
    }

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
        SaveVertexShaderProperty(mVertexBufferIndex, 3);
        // push to vertexBuffer
        mVertexBufferIndex += 3;
    }
}

void GCanvasContext::SaveVertexShaderProperty(int offset, int count)
{
    // comment out for unvery optim
    //    std::string shaderType = mCurrentState->mShader->GetName();
    //    int textureId = mCurrentState->mTextureId;
    //    bool merged = false;
    //    auto iter = mVertexShaderProperties.begin();
    //    for (; iter != mVertexShaderProperties.end(); ++iter) {
    //        int nextOffset = iter->vertexStartIndex + iter->vertexCount;
    //        // LOG_E("SaveVertexShaderProperty offset=%i, nextOffset=%i", offset, nextOffset);
    //        if (nextOffset == offset) {
    //            // TODO 是否相同状态，优化shader判断
    //            if (shaderType == iter->shaderType && textureId == iter->textureId) {
    //                iter->vertexCount = iter->vertexCount + count;
    //
    //                merged = true;
    //            }
    //        } else {
    //
    //        }
    //    }
    //
    //    if (!merged) {
    //        GCanvasVertexShaderProperty property;
    //        property.shaderType = shaderType;
    //        property.textureId = textureId;
    //        property.vertexStartIndex = offset;
    //        property.vertexCount = count;
    //
    //        mVertexShaderProperties.push_back(property);
    //    }
}

void GCanvasContext::DrawTexture(int textureId, float *vertexList)
{
    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(textureId);

    NeedSendVertexBufferToGPUWithSize(3);

    GPoint p1 = PointMake(vertexList[0], vertexList[1]);
    GPoint p2 = PointMake(vertexList[4], vertexList[5]);
    GPoint p3 = PointMake(vertexList[8], vertexList[9]);

    GPoint t1 = PointMake(vertexList[2], vertexList[3]);
    GPoint t2 = PointMake(vertexList[6], vertexList[7]);
    GPoint t3 = PointMake(vertexList[10], vertexList[11]);

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    vb[0].pos = p1;
    vb[0].uv = t1;
    vb[1].pos = p2;
    vb[1].uv = t2;
    vb[2].pos = p3;
    vb[2].uv = t3;

    vb[0].color = vb[1].color = vb[2].color = color;
    mVertexBufferIndex += 3;

    SaveVertexShaderProperty(mVertexBufferIndex - 3, 3);
}

void GCanvasContext::DrawTextureArray(int textureId, int count, float *vertexList, float *texList)
{
    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(textureId);

    NeedSendVertexBufferToGPUWithSize(count);

    // max size
    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    for (int i = 0; i < count; i++)
    {
        vb[i].pos.x = vertexList[i * 2];
        vb[i].pos.y = vertexList[i * 2 + 1];

        vb[i].uv.x = texList[i * 2];
        vb[i].uv.y = texList[i * 2 + 1];

        vb[i].color = color;
    }
    mVertexBufferIndex += count;

    SaveVertexShaderProperty(mVertexBufferIndex - count, count);
}

void GCanvasContext::PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                              GColorRGBA color, GTransform transform, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    GVertex vertex[6];
    if (vec)
    {
        vb = vertex;
    }
    else
    {
        // push to vertexBuffer
        NeedSendVertexBufferToGPUWithSize(6);
        vb = &CanvasVertexBuffer[mVertexBufferIndex];
    }
    GPoint p = {0, 0};

    if (!GTransformIsIdentity(transform))
    {
        GPointApplyGTransformInPlace(v1, transform);
        GPointApplyGTransformInPlace(v2, transform);
        GPointApplyGTransformInPlace(v3, transform);
        GPointApplyGTransformInPlace(v4, transform);
    }

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
        SaveVertexShaderProperty(mVertexBufferIndex - 6, 6);
    }
}

void GCanvasContext::PushRectangle(float x, float y, float w, float h,
                                   float tx, float ty, float tw, float th,
                                   GColorRGBA color, GTransform transform, bool flipY,
                                   std::vector<GVertex> *vec)
{
    PushRectangleFormat(x, y, w, h, tx, ty, tw, th, color, transform, flipY, vec, false);
}

void GCanvasContext::PushRectangleFormat(float x, float y, float w, float h,
                                         float tx, float ty, float tw, float th,
                                         GColorRGBA color, GTransform transform, bool flipY,
                                         std::vector<GVertex> *vec, bool formatIntVertex)
{
    GVertex *vb = NULL;
    GVertex vertex[6];
    if (vec)
    {
        // push to std::vector
        vb = vertex;
    }
    else
    {
        // push to vertexBuffer
        NeedSendVertexBufferToGPUWithSize(6);
        vb = &CanvasVertexBuffer[mVertexBufferIndex];
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

    if (!GTransformIsIdentity(transform))
    {
        GPointApplyGTransformInPlace(p11, transform);
        GPointApplyGTransformInPlace(p21, transform);
        GPointApplyGTransformInPlace(p12, transform);
        GPointApplyGTransformInPlace(p22, transform);
    }

    if (formatIntVertex)
    {
        p11.x = (int)(p11.x + 0.5);
        p11.y = (int)(p11.y + 0.5);
        p21.x = (int)(p21.x + 0.5);
        p21.y = (int)(p21.y + 0.5);
        p12.x = (int)(p12.x + 0.5);
        p12.y = (int)(p12.y + 0.5);
        p22.x = (int)(p22.x + 0.5);
        p22.y = (int)(p22.y + 0.5);
    }

    GPoint t11 = PointMake(tx, ty);
    GPoint t21 = PointMake(tx + tw, ty);
    GPoint t12 = PointMake(tx, ty + th);
    GPoint t22 = PointMake(tx + tw, ty + th);
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
        SaveVertexShaderProperty(mVertexBufferIndex - 6, 6);
    }
}

void GCanvasContext::PushRectangle4TextureArea(float x, float y, float w, float h,
                                               float tx, float ty, float tw, float th, GColorRGBA color,
                                               GTransform transform, bool flipY)
{
    NeedSendVertexBufferToGPUWithSize(6);

    if (flipY)
    {
        ty = th - ty; //th is ratio to texture
        th *= -1;
    }

    GPoint p11 = PointMake(x, y);
    GPoint p21 = PointMake(x + w, y);
    GPoint p12 = PointMake(x, y + h);
    GPoint p22 = PointMake(x + w, y + h);

    if (!GTransformIsIdentity(transform))
    {
        GPointApplyGTransformInPlace(p11, transform);
        GPointApplyGTransformInPlace(p21, transform);
        GPointApplyGTransformInPlace(p12, transform);
        GPointApplyGTransformInPlace(p22, transform);
    }
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

    SaveVertexShaderProperty(mVertexBufferIndex - 6, 6);
}

void GCanvasContext::PushReverseRectangle(float x, float y, float w, float h,
                                          float tx, float ty, float tw, float th,
                                          GColorRGBA color)
{
    NeedSendVertexBufferToGPUWithSize(6);

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

    SaveVertexShaderProperty(mVertexBufferIndex - 6, 6);
}

/**
 * deprecated
 */
void GCanvasContext::PushPoints(const std::vector<GPoint> &points,
                                GColorRGBA color)
{
    GPoint uv = PointMake(0, 0);
    for (int i = 0; i + 2 < (int)points.size(); i += 3)
    {
        NeedSendVertexBufferToGPUWithSize(3);

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
        NeedSendVertexBufferToGPUWithSize(3);

        int start = mVertexBufferIndex;
        for (int j = 0; j < 3; ++j)
        {
            CanvasVertexBuffer[mVertexBufferIndex] = vertexs[i + j];
            mVertexBufferIndex++;
        }

        SaveVertexShaderProperty(start, 3);
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

            // first add head and SendVertexBufferToGPU
            CanvasVertexBuffer[mVertexBufferIndex].pos = head;
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;

            SaveVertexShaderProperty(mVertexBufferIndex - 1, 1);

            SendVertexBufferToGPU(GL_TRIANGLE_FAN);

            // second add head
            CanvasVertexBuffer[mVertexBufferIndex].pos = head;
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;

            SaveVertexShaderProperty(mVertexBufferIndex - 1, 1);
        }

        int start = mVertexBufferIndex;
        for (int j = 0; j < segmentSize; ++j)
        {
            CanvasVertexBuffer[mVertexBufferIndex].pos = points[i + j];
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;
        }

        SaveVertexShaderProperty(start, segmentSize);
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

GShaderManager *GCanvasContext::GetShaderManager()
{
    return GShaderManager::getSingleton();
}

GShader *GCanvasContext::FindShader(const char *name)
{
    GShaderManager *s = GetShaderManager();
    if (s == nullptr)
    {
        return nullptr;
    }
    else
    {
        return s->programForKey(name);
    }
}

void GCanvasContext::SetTransformOfShader(const GTransform &trans)
{
    // emit the buffered geometry data
    // before the updating of transform matrix
    SendVertexBufferToGPU();

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

        shader->SetTransform(trans);
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
    int w = dest.ExpectedWidth();
    int h = dest.ExpectedHeight();
    glViewport(0, 0, w, h);
    DoSetGlobalCompositeOperation(COMPOSITE_OP_COPY, COMPOSITE_OP_COPY);

    PushRectangle4TextureArea(-1, -1, 2, 2, 0, 0,
                              static_cast<float>(src.ExpectedWidth()) / src.Width(),
                              static_cast<float>(src.ExpectedHeight()) / src.Height(),
                              GColorWhite, GTransformIdentity);
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetTransform(GTransformIdentity);
    }

    glBindTexture(GL_TEXTURE_2D, src.mFboTexture.GetTextureID());
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;
}

void GCanvasContext::PrepareDrawElemetToFBO(GFrameBufferObject &fbo, float offsetX, float offsetY)
{
    int w = fbo.ExpectedWidth();
    int h = fbo.ExpectedHeight();

    glViewport(0, 0, w, h);
    mProjectTransform = CalculateProjectTransform(w, h);
    mProjectTransform = GTransformTranslate(mProjectTransform, offsetX, offsetY);

    mCurrentState->mShader->SetTransform(mProjectTransform);
    ResetTransform();
}

void GCanvasContext::DrawFBOToScreen(GFrameBufferObject &fbo, float x, float y, float w, float h,
                                     GColorRGBA color)
{
    SetTexture(fbo.mFboTexture.GetTextureID());
    PushRectangle4TextureArea(x, y, w, h, 0, 0,
                              static_cast<float>(fbo.ExpectedWidth()) / fbo.Width(),
                              static_cast<float>(fbo.ExpectedHeight()) / fbo.Height(),
                              color, GTransformIdentity, false);
}

void GCanvasContext::DoDrawBlur(const GRectf &rect, float blur, std::function<void()> draw,
                                GFrameBufferObjectPtr &inputFbo, GFrameBufferObjectPtr &outputFbo,
                                float scale)
{
    // const float Step = 5;
    // const float inverseStep = 0.2;

    GFrameBufferObjectPtr &tmpFbo = inputFbo;
    float scaleFactor = 1;
    // down sample blur (not effective, do not use it now)
    // float blurStep = blur * scale;
    //   float subBlur = (blur /= 2.5f);
    //    if ((subBlur * scale) > Step) {
    //        scaleFactor = inverseStep;
    //        DoDrawBlur(rect * inverseStep, subBlur, draw, inputFbo, tmpFbo, scale * inverseStep);
    //    }

    Save();

    UseBlurRenderPipeline(blur * scale);

    // horizontal blur
    float deltaH = scaleFactor / tmpFbo->Width();
    float deltaV = scaleFactor / tmpFbo->Height();

    auto shadowFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
    shadowFbo->BindFBO();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetDelta(deltaH, 0);
    }
    DrawFBOToFBO(*tmpFbo, *shadowFbo);
    shadowFbo->UnbindFBO();

    // vertical blur
    outputFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
    outputFbo->BindFBO();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetDelta(0, deltaV);
    }
    DrawFBOToFBO(*shadowFbo, *outputFbo);
    outputFbo->UnbindFBO();

    Restore();
}

void GCanvasContext::DrawBlur(const GRectf &rect, float blur, std::function<void()> draw,
                              std::vector<GPath *> *clipPath)
{
    GRectf shadowRect = rect;
    shadowRect.Enlarge(blur, blur);
    GFrameBufferObjectPtr shadowFbo, blurFbo;
    // draw content image to shadow fbo
    DoDrawShadowToFBO(shadowFbo, 1, shadowRect, draw);
    // draw blur on shadow fbo
    DoDrawBlur(shadowRect, blur, draw, shadowFbo, blurFbo, 1.0);
    // draw shadow to screen
    DoDrawShadowFBOToScreen(blurFbo, shadowRect, clipPath);
}

void GCanvasContext::DoDrawShadowToFBO(GFrameBufferObjectPtr &shadowFbo, float dpr, const GRectf &rect,
                                       std::function<void()> draw)
{
    // draw to fbo
    // LOG_E("DoDrawShadowToFBO %f, %f", rect.Width() * dpr, rect.Height() * dpr);
    shadowFbo = mFrameBufferPool.GetFrameBuffer(rect.Width() * dpr, rect.Height() * dpr);
    shadowFbo->BindFBO();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float origin_dpr = mDevicePixelRatio;
    SetDevicePixelRatio(dpr);

    Save();

    PrepareDrawElemetToFBO(*shadowFbo, -rect.leftTop.x, -rect.leftTop.y);
    draw();
    Restore();
    SetDevicePixelRatio(origin_dpr);
    shadowFbo->UnbindFBO();
}

void GCanvasContext::DoDrawShadowFBOToScreen(GFrameBufferObjectPtr &shadowFbo, const GRectf &rect,
                                             std::vector<GPath *> *clipPath)
{
    Save();
    UseShadowRenderPipeline();
    glViewport(0, 0, mWidth, mHeight);
    mProjectTransform = CalculateProjectTransform(mWidth, mHeight);
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetTransform(mProjectTransform);
    }

    // output shadow fbo to screen, we need restore clip first
    if (clipPath != nullptr && clipPath->size() > 0)
    {
        DrawClip();
    }

    DrawFBOToScreen(*shadowFbo, rect.leftTop.x + mCurrentState->mShadowOffsetX,
                    rect.leftTop.y + mCurrentState->mShadowOffsetY,
                    rect.Width(), rect.Height(), mCurrentState->mShadowColor);
    Restore();
}

void GCanvasContext::DrawShadow(const GRectf &rect, std::function<void()> drawFun, bool isStroke)
{
    SendVertexBufferToGPU();

    // get current clip path and move it locally, temp disable clip for shadow render
    std::vector<GPath *> savedClipPaths = (mCurrentState->clipPaths);
    bool hasClip = savedClipPaths.size() > 0;
    if (hasClip)
    {
        mCurrentState->clipPaths.clear();
        glDepthFunc(GL_GEQUAL);
    }

    if (mCurrentState->mShadowBlur < 0.01)
    {
        GFrameBufferObjectPtr shadowFbo;
        DoDrawShadowToFBO(shadowFbo, mDevicePixelRatio, rect, drawFun);
        DoDrawShadowFBOToScreen(shadowFbo, rect, &savedClipPaths);
    }
    else
    {
        DrawBlur(
            rect, mCurrentState->mShadowBlur, [=]() {
                drawFun();
            },
            &savedClipPaths);
    }

    // after restore, recover saved clip path
    if (hasClip)
    {
        mCurrentState->clipPaths = savedClipPaths;
        DrawClip();
        // restore depth EQUAL
        glDepthFunc(GL_EQUAL);
    }
}

bool GCanvasContext::NeedDrawShadow()
{
    if (mCurrentState->mGlobalCompositeOp == COMPOSITE_OP_COPY ||
        (mCurrentState->mGlobalCompositeOp == COMPOSITE_OP_DESTINATION_ATOP &&
         G_FLOAT_EQUAL(mCurrentState->mShadowOffsetX, 0) &&
         G_FLOAT_EQUAL(mCurrentState->mShadowOffsetY, 0) &&
         G_FLOAT_EQUAL(mCurrentState->mShadowBlur, 0)))
    {
        return false;
    }
    return mCurrentState->mShadowColor.rgba.a > 0.01;
}

float GCanvasContext::GetCurrentAlphaOfStyle(bool isStroke)
{
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style == nullptr || style->IsDefault() || style->IsPattern())
    {
        return isStroke ? mCurrentState->mStrokeColor.rgba.a : mCurrentState->mFillColor.rgba.a;
    }
    else
    {
        if (style->IsLinearGradient())
        {
            FillStyleLinearGradient *grad = (FillStyleLinearGradient *)(style);
            const FillStyleLinearGradient::ColorStop *stop = grad->GetColorStop(0);
            if (stop)
            {
                return stop->color.rgba.a;
            }
            else
            {
                return 1.0;
            }
        }
        else if (style->IsRadialGradient())
        {
            FillStyleRadialGradient *grad = (FillStyleRadialGradient *)(style);
            const FillStyleRadialGradient::ColorStop *stop = grad->GetColorStop(0);
            if (stop)
            {
                return stop->color.rgba.a;
            }
            else
            {
                return 1.0;
            }
        }
    }
    return 1;
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
    if (mSaveShader)
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
        else if (name == "BLUR")
        {
            UseBlurRenderPipeline();
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

inline void WeightCalculate(double radius, float *weight, int lastIndex)
{
    double sigma = radius * 0.5;
    for (int i = 0; i <= lastIndex; ++i)
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
    ShadowShader *shadowShader = (ShadowShader *)(mCurrentState->mShader);
    if (shadowShader)
    {
        GColorRGBA color = mCurrentState->mShadowColor;
        float alpha = color.rgba.a;
        color.rgba.r = color.rgba.r * alpha;
        color.rgba.g = color.rgba.g * alpha;
        color.rgba.b = color.rgba.b * alpha;
        shadowShader->SetShadowColor(color.components);
    }
}

void GCanvasContext::UseBlurRenderPipeline()
{
    GShader *newShader = FindShader("BLUR");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }
}

void GCanvasContext::UseBlurRenderPipeline(double radius)
{
    GShader *newShader = FindShader("BLUR");

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }

    int iradius = (int)radius;
    // max support radius
    if (iradius > MAX_BLUE_RADIUS)
    {
        iradius = MAX_BLUE_RADIUS;
    }

    int count = iradius + 1;
    float weight[count];
    WeightCalculate(iradius, weight, iradius);
    BlurShader *bs = (BlurShader *)newShader;
    bs->SetBlurRadius(iradius);
    bs->SetWeight(weight, count);
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
    if (style != nullptr && style->IsPattern() && mCurrentState->mShader)
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

    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr && style->IsLinearGradient() && mCurrentState->mShader)
    {
        FillStyleLinearGradient *grad = (FillStyleLinearGradient *)(style);
        GPoint startPos = GPointApplyGTransform(grad->GetStartPos(), mCurrentState->mTransform);
        GPoint endPos = GPointApplyGTransform(grad->GetEndPos(), mCurrentState->mTransform);

        mCurrentState->mShader->SetRange(startPos, endPos);
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
    if (style != nullptr && style->IsRadialGradient() && mCurrentState->mShader)
    {
        FillStyleRadialGradient *grad = (FillStyleRadialGradient *)(style);
        mCurrentState->mShader->SetRange(grad->GetStartPos(), grad->GetEndPos());
        mCurrentState->mShader->SetColorStopCount(grad->GetColorStopCount());

        //RadialGradient invertTransform
        RadialGradientShader *radialGradientShader = (RadialGradientShader *)(mCurrentState->mShader);
        if (radialGradientShader)
        {
            //Use invert transform in shader
            radialGradientShader->SetInvertTransform(GTransformInvert(mCurrentState->mTransform));
        }

        const int count = grad->GetColorStopCount();
        for (int i = 0; i < count; ++i)
        {
            const FillStyleRadialGradient::ColorStop *stop = grad->GetColorStop(i);
            GColorRGBA c = BlendColor(this, stop->color);
            mCurrentState->mShader->SetColorStop(c.components, stop->pos, i);
        }
    }
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
    FillText((const unsigned short *)text, (unsigned int)strLength, x, y, isStroke, scaleWidth);

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

float GCanvasContext::GlobalAlpha()
{
    return mCurrentState->mGlobalAlpha;
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
    //    UseDefaultRenderPipeline();
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
    //    UseDefaultRenderPipeline();
    //    if (mCurrentState->mShader != nullptr) {
    //        mCurrentState->mShader->SetHasTexture(0);
    //    }
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
                                                const float posArray[],
                                                const std::string colorArray[],
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
                                                const float posArray[],
                                                const std::string colorArray[],
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
    GLineCap lineCap = mCurrentState->mLineCap;
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
    GLineJoin lineJoin = mCurrentState->mLineJoin;
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
    LOG_D("ClearScreen: r:%f, g:%f, b:%f, a:%f", mClearColor.rgba.r, mClearColor.rgba.g,
          mClearColor.rgba.b,
          mClearColor.rgba.a);
    glClearColor(mClearColor.rgba.r, mClearColor.rgba.g, mClearColor.rgba.b, mClearColor.rgba.a);
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/**
 * 改变canvas 画布大小(目前支付宝内并未使用)
 * reset状态为默认，且更新OpenGL状态为默认
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
    GTransform t = GTransform(a, b, c, d, tx, ty);
    mPath.mTransform = mCurrentState->mTransform = t;
}

void GCanvasContext::Transfrom(float a, float b, float c, float d, float tx, float ty)
{
    GTransform t = GTransform(a, b, c, d, tx, ty);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);
    mPath.mTransform = mCurrentState->mTransform;
}

void GCanvasContext::ResetTransform()
{
    mPath.mTransform = mCurrentState->mTransform = GTransformIdentity;
}

void GCanvasContext::DoResetTransform()
{
    SetTransformOfShader(mProjectTransform);
}

void GCanvasContext::Scale(float sx, float sy)
{
    mCurrentState->mTransform = GTransformScale(mCurrentState->mTransform, sx, sy);
    mPath.mTransform = mCurrentState->mTransform;
}

void GCanvasContext::DoScale(float sx, float sy)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeScale(sx, sy));
}

void GCanvasContext::Rotate(float angle)
{
    if (fabs(angle) < FLT_EPSILON)
    {
        return;
    }

    mCurrentState->mTransform = GTransformRotate(mCurrentState->mTransform, angle);
    mPath.mTransform = mCurrentState->mTransform;
}

void GCanvasContext::DoRotate(float angle)
{
    mCurrentState->mTransform = GTransformRotate(mCurrentState->mTransform, angle);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::Translate(float tx, float ty)
{
    if (fabs(tx) < FLT_EPSILON && fabs(ty) < FLT_EPSILON)
    {
        return;
    }

    mCurrentState->mTransform = GTransformTranslate(mCurrentState->mTransform, tx, ty);
    mPath.mTransform = mCurrentState->mTransform;
}

void GCanvasContext::DoTranslate(float tx, float ty)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeTranslation(tx, ty));
}

void GCanvasContext::Save()
{
    // LOG_I("call save and the mstatestack size is %d\n", mStateStack.size());
    GCanvasState *newState = new GCanvasState(*mCurrentState);
    mStateStack.push_back(newState);
    mCurrentState = newState;
}

bool GCanvasContext::Restore()
{
    // FIXME by zhanqu
    // performance concern, avoid issue draw call
    // since previous draw commands are buffered, them should be draw before switching to other state
    SendVertexBufferToGPU();

    // last state, no need to do
    if (mStateStack.size() <= 1)
    {
        return false;
    }

    GShader *oldShader = mCurrentState->mShader;
    GCompositeOperation oldCompositeOp = mCurrentState->mGlobalCompositeOp;

    // reset clip status
    ResetClip();

    // free top state object
    GCanvasState *state = mStateStack.back();
    if (state != nullptr)
    {
        delete state;
    }

    // change to new state
    mStateStack.pop_back();
    mCurrentState = mStateStack.back();

    mPath.mTransform = mCurrentState->mTransform;

    auto op = mCurrentState->mGlobalCompositeOp;
    if (op != oldCompositeOp)
    {
        GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);
        glBlendFuncSeparate(funcs.source, funcs.destination,
                            funcs.source, funcs.destination);
    }

    if (mCurrentState->mShader && mCurrentState->mShader != oldShader)
    {
        mCurrentState->mShader->Bind();
    }

    if (mCurrentState->clipPaths.size() > 0)
    {
        DrawClip();
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
    mPath.Rect(x, y, w, h);
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
    //    float sx = mCurrentState->mTransform.a * GetWidth();
    //    float sy = mCurrentState->mTransform.d * GetHeight();
    //    float scale = sqrt(sx * sx + sy * sy);
    mPath.QuadraticCurveTo(cpx, cpy, x, y);
}

void GCanvasContext::BezierCurveTo(float cp1x, float cp1y, float cp2x,
                                   float cp2y, float x, float y)
{
    //    float scale = mCurrentState->mTransform.a * mCurrentState->mTransform.a +
    //                  mCurrentState->mTransform.d * mCurrentState->mTransform.d;
    //    scale = 1 / sqrtf(scale);
    mPath.BezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

//rect
void GCanvasContext::FillRect(float x, float y, float w, float h)
{
    ApplyFillStylePipeline();
    GColorRGBA color = BlendFillColor(this);
    if (NeedDrawShadow())
    {
        std::vector<GVertex> vec;
        PushRectangle(x, y, w, h, 0, 0, 0, 0, color, mCurrentState->mTransform, false, &vec);

        GRectf rect;
        GPath::GetRectCoverVertex(rect, vec);
        DrawShadow(rect, [&] {
            PushVertexs(vec);
        });
        PushVertexs(vec);
    }
    else
    {
        PushRectangle(x, y, w, h, 0, 0, 0, 0, color, mCurrentState->mTransform);
    }
}

void GCanvasContext::DoFillRect(float x, float y, float w, float h)
{
    GColorRGBA color = BlendFillColor(this);

    ApplyFillStylePipeline();
    PushRectangle(x, y, w, h, 0, 0, 0, 0, color, mCurrentState->mTransform);
}

void GCanvasContext::StrokeRect(float x, float y, float w, float h)
{
    if (NeedDrawShadow())
    {
        GPath tempPath;
        GenStrokeRectPath(tempPath, x, y, w, h);

        GColorRGBA color = BlendStrokeColor(this);
        std::vector<GVertex> vertexVec;
        tempPath.Stroke(this, color, &vertexVec);

        GRectf rect;
        GPath::GetRectCoverVertex(rect, vertexVec);
        DrawShadow(rect, [&] {
            mPath.DrawVertexToContext(this, vertexVec);
        });
        tempPath.DrawVertexToContext(this, vertexVec);
    }
    else
    {
        ApplyFillStylePipeline(true);

        GPath path;
        GenStrokeRectPath(path, x, y, w, h);
        path.Stroke(this);
    }
}

// for weex
void GCanvasContext::DoStrokeRect(float x, float y, float w, float h)
{
    GPath *tempPath = new GPath();

    tempPath->mTransform = mCurrentState->mTransform;
    tempPath->MoveTo(x, y);
    tempPath->LineTo(x + w, y);
    tempPath->LineTo(x + w, y + h);
    tempPath->LineTo(x, y + h);
    tempPath->Close();

    tempPath->Stroke(this);
    delete tempPath;
}

void GCanvasContext::GenStrokeRectPath(GPath &path, float x, float y, float w, float h)
{
    path.mTransform = mCurrentState->mTransform;
    path.MoveTo(x, y);
    path.LineTo(x + w, y);
    path.LineTo(x + w, y + h);
    path.LineTo(x, y + h);
    path.Close();
}

void GCanvasContext::ClearRect(float x, float y, float w, float h)
{
    UseDefaultRenderPipeline();

    GCompositeOperation oldOp = mCurrentState->mGlobalCompositeOp;
    //    SendVertexBufferToGPU();
    DoSetGlobalCompositeOperation(COMPOSITE_OP_COPY, COMPOSITE_OP_COPY);

    PushRectangle(x, y, w, h, 0, 0, 0, 0, mClearColor, mCurrentState->mTransform);

    DoSetGlobalCompositeOperation(oldOp, oldOp);
}

/**
 * spec:
 * The clip() method, when invoked, must create a new clipping region by calculating the intersection of
 * the current clipping region and the area described by the intended path, using the fill rule indicated by the fillRule argument.
 * Open subpaths must be implicitly closed when computing the clipping region, without affecting the actual subpaths.
 * The new clipping region replaces the current clipping region.
 */
void GCanvasContext::Clip(GFillRule rule)
{
    mPath.Close();
    GPath *newPath = new GPath(mPath);
    DoClipPath(newPath, rule);
}

void GCanvasContext::Clip(GPath2D &path2d, GFillRule rule)
{
    GPath *path = new GPath();
    path->mTransform = mPath.mTransform;
    path2d.WriteToPath(*path);
    path->Close();

    DoClipPath(path, rule);
}

/**
 * deprecated
 */
void GCanvasContext::ClipRegion()
{
    BeforeClip();

    mPath.Close();

    GPath *newPath = new GPath(mPath);
    DrawClipPath(newPath);

    mCurrentState->clipPaths.push_back(newPath);

    SetClipFlag(true);

    AfterClip();
}

//void GCanvasContext::ClipRegionNew(GFillRule rule) {
//    mPath.Close();
//
//    GPath* newPath = new GPath(mPath);
//    DoClipPath(*newPath, rule);
//}

void GCanvasContext::DoClipPath(GPath *path, GFillRule rule)
{
    path->mClipFillRule = rule;
    DrawClipPath(path);

    mCurrentState->clipPaths.push_back(path);
    SetClipFlag(true);
}

inline void GCanvasContext::DrawClip()
{
    for (int i = 0; i < mCurrentState->clipPaths.size(); i++)
    {
        DrawClipPath(mCurrentState->clipPaths[i]);
    }
}

inline void GCanvasContext::DrawClipPath(GPath *path)
{
    path->Fill(this, path->mClipFillRule, FILL_TARGET_DEPTH);
}

void GCanvasContext::ResetClip()
{
    if (mCurrentState->clipPaths.size() <= 0)
    {
        return;
    }

    mCurrentState->ClearClip();

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);

    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);

    SetClipFlag(false);
}

void GCanvasContext::BeforeClip()
{
    UseDefaultRenderPipeline();
    if (mCurrentState->mShader)
    {
        mCurrentState->mShader->SetTransform(mProjectTransform);
        mCurrentState->mShader->SetHasTexture(0);
    }
}

void GCanvasContext::AfterClip()
{
    RestoreRenderPipeline();
}

void GCanvasContext::Stroke()
{
    DoStrokeWithPath(mPath);
}

void GCanvasContext::Stroke(GPath2D &path2d)
{
    GPath path;
    path.mTransform = mPath.mTransform;
    path2d.WriteToPath(path);

    DoStrokeWithPath(path);
}

void GCanvasContext::Fill(GFillRule rule)
{
    DoFillWithPath(mPath, rule);
}

void GCanvasContext::Fill(GPath2D &path2d, GFillRule rule)
{
    GPath path;
    path.mTransform = mPath.mTransform;
    path2d.WriteToPath(path);

    DoFillWithPath(path, rule);
}

void GCanvasContext::DoFillWithPath(GPath &path, GFillRule rule)
{
    ApplyFillStylePipeline();
    if (NeedDrawShadow())
    {
        GRectf rect;
        path.GetRect(rect);
        DrawShadow(rect, [=]() {
            GPath &cpath = const_cast<GPath &>(path);
            cpath.Fill(this, rule, FILL_TARGET_COLOR, true);
        });
    }
    path.Fill(this, rule);
}

void GCanvasContext::DoStrokeWithPath(GPath &path)
{
    if (NeedDrawShadow())
    {
        GColorRGBA color = BlendStrokeColor(this);
        std::vector<GVertex> vertexVec;
        // FIXME
        // draw 2 times for get cover rect, maybe we can reduce 1 time
        path.Stroke(this, color, &vertexVec);

        GRectf rect;
        GPath::GetRectCoverVertex(rect, vertexVec);
        DrawShadow(rect, [&] {
            path.DrawVertexToContext(this, vertexVec);
        });
        path.DrawVertexToContext(this, vertexVec);
    }
    else
    {
        ApplyFillStylePipeline(true);
        path.Stroke(this);
    }
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
    if (text == nullptr)
    {
        return 0;
    }
    if (strLength == 0)
    {
        strLength = static_cast<int>(strlen(text));
    }
    if (mCurrentState->mFont == nullptr)
    {
        mCurrentState->mFont = new GFontStyle(nullptr, mDevicePixelRatio);
    }

    if (mFontManager != nullptr)
    {
        float width = mFontManager->MeasureText(text, strLength, mCurrentState->mFont);
        return width / mDevicePixelRatio;
    }
    else
    {
        return 0;
    }
}

//image
void GCanvasContext::DrawImage(int textureId, int textureWidth, int textureHeight,
                               float sx, float sy, float sw, float sh,
                               float dx, float dy, float dw, float dh,
                               bool flipY)
{
    if (textureId <= 0)
    {
        LOG_EXCEPTION(mHooks, mContextId, "texture invalid", "<function:%s> textureId=%i",
                      __FUNCTION__, textureId);
        return;
    }

    UseTextureRenderPipeline();
    // FIXME 缓存blended white color, 避免重复blend计算
    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(textureId);

    // if (NeedDrawShadow())
    // {
    //     std::vector<GVertex> vec;
    //     PushRectangle(dx, dy, dw, dh, sx / textureWidth, sy / textureHeight, sw / textureWidth,
    //                   sh / textureHeight, color, mCurrentState->mTransform, flipY, &vec);

    //     GRectf rect;
    //     GPath::GetRectCoverVertex(rect, vec);
    //     DrawShadow(rect, [&] {
    //         PushVertexs(vec);
    //     });
    //     PushVertexs(vec);
    // }
    // else
    // {
    PushRectangle(dx, dy, dw, dh, sx / textureWidth, sy / textureHeight, sw / textureWidth,
                  sh / textureHeight, color, mCurrentState->mTransform, flipY);
    // }
}

void GCanvasContext::DoDrawImage(float w, float h, int TextureId, float sx,
                                 float sy, float sw, float sh, float dx,
                                 float dy, float dw, float dh, bool flipY)
{
    if (TextureId <= 0)
    {
        LOG_EXCEPTION(mHooks, mContextId, "texture invalid", "<function:%s> textureId=%i",
                      __FUNCTION__, TextureId);
        return;
    }

    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(TextureId);
    PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color, mCurrentState->mTransform,
                  flipY);
}

void GCanvasContext::GetImageData(int x, int y, int width, int height, uint8_t *pixels)
{
    SendVertexBufferToGPU();

    std::vector<int> rawPixel;

    if (width < 0)
    {
        x += width;
        width = -width;
    }
    if (height < 0)
    {
        y += height;
        height = -height;
    }

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
        LOG_EXCEPTION(mHooks, mContextId, "glReadPixels_fail", "<function:%s, glGetError:%x>",
                      __FUNCTION__,
                      glerror);
    }
    // sample
    gcanvas::PixelsSampler(realWidth, realHeight, &rawPixel[0], width, height,
                           reinterpret_cast<int *>(pixels));
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

int GCanvasContext::BindImage(const unsigned char *rgbaData, GLint format, unsigned int width,
                              unsigned int height)
{
    std::vector<GCanvasLog> logVec;
    GLuint result = gcanvas::PixelsBindTexture(rgbaData, format, width, height, &logVec);
    LOG_EXCEPTION_VECTOR(mHooks, mContextId, logVec);
    return result;
}
