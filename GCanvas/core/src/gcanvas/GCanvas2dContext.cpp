/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <assert.h>

#include "GCanvas2dContext.h"
#include "GShaderManager.h"
#include "../GCanvas.hpp"

#ifdef IOS
#include <malloc/malloc.h>
#include <unordered_map>
#endif

#ifdef ANDROID

#include "GFontCache.h"

#endif

#ifdef GFONT_LOAD_BY_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include "GPoint.h"

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s)  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct
{
    int code;
    const char *message;
} FT_Errors[] =

#include FT_ERRORS_H

#endif


#define SIZE_EPSILON 1.f

using namespace gcanvas;

//static const int FontTextureWidth = 2048;
//static const int FontTextureHeight = 2048;

#define FontTextureWidth        2048
#define FontTextureHeight       2048


#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"

GBlendOperationFuncs GCompositeOperationFuncs(int index)
{
    static GBlendOperationFuncs funcs[] = {
        {GL_ONE,           GL_ONE_MINUS_SRC_ALPHA}, //0
        {GL_SRC_ALPHA,           GL_ONE}, //1
        {GL_DST_COLOR,           GL_ONE_MINUS_SRC_ALPHA}, //2
        {GL_ZERO,                GL_ONE_MINUS_SRC_ALPHA}, //3
        {GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA}, //4
        {GL_DST_ALPHA,           GL_ZERO}, //5
        {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}, //6
        {GL_ONE,                 GL_ZERO}, //7
        {GL_SRC_ALPHA,           GL_DST_ALPHA}, //8
        {GL_ONE,                 GL_ONE}, //9
        {GL_ONE_MINUS_DST_ALPHA, GL_ZERO},//10
        {GL_ZERO,                GL_SRC_ALPHA},//11
    };
    
    if( index < 0 || index > 11 ) index = 0;
    
    return funcs[index];
}

GColorRGBA BlendColor(GCanvasContext *context, GColorRGBA color )
{
    float alpha = context->GlobalAlpha() * color.rgba.a;
    GColorRGBA c;
    c.rgba.r = color.rgba.r * alpha;
    c.rgba.g = color.rgba.g * alpha;
    c.rgba.b = color.rgba.b * alpha;

    //TODO CompositeOperation
    c.rgba.a = alpha;
    return c;
}

GColorRGBA BlendWhiteColor(GCanvasContext *context)
{
    GColorRGBA c = {1,1,1,1};
    return BlendColor(context, c);
}

GColorRGBA BlendFillColor(GCanvasContext *context)
{
    return BlendColor(context, context->GetFillStyle());
}

GColorRGBA BlendStrokeColor(GCanvasContext *context)
{
    return BlendColor(context, context->StrokeStyle());
}

#ifdef ANDROID

bool GCanvasContext::LoadFace(FT_Library *library, const char *filename,
                              const float size, FT_Face *face)
{
    size_t hres = 64;
    FT_Matrix matrix = {(int) ((1.0 / hres) * 0x10000L), (int) ((0.0) * 0x10000L),
                        (int) ((0.0) * 0x10000L), (int) ((1.0) * 0x10000L)};

    assert(library);
    assert(filename);
    assert(size);

    FT_Error error = FT_Init_FreeType(library);
    if (error)
    {
        return false;
    }

    error = FT_New_Face(*library, filename, 0, face);
    if (error)
    {
        assert(filename == 0);
        FT_Done_FreeType(*library);
        return false;
    }

    error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
    if (error)
    {
        FT_Done_Face(*face);
        FT_Done_FreeType(*library);
        return false;
    }

    error = FT_Set_Char_Size(*face, (int) (size * 64), 0, (FT_UInt) 72 * hres, 72);
    if (error)
    {
        FT_Done_Face(*face);
        FT_Done_FreeType(*library);
        return false;
    }

    FT_Set_Transform(*face, &matrix, nullptr);

    return true;
}

