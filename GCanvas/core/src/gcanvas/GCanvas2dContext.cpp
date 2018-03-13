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

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H
#endif

    using namespace gcanvas;

#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"
static GVertex
    CanvasVertexBuffer[GCanvasContext::GCANVAS_VERTEX_BUFFER_SIZE];

GCanvasState::GCanvasState()
{
    mGlobalCompositeOp = COMPOSITE_OP_SOURCE_OVER;
    mFillColor.rgba = {0.0f, 0.0f, 0.0f, 1.0f};
    mStrokeColor.rgba = {0.0f, 0.0f, 0.0f, 1.0f};
    mGlobalAlpha = 1.0;
    mLineWidth = 1;
    mLineCap = LINE_CAP_BUTT;
    mLineJoin = LINE_JOIN_MITER;
    mMiterLimit = 10;

#ifdef ANDROID
    mFont = nullptr; // delay-load: "10px sans-serif"
#endif

    mTextAlign = TEXT_ALIGN_START;
    mTextBaseline = TEXT_BASELINE_ALPHABETIC;
    mTransform = GTransformIdentity;
    mClipTransform = GTransformIdentity;
    mClipPath = nullptr;
    mShader = nullptr;
    mFillStyle = nullptr;
}

GCanvasState::GCanvasState(const GCanvasState &state)
{
    mGlobalCompositeOp = state.mGlobalCompositeOp;
    mFillColor.rgba = state.mFillColor.rgba;
    mStrokeColor.rgba = state.mStrokeColor.rgba;
    mGlobalAlpha = state.mGlobalAlpha;
    mLineWidth = state.mLineWidth;
    mLineCap = state.mLineCap;
    mLineJoin = state.mLineJoin;
    mMiterLimit = state.mMiterLimit;
    mShader = state.mShader;

    mFillStyle = nullptr;
    if (state.mFillStyle != nullptr)
    {
        mFillStyle = state.mFillStyle->Clone();
    }

#ifdef ANDROID
    if (state.mFont != nullptr)
    {
        mFont = new GFontStyle(*state.mFont);
    }
    else
    {
        mFont = nullptr;
    }
#endif

#ifdef IOS
    mFontStyle = state.mFontStyle;
#endif

    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;

    mClipTransform = state.mClipTransform;
    if (state.mClipPath != nullptr)
    {
        mClipPath = new GPath(*(state.mClipPath));
    }
    else
    {
        mClipPath = nullptr;
    }
}

GCanvasState &GCanvasState::operator=(const GCanvasState &state)
{
    mGlobalCompositeOp = state.mGlobalCompositeOp;
    mFillColor.rgba = state.mFillColor.rgba;
    mStrokeColor.rgba = state.mStrokeColor.rgba;
    mGlobalAlpha = state.mGlobalAlpha;
    mLineWidth = state.mLineWidth;
    mLineCap = state.mLineCap;
    mLineJoin = state.mLineJoin;
    mMiterLimit = state.mMiterLimit;
    mShader = state.mShader;

    mFillStyle = nullptr;
    if (state.mFillStyle != nullptr)
    {
        mFillStyle = state.mFillStyle->Clone();
    }
#ifdef ANDROID
    if (mFont != nullptr)
    {
        delete mFont;
        mFont = nullptr;
    }

    if (state.mFont != nullptr)
    {
        mFont = new GFontStyle(*state.mFont);
    }
#endif

#ifdef IOS
    mFontStyle = state.mFontStyle;
#endif

    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;

    mClipTransform = state.mClipTransform;
    if (mClipPath != nullptr)
    {
        delete mClipPath;
        mClipPath = nullptr;
    }

    if (state.mClipPath != nullptr)
    {
        mClipPath = new GPath(*(state.mClipPath));
    }
    return *this;
}

GCanvasState::~GCanvasState()
{
    if (mClipPath != nullptr)
    {
        delete mClipPath;
    }
#ifdef ANDROID
    if (mFont != nullptr)
    {
        delete mFont;
        mFont = nullptr;
    }
#endif
    if (mFillStyle != nullptr)
    {
        delete mFillStyle;
    }
}