bool GCanvasContext::IsGlyphExistedInFont(const wchar_t charCode,
                                          const float size,
                                          std::string filename)
{
    FT_Library library;
    FT_Face face;
    bool existed = true;

    if (!this->LoadFace(&library, filename.c_str(), size, &face))
    {
        return false;
    }

    FT_UInt glyphIndex = FT_Get_Char_Index(face, charCode);
    if (glyphIndex == 0)
    {
        existed = false;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return existed;
}


#endif


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
    Translate(x, y);
    Scale(1 / mDevicePixelRatio * scaleWidth, 1 / mDevicePixelRatio);
    mFontManager->DrawText(text, text_length, 0, 0, isStroke, mCurrentState->mFont);
    Restore();
    
    mCurrentState->mShader->SetOverideTextureColor(0);
}

GCanvasContext::GCanvasContext(short w, short h, bool flip, std::string appInfo, bool useFBO)
        : mIsFboSupported(true), mWidth(w), mHeight(h), mCurrentState(nullptr),
          mHasClipRegion(false), mHiQuality(false), mVertexBufferIndex(0),
          mSaveShader(nullptr),
          mClearColor(GColorTransparentWhite),
          mDevicePixelRatio(1.f),
          mContextType(0), //默认值设置为2D
          mFlip(flip),
          mAppInfo(appInfo),
          mUseFBO(useFBO)
{
    if (mWidth > 0 && mHeight > 0)
    {
        UpdateProjectTransform();
        InitFBO();
    }
    
    mFontManager = GFontManager::NewInstance(this);
#ifdef ANDROID
    mShaderManager = nullptr;
    mFontCache = new GFontCache(*mFontManager);
#endif
    mFontManager->SetFontCache(mFontCache);
}

void GCanvasContext::SetDevicePixelRatio(const float ratio)
{
    mDevicePixelRatio = ratio;
    UpdateProjectTransform();
}


void GCanvasContext::SetCanvasDimension(const int w, const int h, bool resetStatus)
{
    LOG_I("GCanvasContext.SetDimension:%i,%i", w, h);
    // 如果设置为0, 表示取当前view尺寸
    mCanvasWidth = w;
    mCanvasHeight = h;
    UpdateProjectTransform();

    if (mCurrentState != nullptr)
    {
        // LOG_W("SetCanvasDimension: change currentState");
        GTransform old = mCurrentState->mTransform;
        mCurrentState->mTransform = mProjectTransform;
        if (!GTransformEqualToTransform(old, mCurrentState->mTransform))
        {
            // LOG_W("SetCanvasDimension: SetTransformOfShader to: %p", &mProjectTransform);
            // 更新 shader transform
            SetTransformOfShader(mCurrentState->mTransform);
        }
    }

    // 恢复到默认状态并清除当前已渲染内容
    if (resetStatus)
    {
        // 恢复到初始状态
        mVertexBufferIndex = 0;
        ResetStateStack();
        SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        UseDefaultRenderPipeline();
        ClearScreen(mClearColor);
    }
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


int GCanvasContext::GetCanvasWidth()
{
    return mCanvasWidth <= 0 ? mWidth : mCanvasWidth;
}


int GCanvasContext::GetCanvasHeight()
{
    return mCanvasHeight <= 0 ? mHeight : mCanvasHeight;
}


float GCanvasContext::GetDevicePixelRatio()
{
    return mDevicePixelRatio;
}


bool GCanvasContext::IsUseFbo()
{
    return mUseFBO;
}

void GCanvasContext::UpdateProjectTransform()
{
    int w = GetCanvasWidth();
    int h = GetCanvasHeight();
    if (w <= 0 || h <= 0) {
        return;
    }

    mProjectTransform = CalculateProjectTransform(w, h);
}

GTransform GCanvasContext::CalculateProjectTransform(int w, int h)
{
    GTransform t = GTransformIdentity;
    if (mFlip)
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

void GCanvasContext::DeleteFBO()
{

}

void GCanvasContext::ClearScreen(const GColorRGBA &color)
{
    LOG_D("ClearScreen: r:%f, g:%f, b:%f, a:%f", color.rgba.r, color.rgba.g, color.rgba.b,
          color.rgba.a);
    glClearColor(color.rgba.r, color.rgba.g, color.rgba.b, color.rgba.a);
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void GCanvasContext::InitFBO()
{
    if (0 != mContextType) return;

    if (!mUseFBO)
    {
        return;
    }

    if (!mIsFboSupported)
    {
        return;
    }

    if (mFboMap.find(DefaultFboName) == mFboMap.end())
    {
    #if ANDROID
        mIsFboSupported = mFboMap[DefaultFboName].InitFBO(mWidth, mHeight,
                                                          GColorTransparent, mEnableFboMsaa, mAppInfo);
    #else
        mIsFboSupported = mFboMap[DefaultFboName].InitFBO(mWidth, mHeight,
                                                          mClearColor, false, mAppInfo);
    #endif
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

GTexture *GCanvasContext::GetFboTexture()
{
    return &mFboMap[DefaultFboName].mFboTexture;
}



GCanvasContext::~GCanvasContext()
{
    DeleteFBO();

#ifdef ANDROID
    delete mFontCache;
    delete mShaderManager;
    mShaderManager = nullptr;
#endif
    
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
    glDepthFunc(GL_ALWAYS);
    glActiveTexture(GL_TEXTURE0);

    glViewport(0, 0, mWidth, mHeight);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    UpdateProjectTransform();

    // 只有首次初始化才需要更新gl状态
    if (!isGLInited) {
        // 初始化绘制配置和状态, 不能重复执行
        ResetStateStack();
        SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        InitializeGLShader();

        isGLInited = true;
    }

    return true;
}


bool GCanvasContext::InitializeGLShader()
{
#ifdef ANDROID
    if (mShaderManager == nullptr)
    {
        mShaderManager = new GShaderManager();
    }
#endif

    UsePatternRenderPipeline();
    if (nullptr == mCurrentState->mShader) {
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

    if (!hasOldState)
    {
        SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
    }

    if (!hasOldState ||
        !GTransformEqualToTransform(oldTransfrom, mCurrentState->mTransform))
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }

    SetClipFlag(false);
    if (mCurrentState->mClipPath != nullptr)
    {
        mCurrentState->mClipPath->DrawPolygons2DToContextNew(this, mCurrentState->mClipPath->mFillRule, FILL_TARGET_DEPTH);
//        mCurrentState->mClipPath->ClipRegion(this);
        SetClipFlag(true);
    }
}

void GCanvasContext::BindVertexBuffer()
{
    BindPositionVertexBuffer();

    if (mCurrentState->mShader->GetTexcoordSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint) mCurrentState->mShader->GetTexcoordSlot());
        glVertexAttribPointer((GLuint) mCurrentState->mShader->GetTexcoordSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *) CanvasVertexBuffer) + 2);
    }
    if (mCurrentState->mShader->GetColorSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint) mCurrentState->mShader->GetColorSlot());
        glVertexAttribPointer((GLuint) mCurrentState->mShader->GetColorSlot(), 4,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              ((float *) CanvasVertexBuffer) + 4);
    }
}

GLuint GCanvasContext::PositionSlot()
{
    return (GLuint) mCurrentState->mShader->GetPositionSlot();
}

void GCanvasContext::BindPositionVertexBuffer()
{
    if (mCurrentState->mShader->GetPositionSlot() >= 0)
    {
        glEnableVertexAttribArray((GLuint) mCurrentState->mShader->GetPositionSlot());
        glVertexAttribPointer((GLuint) mCurrentState->mShader->GetPositionSlot(), 2,
                              GL_FLOAT, GL_FALSE, sizeof(GVertex),
                              CanvasVertexBuffer);
    }
}