#ifdef ANDROID
bool GCanvasContext::LoadFace(FT_Library *library, const char *filename,
                               const float size, FT_Face *face)
{
    size_t hres = 64;
    FT_Matrix matrix = {(int)((1.0 / hres) * 0x10000L), (int)((0.0) * 0x10000L),
                        (int)((0.0) * 0x10000L), (int)((1.0) * 0x10000L)};

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

    error = FT_Set_Char_Size(*face, (int)(size * 64), 0, (FT_UInt)72 * hres, 72);
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

char *GCanvasContext::GetDefaultFont()
{
    return SystemFontInformation::GetSystemFontInformation()
        ->GetDefaultFontFile();
}

//char *GCanvasContext::GetDefaultFallbackFont()
//{
//    return "DroidSansFallback.ttf";
//}

char *GCanvasContext::TrySpecFont(const wchar_t charCode, const float size,
                                   const char *currentFontLocation,
                                   const char *specFontFile)
{
    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += specFontFile;

    bool exist = this->IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    if (exist)
    {
        return (char *)specFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GCanvasContext::TryDefaultFont(const wchar_t charCode, const float size,
                                      const char *currentFontLocation)
{
    auto defaultFontFile =
        SystemFontInformation::GetSystemFontInformation()->GetDefaultFontFile();
    if (defaultFontFile == nullptr)
    {
        return nullptr;
    }

    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += defaultFontFile;

    bool exist = this->IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    if (exist)
    {
        return defaultFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GCanvasContext::TryDefaultFallbackFont(const wchar_t charCode,
                                              const float size,
                                              const char *currentFontLocation)
{
    auto defaultFontFile = "DroidSansFallback.ttf";

    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += defaultFontFile;

    bool exist = this->IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    if (exist)
    {
        return (char *)defaultFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GCanvasContext::TryOtherFallbackFont(const wchar_t charCode,
                                            const float size,
                                            const char *currentFontLocation)
{
    return SystemFontInformation::GetSystemFontInformation()
        ->GetClosestFontFamily(this, currentFontLocation, charCode, size,
                               *mCurrentState->mFont);
}

GFont *GCanvasContext::GetFontByCharCode(wchar_t charCode)
{
    if (mCurrentState->mFont == nullptr)
    {
        mCurrentState->mFont = new GFontStyle();
    }

    float devicePixelRatio = 1;
    if (mHiQuality) devicePixelRatio = this->mDevicePixelRatio;

    float size = mCurrentState->mFont->GetSize() * devicePixelRatio;

    const char *defaultSystemFontLocation = "/system/fonts/";

    auto systemFontLocation = SystemFontInformation::GetSystemFontInformation()
            ->GetSystemFontLocation();
    const char *currentFontLocation = (systemFontLocation != nullptr)
                                            ? systemFontLocation
                                            : defaultSystemFontLocation;

    const char *currentFontFile = nullptr;
    auto fontFamily = SystemFontInformation::GetSystemFontInformation()->FindFontFamily(
            mCurrentState->mFont->GetFamily().c_str());

    if (fontFamily != nullptr)
    {
        currentFontFile = fontFamily->MatchFamilyStyle(*mCurrentState->mFont);
    }

    if (nullptr != currentFontFile)
    {
        currentFontFile = TrySpecFont(charCode, size, currentFontLocation,
                                        currentFontFile);
    }

    if (currentFontFile == nullptr)
    {
        currentFontFile = TryDefaultFont(charCode, size, currentFontLocation);
        if (nullptr == currentFontFile)
        {
            currentFontFile = TryDefaultFallbackFont(charCode, size, currentFontLocation);
            if (nullptr == currentFontFile)
            {
                currentFontFile = TryOtherFallbackFont(charCode, size, currentFontLocation);
            }
        }
    }

    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += currentFontFile;

    GFont *font = GFontCache::sharedInstance()->GetOrCreateFont((reinterpret_cast<GCanvas *>(this))->mContextId,
            fontFileFullPath, size);

    return font;
}

void GCanvasContext::FillTextInternal(GFont *font, wchar_t charcode,
                                       float &x, float y)
{
    mCurrentState->mShader->SetOverideTextureColor(1);

    font->DrawText(charcode, this, x, y, GetFillStyle());
}
#endif

void GCanvasContext::FillText(const unsigned short *text,
                               unsigned int text_length, float x, float y,
                               bool isStroke)
{
    if (text == nullptr || text_length == 0)
    {
        return;
    }

#ifdef ANDROID
    std::vector< GFont * > fonts;

    for (unsigned int i = 0; i < text_length; ++i)
    {
        fonts.push_back(this->GetFontByCharCode(text[i]));
    }

    this->adjustTextPenPoint(fonts, text, text_length, x, y);

    for (unsigned int i = 0; i < text_length; ++i)
    {
        this->FillTextInternal(fonts[i], text[i], x, y);
    }

    SendVertexBufferToGPU();
    mCurrentState->mShader->SetOverideTextureColor(0);

#endif

#ifdef IOS
    if (g2d_font_draw_text_func_)
    {
        g2d_font_draw_text_func_(text, text_length, x, y, isStroke, this);
    }
#endif
}

// <description>
// Adjusts the pen point per the current text style.
// </description>
// <remarks>
// Note:
//     1. We don't support several TextBaseLine styles because it might not be
//     so important.
//     2. We leave the glyph performance consideration to font cache and glyph
//     cache, i.e. we don't
//        deal with them in this function because we assume that it is corner
//        case that textAlign
//        & textBaseLine are called together for the same text.
//     3. We assume that we support only horizontal text for now.
// </remarks>
#ifdef ANDROID
void GCanvasContext::adjustTextPenPoint(std::vector< GFont * > font,
                                         const unsigned short *text,
                                         unsigned int textLength,
                                         /*out*/ float &x,
                                         /*out*/ float &y)
{
    if (mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_START &&
        mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_LEFT)
    {
        auto left_x = x;
        auto delta_x = 0.0f;
        for (unsigned int i = 0; i < textLength; ++i)
        {
            auto glyph = font[i]->GetGlyph(text[i]);

            if (glyph != nullptr)
            {
                delta_x += glyph->advanceX;
            }
        }

        if (mCurrentState->mTextAlign == GTextAlign::TEXT_ALIGN_CENTER)
        {
            x = left_x - delta_x / 2.0f;
        }
        else // textAlign is "Right" or "End"
        {
            x = left_x - delta_x;
        }
    }

    // Set the new start y of the pen point.
    if (mCurrentState->mTextBaseline !=
        GTextBaseline::TEXT_BASELINE_ALPHABETIC)
    {
        // Pre-process the not-supported styles.
        if (mCurrentState->mTextBaseline ==
            GTextBaseline::TEXT_BASELINE_HANGING)
        {
            // NOTE: Not supported - make it Top
            mCurrentState->mTextBaseline = GTextBaseline::TEXT_BASELINE_TOP;
        }
        else if (mCurrentState->mTextBaseline ==
                 GTextBaseline::TEXT_BASELINE_IDEOGRAPHIC)
        {
            // NOTE: Not supported - make it Alphabetic
            mCurrentState->mTextBaseline =
                GTextBaseline::TEXT_BASELINE_ALPHABETIC;
            return;
        }

        auto font_metrics = font[0]->GetMetrics();
        // We only deal with scalable fonts. Otherwise, we just simply skip it.
        if (font_metrics->unitsPerEM != 0)
        {
            auto ascender = font_metrics->ascender;
            auto descender = font_metrics->descender;
            if (mCurrentState->mTextBaseline ==
                GTextBaseline::TEXT_BASELINE_TOP)
            {
                y += fabs(ascender);
            }
            else if (mCurrentState->mTextBaseline ==
                     GTextBaseline::TEXT_BASELINE_BOTTOM)
            {
                y -= fabs(descender);
            }
            else if (mCurrentState->mTextBaseline ==
                     GTextBaseline::TEXT_BASELINE_MIDDLE)
            {
                y += (fabs(ascender) - fabs(descender)) / 2.0f;
            }
            else
            {
                LOG_W("Class: GCanvasContext  Method: adjustTextPenPoint  "
                          "Error: not handled case.");
            }
        }
    }
}
#endif

GCanvasContext::GCanvasContext(short w, short h)
    : mIsFboSupported(true), mWidth(w), mHeight(h), mCurrentState(nullptr),
      mHasClipRegion(false), mHiQuality(false), mVertexBufferIndex(0),
      mSaveShader(nullptr), mFboTexture(), mFboFrame(0), mFboStencil(0), mSaveFboFrame(0),
      mClearColor(DEFAULT_CLEAR_COLOR), mDevicePixelRatio(1.f), mContextType(0) //默认值设置为2D
{
    CalculateProjectTransform();

    if (mWidth > 0 && mHeight > 0)
    {
        InitFBO();
    }
#ifdef ANDROID
    mShaderManager = nullptr;
#endif
}

void GCanvasContext::SetDevicePixelRatio(const float ratio)
{
    mDevicePixelRatio = ratio;
    CalculateProjectTransform();
}

void GCanvasContext::CalculateProjectTransform()
{
    GTransform t = GTransformIdentity;
    t = GTransformScale(t, 2.f * mDevicePixelRatio / mWidth,
                        -2.f * mDevicePixelRatio / mHeight);
    t = GTransformTranslate(t, -1.f, 1.f);

    mProjectTransform = t;
}

void GCanvasContext::DeleteFBO()
{
    if (mFboFrame)
    {
        glDeleteFramebuffers(1, &mFboFrame);
        mFboFrame = 0;
    }

    if (mFboStencil)
    {
        glDeleteRenderbuffers(1, &mFboStencil);
        mFboStencil = 0;
    }
    
    GLuint textureId = mFboTexture.GetTextureID();
    glDeleteTextures(1, &textureId);
    mFboTexture.Unbind();
}

void GCanvasContext::ClearScreen(const GColorRGBA &color) {
    LOG_D("ClearScreen: r:%f, g:%f, b:%f, a:%f", color.rgba.r, color.rgba.g, color.rgba.b,
              color.rgba.a);
    glClearColor(color.rgba.r, color.rgba.g, color.rgba.b, color.rgba.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GCanvasContext::InitFBO()
{
    if (0 != mContextType) return;
    if (!mIsFboSupported)
    {
        return;
    }

    DeleteFBO();

    glGenFramebuffers(1, &mFboFrame);
    BindFBO();

    GLuint tid = 0;
    glGenTextures(1, &tid);
    mFboTexture.SetTextureID(tid);
    mFboTexture.SetWidth(mWidth);
    mFboTexture.SetHeight(mHeight);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, mFboTexture.GetFormat(), mFboTexture.GetWidth(),
                 mFboTexture.GetHeight(), 0, mFboTexture.GetFormat(),
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mFboTexture.GetTextureID(), 0);

    const char *extString = (const char *)glGetString(GL_EXTENSIONS);
    bool OES_packed_depth_stencil =
        (strstr(extString, OES_PACKED_DEPTH_STENCIL) != 0);

    if (OES_packed_depth_stencil)
    {
        glGenRenderbuffers(1, &mFboStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, mFboStencil);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH24_STENCIL8_OES /*GL_DEPTH_COMPONENT16*/,
                              mFboTexture.GetWidth(), mFboTexture.GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
    }
    else
    {
        glGenRenderbuffers(1, &mFboStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, mFboStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8,
                              mFboTexture.GetWidth(), mFboTexture.GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
    }

    ClearScreen(mClearColor);
    glClear(GL_STENCIL_BUFFER_BIT);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    bool is_fbo_supported = true;
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        is_fbo_supported = false;

        LOG_D("failed to make complete framebuffer object %x", status);
        LOG_D("FBO fail ! fboFrame = %d, fboTexture = %d, fboStencial = "
                  "%d, w = %d, h = %d",
                  mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
                  mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }
    else
    {
        is_fbo_supported = true;

        LOG_D("FBO complete! fboFrame = %d, fboTexture = %d, fboStencial = "
                  "%d, w = %d, h = %d",
                  mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
                  mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }

    UnbindFBO();

    mIsFboSupported = is_fbo_supported;
}

void GCanvasContext::BindFBO()
{
    if (!mIsFboSupported)
    {
        return;
    }

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);
}

void GCanvasContext::UnbindFBO()
{
    if (!mIsFboSupported)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mSaveFboFrame);
}

GCanvasContext::~GCanvasContext()
{
    DeleteFBO();
//    GShaderManager::release();

#ifdef ANDROID
    GFontCache::ClearInstance();
    delete mShaderManager;
    mShaderManager = nullptr;
#endif
}

bool GCanvasContext::InitializeGLEnvironment()
{
    if (0 != mContextType)
    { // only 2D need to init
        return true;
    }
    LOG_D("initailizeGLEnvironment");

#ifdef ANDROID
    if(mShaderManager == nullptr) {
        mShaderManager = new GShaderManager();
    }
#endif

    if (mWidth > 0 && mHeight > 0)
    {
        InitFBO();
    }
    glEnable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);

    glViewport(0, 0, mWidth, mHeight);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    CalculateProjectTransform();
    ResetStateStack();

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

void GCanvasContext::ResetStateStack()
{
    mPath.Reset();

    bool hasOldState = (nullptr != mCurrentState);
    GCompositeOperation oldCompositeOp = COMPOSITE_OP_SOURCE_OVER;
    GTransform oldTransfrom = {0, 0, 0, 0, 0, 0};

    if (hasOldState)
    {
        oldCompositeOp = mCurrentState->mGlobalCompositeOp;
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

    if (hasOldState == false ||
        oldCompositeOp != mCurrentState->mGlobalCompositeOp)
    {
        SetGlobalCompositeOperation(mCurrentState->mGlobalCompositeOp);
    }

    if (!hasOldState ||
        !GTransformEqualToTransform(oldTransfrom, mCurrentState->mTransform))
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }

    SetClipFlag(false);
    if (mCurrentState->mClipPath != nullptr)
    {
        mCurrentState->mClipPath->ClipRegion(this);
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
    if (mCurrentState->mShader->GetColorSlot())
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

void GCanvasContext::SetTexture(const GTexture *texture)
{
    SendVertexBufferToGPU();

    bool has_texture = (texture != nullptr);
    mCurrentState->mShader->SetHasTexture(has_texture);
    if (texture != nullptr)
    {
        glActiveTexture(GL_TEXTURE0);
        texture->Bind();
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

    glDrawArrays(geometry_type, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;
}

void GCanvasContext::PushTriangle(GPoint v1, GPoint v2, GPoint v3,
                                   GColorRGBA color)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 3)
    {
        SendVertexBufferToGPU();
    }

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
    GPoint p = {0, 0};
    vb[0].pos = v1;
    vb[1].pos = v2;
    vb[2].pos = v3;
    vb[0].uv = vb[1].uv = vb[2].uv = p;
    vb[0].color = vb[1].color = vb[2].color = color;

    mVertexBufferIndex += 3;
}

void GCanvasContext::PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                               GColorRGBA color)
{
    if (mVertexBufferIndex >= GCANVAS_VERTEX_BUFFER_SIZE - 6)
    {
        SendVertexBufferToGPU();
    }

    GVertex *vb = &CanvasVertexBuffer[mVertexBufferIndex];
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

    mVertexBufferIndex += 6;
}

void GCanvasContext::PushRectangle(float x, float y, float w, float h,
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

void GCanvasContext::PushPoints(const std::vector< GPoint > &points,
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


void GCanvasContext::Save()
{
    // since previous draw commands
    // are buffered, them should be draw
    // before switching to other state
    SendVertexBufferToGPU();

    GCanvasState newstate = mStateStack.back();
    mStateStack.push_back(newstate);
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

    mStateStack.pop_back();
    mCurrentState = &mStateStack.back();

    SetClipFlag(false);
    if (mCurrentState->mClipPath)
    {
        SetTransformOfShader(mCurrentState->mClipTransform);
        mCurrentState->mClipPath->ClipRegion(this);
        SetClipFlag(true);
    }

    SetTransformOfShader(mCurrentState->mTransform);
    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->Bind();
    }

    return true;
}

void GCanvasContext::Scale(float x, float y)
{
    mCurrentState->mTransform =
            GTransformScale(mCurrentState->mTransform, x, y);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::Rotate(float angle)
{
    mCurrentState->mTransform =
            GTransformRotate(mCurrentState->mTransform, angle);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::Translate(float x, float y)
{
    mCurrentState->mTransform =
            GTransformTranslate(mCurrentState->mTransform, x, y);
    SetTransformOfShader(mCurrentState->mTransform);
}

void GCanvasContext::ApplyTransform(float m11, float m12, float m21, float m22,
                                     float dx, float dy)
{
    mCurrentState->mTransform = GTransformConcat(
            mProjectTransform, GTransformMake(m11, m12, m21, m22, dx, dy));
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
        
        GShader *shader = dynamic_cast<GShader*>(mCurrentState->mShader);
        if( nullptr == shader)
        {
            LOG_I("SetTransformOfShader ===> shader is null");
            return;
        }
        
        std::string name = shader->GetName();
#ifdef IOS
        if( nullptr == GShaderManager::getSingleton()->programForKey(name) )
#endif

#ifdef ANDROID
        if( nullptr == mShaderManager->programForKey(name) )
#endif
        {
            LOG_I("SetTransformOfShader program for key is null");
            return;
        }
        
        glUniformMatrix4fv(shader->GetTransformSlot(), 1, GL_FALSE, &(m[0]));
        //glUniformMatrix4fv(current_state_->shader->GetTransformSlot(), 1, GL_FALSE, &(m[0]));

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

void GCanvasContext::SetGlobalCompositeOperation(GCompositeOperation op)
{
    SendVertexBufferToGPU();
    glBlendFunc(GCompositeOperationFuncs[op].source,
                GCompositeOperationFuncs[op].destination);
    mCurrentState->mGlobalCompositeOp = op;
}


void GCanvasContext::ClearRect(float x, float y, float w, float h)
{
#ifdef IOS
    UseDefaultRenderPipeline();
#endif
    SetTexture(nullptr);

    GCompositeOperation oldOp = mCurrentState->mGlobalCompositeOp;
    SendVertexBufferToGPU();
    SetGlobalCompositeOperation(COMPOSITE_OP_REPLACE);

    PushRectangle(x, y, w, h, 0, 0, 0, 0, mClearColor);

    SetGlobalCompositeOperation(oldOp);
}

void GCanvasContext::FillRect(float x, float y, float w, float h)
{
    GColorRGBA color = mCurrentState->mFillColor;
    color.rgba.a = (float)color.rgba.a * mCurrentState->mGlobalAlpha;
    PushRectangle(x, y, w, h, 0, 0, 0, 0, color);
}

void GCanvasContext::StrokeRect(float x, float y, float w, float h)
{
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

void GCanvasContext::Fill()
{
    mCurrentState->mShader->SetHasTexture(0);
    mPath.DrawPolygons2DToContext(this);
}

void GCanvasContext::Stroke() { mPath.DrawLinesToContext(this); }

void GCanvasContext::ClipRegion()
{
    SendVertexBufferToGPU();

    if (mCurrentState->mClipPath)
    {
        delete mCurrentState->mClipPath;
        mCurrentState->mClipPath = nullptr;
    }

    mPath.Close();
    mCurrentState->mClipPath = new GPath(mPath);
    mCurrentState->mClipTransform = mCurrentState->mTransform;
    mCurrentState->mClipPath->ClipRegion(this);
    SetClipFlag(true);
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

void GCanvasContext::DrawImage1(float w, float h, int TextureId, float sx,
                                 float sy, float sw, float sh, float dx,
                                 float dy, float dw, float dh)
{
    GColorRGBA color;
    color.rgba.r = 1.0f;
    color.rgba.g = 1.0f;
    color.rgba.b = 1.0f;
    color.rgba.a = mCurrentState->mGlobalAlpha;
    LOG_D("DrawImage1, texture id = %d, alpha = %f, w = %f, h = %f, sx = %f, sy = %f, sw = %f, sh = %f, dx = %f, dy = %f, dw = %f, dh = %f ", TextureId, color.rgba.a, w, h, sx, sy, sw, sh, dx, dy, dw, dh);
    
    if( color.rgba.a < 1.0 )
    {
        GCompositeOperation oldOp = mCurrentState->mGlobalCompositeOp;
        SetGlobalCompositeOperation(COMPOSITE_OP_ALPHA);

        mCurrentState->mShader->SetHasTexture(1);
        glBindTexture(GL_TEXTURE_2D, TextureId);
        PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color);

        SetGlobalCompositeOperation(oldOp);
    }
    else
    {
        mCurrentState->mShader->SetHasTexture(1);
        glBindTexture(GL_TEXTURE_2D, TextureId);
        PushRectangle(dx, dy, dw, dh, sx / w, sy / h, sw / w, sh / h, color);
    }

    SendVertexBufferToGPU();
}

void GCanvasContext::ClosePath() { mPath.Close(); }

void GCanvasContext::MoveTo(float x, float y) { mPath.MoveTo(x, y); }

void GCanvasContext::LineTo(float x, float y) { mPath.LineTo(x, y); }

void GCanvasContext::QuadraticCurveTo(float cpx, float cpy, float x, float y)
{
    float scale = mCurrentState->mTransform.a * mCurrentState->mTransform.a +
                  mCurrentState->mTransform.d * mCurrentState->mTransform.d;
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
    mCurrentState->mFillColor = c;
    UseDefaultRenderPipeline();
    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->SetHasTexture(0);
    }
}

void GCanvasContext::SetClearColor(const GColorRGBA &c)
{
    mClearColor = c;
}

void GCanvasContext::SetStrokeStyle(GColorRGBA c)
{
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
}

void GCanvasContext::RestoreRenderPipeline()
{
    mCurrentState->mShader = mSaveShader;
    mCurrentState->mShader->Bind();
}

void GCanvasContext::UseDefaultRenderPipeline()
{
    SendVertexBufferToGPU();
    
#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("DEFAULT");
#endif
    
#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("DEFAULT");
#endif
    
    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->Bind();
    }

    if (mCurrentState->mFillStyle != nullptr)
    {
        delete mCurrentState->mFillStyle;
        mCurrentState->mFillStyle = nullptr;
    }
    if (mCurrentState != nullptr)
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }
}

void GCanvasContext::UsePatternRenderPipeline()
{
    SendVertexBufferToGPU();
    
#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif
    
#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("PATTERN");
#endif
    
    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->Bind();
    }
    if (mCurrentState->mFillStyle != nullptr)
    {
        delete mCurrentState->mFillStyle;
        mCurrentState->mFillStyle = nullptr;
    }
    if (mCurrentState != nullptr)
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }
}

void GCanvasContext::UseLinearGradientPipeline()
{
    SendVertexBufferToGPU();
    
#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("LINEAR");
#endif
    
#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("LINEAR");
#endif

    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->Bind();
    }
    if (mCurrentState->mFillStyle != nullptr &&
        mCurrentState->mFillStyle->IsLinearGradient())
    {
        FillStyleLinearGradient *grad =
            dynamic_cast< FillStyleLinearGradient * >(
                mCurrentState->mFillStyle);
        mCurrentState->mShader->SetRange(grad->GetStartPos(),
                                         grad->GetEndPos());
        mCurrentState->mShader->SetColorStopCount(grad->GetColorStopCount());

        const int count = grad->GetColorStopCount();
        for (int i = 0; i < count; ++i)
        {
            const FillStyleLinearGradient::ColorStop *stop =
                grad->GetColorStop(i);
            float color[4];
            color[0] = stop->color.rgba.r;
            color[1] = stop->color.rgba.g;
            color[2] = stop->color.rgba.b;
            color[3] = stop->pos;
            mCurrentState->mShader->SetColorStop(color, i);
        }
    }
    if (mCurrentState != nullptr)
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }
}

void GCanvasContext::UseRadialGradientPipeline()
{
    SendVertexBufferToGPU();
    
#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("RADIAL");
#endif
    
#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("RADIAL");
#endif

    if (mCurrentState->mShader != nullptr)
    {
        mCurrentState->mShader->Bind();
    }
    if (mCurrentState->mFillStyle != nullptr &&
        mCurrentState->mFillStyle->IsRadialGradient())
    {
        FillStyleRadialGradient *grad =
            dynamic_cast< FillStyleRadialGradient * >(
                mCurrentState->mFillStyle);
        mCurrentState->mShader->SetRange(grad->GetStartPos(),
                                         grad->GetEndPos());
        mCurrentState->mShader->SetColorStopCount(grad->GetColorStopCount());

        const int count = grad->GetColorStopCount();
        for (int i = 0; i < count; ++i)
        {
            const FillStyleRadialGradient::ColorStop *stop =
                grad->GetColorStop(i);
            float color[4];
            color[0] = stop->color.rgba.r;
            color[1] = stop->color.rgba.g;
            color[2] = stop->color.rgba.b;
            color[3] = stop->pos;
            mCurrentState->mShader->SetColorStop(color, i);
        }
    }
    if (mCurrentState != nullptr)
    {
        SetTransformOfShader(mCurrentState->mTransform);
    }
}

#ifdef ANDROID
std::string GCanvas::exe2dSyncCmd(int cmd,const char *&args){
    return "";
}
#endif