void GCanvasContext::SetTexture(const GTexture *texture)
{
    if (texture)
    {
        SetTexture(texture->GetTextureID());

    }
    else
    {
        SetTexture(InvalidateTextureId);
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

    glDrawArrays(geometry_type, 0, mVertexBufferIndex);

    mVertexBufferIndex = 0;
}


void GCanvasContext::PushTriangle(GPoint v1, GPoint v2, GPoint v3,
                                  GColorRGBA color, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    if (vec) {
        // push到传入的vector中
        GVertex vertex[3];
        vb = vertex;
    }
    else {
        // 直接push到vertexBuffer
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

    if (vec) {
        // push到传入的vector中
        for (int i = 0; i < 3; i++) {
            vec->push_back(vb[i]);
        }
    }
    else {
        // push到vertexBuffer
        mVertexBufferIndex += 3;
    }
}

void GCanvasContext::PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                              GColorRGBA color, std::vector<GVertex> *vec)
{
    GVertex *vb = NULL;
    if (vec) {
        // push到传入的vector中
        GVertex vertex[6];
        vb = vertex;
    }
    else {
        // 直接push到vertexBuffer
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

    if (vec) {
        // push到传入的vector中
        for (int i = 0; i < 6; i++) {
            vec->push_back(vb[i]);
        }
    }
    else {
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
    for (int i = 0; i + 2 < (int) points.size(); i += 3)
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
    for (int i = 0; i + 2 < (int) vertexs.size(); i += 3)
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

/**
 * 推送triangle fan方式的多边形点
 */
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
        // 当前迭代下当前指针到buffer末尾的距离
        curCountToTail = pointSize - i;
        segmentSize = curCountToTail > segmentStride ? segmentStride : curCountToTail;

        // 避免将末尾N个点拆分
        if (mVertexBufferIndex + segmentStride >= (GCANVAS_VERTEX_BUFFER_SIZE - 1))
        {
            //LOG_D("SendVertexBufferToGPU, vertex count=%d, i=%d, segmentSize=%d", mVertexBufferIndex,
            //    i, segmentSize);

            // 在末尾加入head，形成独立的提交
            CanvasVertexBuffer[mVertexBufferIndex].pos = head;
            CanvasVertexBuffer[mVertexBufferIndex].uv = uv;
            CanvasVertexBuffer[mVertexBufferIndex++].color = color;

            SendVertexBufferToGPU(GL_TRIANGLE_FAN);

            // 新的一次提交 重新插入head
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


void GCanvasContext::Save()
{
    mStateStack.push_back(mStateStack.back());
    mCurrentState = &mStateStack.back();
}

bool GCanvasContext::Restore()
{
    // since previous draw commands
    // are buffered, them should be draw
    // before switching to other state
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

    auto op=mCurrentState->mGlobalCompositeOp;
    
    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);
    
    glBlendFuncSeparate(funcs.source, funcs.destination,
                        funcs.source, funcs.destination);

    SetClipFlag(false);
    if (mCurrentState->mClipPath)
    {
//        SetTransformOfShader(mCurrentState->mClipTransform);
//        mCurrentState->mClipPath->ClipRegion(this);
//        mCurrentState->mClipPath->DrawPolygons2DToContextNew(this, mCurrentState->mClipPath->mFillRule);
        mCurrentState->mClipPath->DrawPolygons2DToContextNew(this, mCurrentState->mClipPath->mFillRule, FILL_TARGET_DEPTH);
        SetClipFlag(true);
    }

    if (mCurrentState->mShader != oldShader)
    {
        mCurrentState->mShader->Bind();
    }

    return true;
}

void GCanvasContext::Scale(float sx, float sy)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeScale(sx, sy));
}

void GCanvasContext::Rotate(float angle)
{
    mCurrentState->mTransform =
            GTransformRotate(mCurrentState->mTransform, angle);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::Translate(float tx, float ty)
{
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeTranslation(tx, ty));
}

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
#ifdef IOS
        if( nullptr == GShaderManager::getSingleton()->programForKey(name) )
#endif

#ifdef ANDROID
        if (nullptr == mShaderManager->programForKey(name))
#endif
        {
            LOG_I("SetTransformOfShader program for key is null");
            return;
        }

        glUniformMatrix4fv(shader->GetTransformSlot(), 1, GL_FALSE, &(m[0]));
    }
}

void GCanvasContext::ResetTransform()
{
    SetTransformOfShader(mProjectTransform);
}

void GCanvasContext::SetGlobalAlpha(float a)
{
    SendVertexBufferToGPU();
    mCurrentState->mGlobalAlpha = a;
}

GCompositeOperation GCanvasContext::GlobalCompositeOperation()
{
    return mCurrentState->mGlobalCompositeOp;
}

void
GCanvasContext::SetGlobalCompositeOperation(GCompositeOperation op, GCompositeOperation alphaOp)
{
    if (mCurrentState->mGlobalCompositeOp == op)
    {
        return;
    }
    SendVertexBufferToGPU();
    
    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);
    
#ifdef ANDROID
    GBlendOperationFuncs alphaFuncs = GCompositeOperationFuncs(alphaOp);

    glBlendFuncSeparate(funcs.source, funcs.destination,
                        alphaFuncs.source, alphaFuncs.destination);
#endif

#ifdef IOS
    glBlendFunc(funcs.source, funcs.destination);
#endif
    mCurrentState->mGlobalCompositeOp = op;
}

void
GCanvasContext::SetGlobalCompositeOperationSeprate(GCompositeOperation op, GCompositeOperation alphaOp)
{
    if (mCurrentState->mGlobalCompositeOp == op)
    {
        return;
    }
    SendVertexBufferToGPU();
    
    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);
    GBlendOperationFuncs alphaFuncs = GCompositeOperationFuncs(alphaOp);
    
    glBlendFuncSeparate(funcs.source, funcs.destination,
                        alphaFuncs.source, alphaFuncs.destination);
    mCurrentState->mGlobalCompositeOp = op;
}



void GCanvasContext::ClearRect(float x, float y, float w, float h)
{
    if( x == 0 && y == 0  && SIZE_EPSILON>fabs(w * GetCanvasDimensionWidthScale() - mWidth) &&
            SIZE_EPSILON>fabs(h * GetCanvasDimensionHeightScale() - mHeight) ){
        ClearScreen(mClearColor);
        return;
    }
    
    UseDefaultRenderPipeline();

    GCompositeOperation oldOp = mCurrentState->mGlobalCompositeOp;
    SendVertexBufferToGPU();
    SetGlobalCompositeOperation(COMPOSITE_OP_REPLACE, COMPOSITE_OP_REPLACE);

    PushRectangle(x, y, w, h, 0, 0, 0, 0, mClearColor);

    SetGlobalCompositeOperation(oldOp, oldOp);
}

void GCanvasContext::DrawFBOToFBO(GFrameBufferObject &src, GFrameBufferObject &dest)
{
    glViewport(0, 0, dest.ExpectedWidth(), dest.ExpectedHeight());
    
    SetGlobalCompositeOperation(COMPOSITE_OP_REPLACE, COMPOSITE_OP_REPLACE);
    
    PushRectangle4TextureArea(-1, -1, 2, 2, 0, 0,
                              static_cast<float >(src.ExpectedWidth()) / src.Width(),
                              static_cast<float >(src.ExpectedHeight()) / src.Height(),
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
    //    t = GTransformIdentity;
    //    t = GTransformScale(t, 2.f * mDevicePixelRatio  / fbo.ExpectedWidth(),
    //                        2.f * mDevicePixelRatio / fbo.ExpectedHeight());
    //    t = GTransformTranslate(t, -1.f, -1.f);
}

void GCanvasContext::DrawFBOToScreen(GFrameBufferObject &fbo, float x, float y, float w, float h,
                                     GColorRGBA color)
{
    SetTexture(fbo.mFboTexture.GetTextureID());
    PushRectangle4TextureArea(x, y, w, h, 0, 0,
                              static_cast<float >(fbo.ExpectedWidth()) / fbo.Width(),
                              static_cast<float >(fbo.ExpectedHeight()) / fbo.Height(),
                              color, true);
    
}

void GCanvasContext::Blur(const GRectf &rect, float blur,
                std::function<void()> draw, GFrameBufferObjectPtr &outputFbo, bool isOnScreen, float scale)
{
    const float Step = 5;
    blur /= 2.5f;
    
    GFrameBufferObjectPtr originFbo;
    float blurStep;
    float scaleFactor = 1;
    if (blur * scale > Step)
    {
        Blur(rect/ Step, blur, draw, originFbo, false, scale / Step);
        blurStep = Step;
        scaleFactor = 1 / Step;
    }
    else
    {
        // 画上原始图元
        originFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
        originFbo->BindFBO();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Save();
        PrepareDrawElemetToFBO(*originFbo);
        UseDefaultRenderPipeline();
        Translate(-rect.leftTop.x, -rect.leftTop.y);
        Scale(scale, scale);
        draw();
        Restore();
        originFbo->UnbindFBO();
        
        blurStep = blur * scale;
    }
    
    Save();
    UseShadowRenderPipeline(blurStep * 2.5);
    
    // 水平blur
    auto shadowFbo = mFrameBufferPool.GetFrameBuffer(rect.Width(), rect.Height());
    shadowFbo->BindFBO();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mCurrentState->mShader->SetDelta(scaleFactor / originFbo->Width(), 0);
    DrawFBOToFBO(*originFbo, *shadowFbo);
    shadowFbo->UnbindFBO();
    
    // 垂直blur
    if (isOnScreen) {
        // 渲染到在屏
        SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);
        glViewport(0, 0, mWidth, mHeight);
        mCurrentState->mShader->SetDelta(0, 1.0f / shadowFbo->Height());
        DrawFBOToScreen(*shadowFbo, rect.leftTop.x + mCurrentState->mShadowOffsetX,
                        rect.leftTop.y + mCurrentState->mShadowOffsetY,
                        rect.Width(), rect.Height(), mCurrentState->mShadowColor);
    }
    else {
        // 渲染到离屏
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
            // 无需模糊，直接绘制
            Save();
            UseDefaultRenderPipeline();
            Translate(mCurrentState->mShadowOffsetX, mCurrentState->mShadowOffsetY);
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
            FillRect(x, y, w, h);
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
            StrokeRect(x, y, w, h);
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
        if (mCurrentState->mShadowBlur < 0.01) {
            // 强制走blur情况下，fbo拷贝那一套
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
//            mPath.DrawPolygons2DToContext(this);
            mPath.DrawPolygons2DToContextNew(this, mPath.mFillRule);
        });
    }
}

void GCanvasContext::ApplyFillStylePipeline(bool isStroke)
{
    GFillStyle *style =  isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if ( style == nullptr || style->IsDefault() )
    {
        UseDefaultRenderPipeline();
    }
    else
    {
        if( style->IsPattern() )
        {
            UsePatternRenderPipeline(isStroke);
        }
        else if( style->IsLinearGradient() )
        {
            UseLinearGradientPipeline(isStroke);
        }
        else if( style->IsRadialGradient() )
        {
            UseRadialGradientPipeline(isStroke);
        }
    }
}

void GCanvasContext::FillRect(float x, float y, float w, float h)
{
    GColorRGBA color = BlendFillColor(this);
    ApplyFillStylePipeline();
    PushRectangle(x, y, w, h, 0, 0, 0, 0, color);
}

void GCanvasContext::StrokeRect(float x, float y, float w, float h)
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

void GCanvasContext::BeginPath() { mPath.Reset(); }

void GCanvasContext::Fill(GFillRule rule)
{
//    mPath.Close();
    FillBlur();
    ApplyFillStylePipeline();
//    mPath.DrawPolygons2DToContext(this);
    mPath.DrawPolygons2DToContextNew(this, rule);
}

void GCanvasContext::Stroke()
{
    ApplyFillStylePipeline(true);
    mPath.DrawLinesToContext(this);
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
    mCurrentState->mClipPath->DrawPolygons2DToContextNew(this, rule, FILL_TARGET_DEPTH);
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

void GCanvasContext::DrawCanvas(float w, float h, int textureId, float sx,
                                float sy, float sw, float sh, float dx,
                                float dy, float dw, float dh)
{
    GColorRGBA color;
    color.rgba.r = 1.0f;
    color.rgba.g = 1.0f;
    color.rgba.b = 1.0f;
    color.rgba.a = 1.0f;

    mCurrentState->mShader->SetHasTexture(1);
    glBindTexture(GL_TEXTURE_2D, textureId);
    PushReverseRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color);

    SendVertexBufferToGPU();
}


/**
 * drawImage实现
 * sx, sw 支持为负值，sw, sh必须为正数
 */
void GCanvasContext::DrawImage1(float w, float h, int TextureId, float sx,
                                float sy, float sw, float sh, float dx,
                                float dy, float dw, float dh, bool flipY)
{
    if (TextureId <= 0) {
        LOG_EXCEPTION("", "texture invalid", "<function:%s>", __FUNCTION__);
    }

    GColorRGBA color = BlendWhiteColor(this);
    SetTexture(TextureId);
    PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color, flipY);
}

void GCanvasContext::ClosePath() { mPath.Close(); }

void GCanvasContext::MoveTo(float x, float y) { mPath.MoveTo(x, y); }

void GCanvasContext::LineTo(float x, float y) { mPath.LineTo(x, y); }

void GCanvasContext::QuadraticCurveTo(float cpx, float cpy, float x, float y)
{
    float sx = mCurrentState->mTransform.a * Width();
    float sy = mCurrentState->mTransform.d * Height();
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

void GCanvasContext::ArcTo(float x1, float y1, float x2, float y2,
                           float radius)
{
    mPath.ArcTo(x1, y1, x2, y2, radius);
}

void GCanvasContext::ArcTo(float x1, float y1, float x2, float y2,
                           float radiusX, float radiusY, float rotation)
{
}

void GCanvasContext::Rect(float x, float y, float w, float h)
{
    mPath.MoveTo(x, y);
    mPath.LineTo(x + w, y);
    mPath.LineTo(x + w, y + h);
    mPath.LineTo(x, y + h);
    mPath.Close();
}

void GCanvasContext::Arc(float x, float y, float radius, float startAngle,
                         float endAngle, bool anticlockwise)
{
    mPath.Arc(x, y, radius, startAngle, endAngle, anticlockwise);
}

void GCanvasContext::SetFillStyle(GColorRGBA c)
{
    if ( mCurrentState->mFillStyle != nullptr )
    {
        delete mCurrentState->mFillStyle;
        mCurrentState->mFillStyle = nullptr;
    }
    
    mCurrentState->mFillColor = c;
    UseDefaultRenderPipeline();
}

void GCanvasContext::SetClearColor(const GColorRGBA &c)
{
    mClearColor = c;
}

void GCanvasContext::SetStrokeStyle(const GColorRGBA& c)
{
    if ( mCurrentState->mStrokeStyle != nullptr )
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

void GCanvasContext::SaveRenderPipeline()
{
    mSaveShader = mCurrentState->mShader;
    mSaveIsStroke = mCurrentState->mStrokeStyle != nullptr;
}

void GCanvasContext::RestoreRenderPipeline()
{
    std::string name = mSaveShader->GetName();
    
    if( name == "PATTERN" )
    {
        UsePatternRenderPipeline(mSaveIsStroke);
    }
    else if( name == "TEXTURE" )
    {
        UseTextureRenderPipeline();
    }
    else if( name == "SHADOW" )
    {
        UseShadowRenderPipeline();
    }
    else if( name == "LINEAR" )
    {
        UseLinearGradientPipeline(mSaveIsStroke);
    }
    else if( name == "RADIAL" )
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
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("DEFAULT");
#endif
    
#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("DEFAULT");
#endif
    
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
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("TEXTURE");
#endif

#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("TEXTURE");
#endif

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
        TextureShader *textureShader = dynamic_cast< TextureShader* >(mCurrentState->mShader);
        if( textureShader != nullptr )
        {
        #ifdef ANDROID
            textureShader->SetPremultipliedAlpha(false);
        #endif
        #ifdef IOS
            textureShader->SetPremultipliedAlpha(true);
        #endif
        }
    }
}

inline double Gaussian(double x, double sigma)
{
    return 1/exp(x*x/2/(sigma*sigma))/sigma/sqrt(2*M_PI);
}

inline void WeightCalculate(double radius, float weight[13])
{
    double sigma=radius/2.5;
    for (int i=0; i<=12; ++i) {
        weight[i]=Gaussian(i, sigma);
    }
}

void GCanvasContext::UseShadowRenderPipeline()
{
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("SHADOW");
#endif

#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("SHADOW");
#endif

    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();
    }
}

void GCanvasContext::UseShadowRenderPipeline(double radius)
{
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("SHADOW");
#endif

#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("SHADOW");
#endif

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
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif
    
#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("PATTERN");
#endif
    
    if (newShader != nullptr && mCurrentState->mShader != newShader)
    {
        SendVertexBufferToGPU();
        mCurrentState->mShader = newShader;
        mCurrentState->mShader->Bind();

        //Pattern
        GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
        if ( style != nullptr && style->IsPattern() )
        {
            FillStylePattern *pattern = dynamic_cast<FillStylePattern*>(style);
            if (pattern)
            {
                mCurrentState->mShader->SetRepeatMode(pattern->GetPattern());
                mCurrentState->mTextureId = pattern->GetTextureListID();
                mCurrentState->mShader->SetTextureSize(pattern->GetTextureWidth(), pattern->GetTextureHeight());
                //Pattern Alpha
                PatternShader *patternShader = dynamic_cast<PatternShader*>(mCurrentState->mShader);
                if (patternShader)
                {
                    patternShader->SetPatternAlpha(GlobalAlpha());
                }
            }
        }
    }
}

void GCanvasContext::UseLinearGradientPipeline(bool isStroke)
{
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("LINEAR");
#endif
    
#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("LINEAR");
#endif

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
        FillStyleLinearGradient *grad = dynamic_cast< FillStyleLinearGradient * >(style);
        if (grad) {
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
}

void GCanvasContext::UseRadialGradientPipeline(bool isStroke)
{
#ifdef IOS
    GShader* newShader = GShaderManager::getSingleton()->programForKey("RADIAL");
#endif
    
#ifdef ANDROID
    GShader *newShader = mShaderManager->programForKey("RADIAL");
#endif
    
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
        FillStyleRadialGradient *grad = dynamic_cast< FillStyleRadialGradient * >(style);
        if (grad) {
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
}


#ifdef ANDROID

std::string GCanvas::exe2dSyncCmd(int cmd, const char *&args)
{
    return "";
}

#endif

GTexture *GCanvasContext::GetFontTexture()
{
    if (mFontTexture)
    {
        return mFontTexture;
    }
    else
    {
        mFontTexture = new GTexture(FontTextureWidth, FontTextureHeight, GL_ALPHA, nullptr);
    }

    return mFontTexture;
}
