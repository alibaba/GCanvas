/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "gcanvas/GPoint.h"
#include "GCanvas.hpp"
#include "gcanvas/GShaderManager.h"
//#include "png/PngLoader.h"
#include "support/Encode.h"
#include "support/Util.h"
#include <sstream>

#ifdef ANDROID

#include "gcanvas/GFontCache.h"
#include "support/CharacterSet.h"
#include "GCanvasLinkNative.h"
#include <3d/gmanager.h>

#endif

#include "GCanvasManager.h"

#ifdef IOS
#include <sys/time.h>
#endif

using namespace gcanvas;


void GCanvas::Clear() {
    LOG_D("Canvas::DoContextLost start.");

    // No need to clean up GL memory with glDeleteBuffers or glDeleteTextures.
    // It all gets blown away automatically when the context is lost.

    mContextLost = true;

    mTextureMgr.Clear();

#ifdef ANDROID
    clearCmdQueue();
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    LOG_D("Canvas::DoContextLost end.");
}

int g_clear_color_time = 0;

GCanvas::GCanvas(std::string contextId, bool flip, bool onScreen) :
        GCanvasContext(0, 0, flip),
        mCurrentTransform(1, 0, 0, 1, 0, 0),
        mOnScreen(onScreen) {
    mContextId = contextId;
    mRenderCount = 0;
    mLastTime = clock();
    mFrames = 0;
    mMessages = 0;
    mFps = 0.0f;
    mMsgLength = 0;
    mTyOffset = 0;
    mPixelFlipY = 0;

    mContextLost = false;
    mTyOffsetFlag = false;
    g_clear_color_time = 0; // need reset by env init

//    mCurTransfrom = GTransformIdentity;

#ifdef ANDROID
    sem_init(&mSyncSem, 0, 0);
#endif

    mResult = "";

    LOG_D("Create Canvas");
}

GCanvas::~GCanvas() {
    LOG_D("Canvas clear");
    Clear();
}

void GCanvas::OnSurfaceChanged(int x, int y, int width, int height) {
    GLint maxRenderbufferSize;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
    if ((maxRenderbufferSize <= width) || (maxRenderbufferSize <= height)) {
        LOG_D("GCanvas::OnSurfaceChanged with or height exceed the max buffersize: %d",
              maxRenderbufferSize);
        return;
    }

    if (mWidth != width || mHeight != height) {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
        InitializeGLEnvironment();
    }

    if (mTyOffsetFlag) {
        mTyOffset = mHeight;
        mCurrentTransform.ty = mTyOffset;
    }

    mContextLost = false;
    LOG_D("GCanvas::OnSurfaceChanged mContextLost %d", mContextLost);
}

void GCanvas::execResize(int width, int height) {
    mWidth = width;
    mHeight = height;

    glViewport(0, 0, mWidth, mHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (mContextType == 0) {
        mVertexBufferIndex = 0;
        CalculateProjectTransform(mWidth, mHeight);
        ResetStateStack();
        SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER);
        UseDefaultRenderPipeline();
    }

    mFboMap.erase(DefaultFboName);
    InitFBO();
    BindFBO();
}

void GCanvas::execSetShadowColor(const char *str) {
    GColorRGBA color = StrValueToColorRGBA(str);
    mCurrentState->mShadowColor = color;
}

void GCanvas::execSetShadowBlur(int blur) {
    mCurrentState->mShadowBlur = blur;

}

void GCanvas::execSetShadowOffsetX(float x) {
    mCurrentState->mShadowOffsetX = x;

}

void GCanvas::execSetShadowOffsetY(float y) {
    mCurrentState->mShadowOffsetY = y;

}

void GCanvas::SetBackgroundColor(float red, float green, float blue) {
    GColorRGBA color;
    color.rgba.r = red;
    color.rgba.g = green;
    color.rgba.b = blue;
    color.rgba.a = 1.0f;
    GCanvasContext::SetFillStyle(color);
}

void GCanvas::SetOrtho(int width, int height) {
}

void GCanvas::AddTexture(int textureGroupId, int glID, int width, int height) {
    LOG_D("AddTexture, Group ID = %d, GL ID = %d, width = %d, height = %d, context lost = %d",
          textureGroupId, glID, width, height, mContextLost);
    if (mContextLost) return;
    mTextureMgr.Append(textureGroupId, glID, width, height);
}

#ifdef IOS
void GCanvas::AddOfflineTexture(int textureGroupId, int glID){
    if (mContextLost) return;
    
    std::map< int, int >::iterator iter = mOfflineTextures.find(glID);
    if (iter == mOfflineTextures.end())
    {
        mOfflineTextures.insert( std::pair< int, int >(textureGroupId, glID) );
    }
}
#endif

bool GCanvas::AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                            unsigned int *pWidth, unsigned int *pHeight) {
    if (mContextLost) return false;
    return mTextureMgr.AppendPng(buffer, size, textureGroupId, pWidth, pHeight);
}

void GCanvas::RemoveTexture(int textureId) {
    if (mContextLost) return;
    mTextureMgr.Remove(textureId);
}

const char *GCanvas::parseTokens(const char *p, float *tokens, int iMaxCount) {
    int iToken = 0;

    while (*p && *p != ';' && iToken < iMaxCount) {
        tokens[iToken++] = fastFloat(p);
        while (*p && (*p != ',' && *p != ';')) {
            ++p;
        }
        if (*p == ',') ++p;
    }
    if (*p == ';') ++p;
    return p;
}

// From the current position, past semicolon or to end
// set the GL matrix if we have all the right data

const char *GCanvas::parseSetTransformT(
        const char *p,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    if (concat) {
        GTransform t;
        parseTokesOpt(reinterpret_cast<float *>(&t), &p);

        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        parseTokesOpt(reinterpret_cast<float *>(transOut), &p);
    }

    if (*p == ';') ++p;

    return p;
}

const char *GCanvas::parseSetTransform(
        const char *p,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    float tokens[6] = {0, 0, 0, 0, 0, 0};

// #define debug_Parsetokes 1
#ifdef debug_Parsetokes
    const char *p0 = p;
#endif

#if origin_code_realization
    int iToken = 0;
    while (*p && *p != ';' && iToken < 6)
    {
        tokens[iToken++] = atof(p);

        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }

#ifdef debug_Parsetokes

    FILE *flog = fopen("/storage/sdcard0/ParseTokesOrigin1.txt", "a+");
    fwrite((p0 - 1), 1, (p - p0 + 1 + 1), flog);
    char buffer[256] = {0};
    int len = snprintf(buffer, "\n %f,%f,%f,%f,%f,%f \n", tokens[0], tokens[1],
                        tokens[2], tokens[3], tokens[4], tokens[5]);
    fwrite(buffer, 1, len, flog);
    fclose(flog);
#endif
#else
    parseTokesOpt(tokens, &p);
#endif

    GTransform t;
    switch (parseMode) {
        case IDENTITY:
            break;
        case SET_XFORM:
            t.a = tokens[0];
            t.b = tokens[1];
            t.c = tokens[2];
            t.d = tokens[3];
            t.tx = tokens[4];
            t.ty = tokens[5];
            break;
        case SCALE:
            t.a = tokens[0];
            t.d = tokens[1];
            break;
        case ROTATE: {
            double angle = tokens[0];

            double ca = cos(angle);
            double sa = sin(angle);

            t.a = static_cast<float>(ca);
            t.b = static_cast<float>(sa);
            t.c = static_cast<float>(-sa);
            t.d = static_cast<float>(ca);
        }
            break;
        case TRANSLATE:
            t.tx = tokens[0];
            t.ty = tokens[1];
            break;
        default: ASSERT(0);
            break;
    }

    if (concat) {
        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        *transOut = t;
    }

    if (*p == ';') ++p;
    return p;
}

const char *GCanvas::parseDrawImage(const char *p, Clip *clipOut) {
#if origin_code_realization
    float tokens[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int iToken = 0;

    while (*p && *p != ';' && iToken < 9)
    {
        if (iToken == 0)
        {
            // First token is the texture ID
            clipOut->textureID = FastInt(p);
        }
        else
        {
            tokens[iToken - 1] = FastFloat(p);
        }
        iToken++;
        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }

    clipOut->cx = tokens[0]; // cx
    clipOut->cy = tokens[1]; // cy
    clipOut->cw = tokens[2]; // cw
    clipOut->ch = tokens[3]; // ch
    clipOut->px = tokens[4]; // px
    clipOut->py = tokens[5]; // py
    clipOut->pw = tokens[6]; // pw
    clipOut->ph = tokens[7]; // ph
#else
    clipOut->textureID = atoi(p);

    for (; *p != ','; p++); // skip image.id
    p++;

//    for (; *p != ','; p++); // skip image.src
//    p++;

    parseTokesOpt(&clipOut->cx, &p);
#endif

    if (*p == ';') ++p;
    return p;
}

const char *GCanvas::parseName(const char *p, std::string &name) {
    const char *old = p;
    while (*p && *p != ';') {
        ++p;
    }
    name.assign(old, p - old);
    if (*p == ';') ++p;
    return p;
}

const char *
GCanvas::parseBindingPara(const char *p, std::string &name, float &width, float &height) {
    const char *old = p;
    while (*p && *p != ',') {
        ++p;
    }
    name.assign(old, p - old);

    if (*p == ',') ++p;


    width = fastFloat(p);
    while (*p && *p != ',') {
        ++p;
    }
    if (*p == ',') ++p;
    height = fastFloat(p);
    while (*p && *p != ';') {
        ++p;
    }
    if (*p == ';') ++p;
    return p;
}

const char *
GCanvas::parseBindingPara(const char *p, std::string &name, float &sx, float &sy, float &sw,
                          float &sh,
                          float &dx, float &dy, float &dw, float &dh) {
    const char *old = p;
    while (*p && *p != ',') {
        ++p;
    }
    name.assign(old, p - old);

    if (*p == ',') ++p;

    float tokens[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    parseTokesOpt(tokens, &p);

    sx = tokens[0];
    sy = tokens[1];
    sw = tokens[2];
    sh = tokens[3];

    dx = tokens[4];
    dy = tokens[5];
    dw = tokens[6];
    dh = tokens[7];

    if (*p == ';') ++p;
    return p;
}

// From the current position, past semicolon or to end
const char *GCanvas::parseUnknown(const char *p) {
    while (*p && *p != ';') {
        ++p;
    }
    if (*p == ';') ++p;
    return p;
}

const Texture *GCanvas::getTextureWithOneImage(int aId) {
    const TextureGroup *texture_group = mTextureMgr.Get(aId);
    if (texture_group != nullptr) {
        return texture_group->Get(0);
    }
    return nullptr;
}

const char *GCanvas::extractOneParameterFromCommand(char *outParameter,
                                                    const char *commands) {
    int i = 0;
    const char *p = commands;
    bool has_left_parentheses = false;
    while (*p != ';' && (has_left_parentheses || *p != ',')) {
        if (*p == '(') {
            has_left_parentheses = true;
        } else if (*p == ')') {
            has_left_parentheses = false;
        }
        outParameter[i++] = *p++;
    }
    outParameter[i] = '\0';
    return p;
}

void GCanvas::drawFrame() {
    SendVertexBufferToGPU();
    ClearGeometryDataBuffers();
}

void GCanvas::execSetTransform(float a, float b, float c, float d, float tx, float ty) {
    SendVertexBufferToGPU();
    GTransform t = GTransform(a, c, b, d, tx, ty);
    mCurrentState->mTransform = GTransformConcat(mProjectTransform, t);
}

void
GCanvas::execDrawImageNew(int textureId, int textureWidth, int textureHeight, float sx, float sy,
                          float sw, float sh, float dx, float dy, float dw, float dh, bool flipY) {
    ImageBlur(textureWidth, textureHeight, textureId, sx, sy, sw, sh, dx, dy, dw, dh);
    UseTextureRenderPipeline();
    DrawImage1(textureWidth, textureHeight, textureId, sx, sy, sw, sh, dx, dy, dw, dh, flipY);
}


void GCanvas::execTransfrom(float a, float b, float c, float d, float tx, float ty) {
    SendVertexBufferToGPU();
    GTransform t = GTransform(a, c, b, d, tx, ty);
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform, t);

}

void GCanvas::execResetTransfrom() {
    SendVertexBufferToGPU();
    mCurrentState->mTransform = GTransformConcat(mProjectTransform, GTransformIdentity);
}

void GCanvas::execScale(float sx, float sy) {
    SendVertexBufferToGPU();
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeScale(sx, sy));
}

void GCanvas::execRotate(float angle) {
    SendVertexBufferToGPU();
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeRotation(angle));
}

void GCanvas::execTranslate(float tx, float ty) {
    SendVertexBufferToGPU();
    mCurrentState->mTransform = GTransformConcat(mCurrentState->mTransform,
                                                 GTransformMakeTranslation(tx, ty));

}

void GCanvas::execSave() {
    Save();
}

void GCanvas::execRestore() {
    Restore();
}

void GCanvas::execSetGlobalAlpha(float alpha) {
    float a = std::min<float>(1, std::max<float>(alpha, 0));
    SetGlobalAlpha(a);
}

void GCanvas::execSetFillStyle(const char *str) {
    GColorRGBA color = StrValueToColorRGBA(str);
    SetFillStyle(color);
}

void GCanvas::execSetStrokeStyle(const char *str) {
    GColorRGBA color = StrValueToColorRGBA(str);
    SetStrokeStyle(color);
}

void GCanvas::execSetFillStylePattern(int textureId, int width, int height, const char *repeatMode,
                                      bool isStroke) {
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr) {
        delete style;
        style = nullptr;
    }

    style = new FillStylePattern(textureId, width, height, repeatMode);
    if (isStroke) {
        mCurrentState->mStrokeStyle = style;
    } else {
        mCurrentState->mFillStyle = style;
    }
}

void GCanvas::execSetFillStyleLinearGradient(float startArr[], float endArr[], int stop_count,
                                             const float posArray[], const std::string colorArray[],
                                             bool isStroke) {
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;

    if (style != nullptr) {
        delete style;
        style = nullptr;
    }

    GPoint start, end;
    start.x = startArr[0];
    start.y = startArr[1];
    end.x = endArr[0];
    end.y = endArr[1];

    style = new FillStyleLinearGradient(start, end);
    if (isStroke) {
        mCurrentState->mStrokeStyle = style;
    } else {
        mCurrentState->mFillStyle = style;
    }

    for (int i = 0; i < stop_count; ++i) {
        float pos = posArray[i];
        std::string color = colorArray[i];
        static_cast< FillStyleLinearGradient * >(style)->AddColorStop(pos, color);
    }
}

void GCanvas::execSetFillStyleRadialGradient(float startArr[], float endArr[], int stop_count,
                                             const float posArray[], const std::string colorArray[],
                                             bool isStroke) {
    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;

    if (style != nullptr) {
        delete style;
        style = nullptr;
    }
    style = new FillStyleRadialGradient(startArr, endArr);
    if (isStroke) {
        mCurrentState->mStrokeStyle = style;
    } else {
        mCurrentState->mFillStyle = style;
    }

    for (int i = 0; i < stop_count; ++i) {
        float pos = posArray[i];
        std::string color = colorArray[i];
        static_cast< FillStyleRadialGradient * >(style)->AddColorStop(pos, color);
    }
}


void GCanvas::execSetLineStyle(const char *str) {
    GColorRGBA color = StrValueToColorRGBA(str);
    SetStrokeStyle(color);
}

void GCanvas::execSetLineWidth(float w) {
    SetLineWidth(w);
}

void GCanvas::execSetLineCap(const char *p) {
    if (strncmp(p, "butt", 4) == 0) {
        SetLineCap(LINE_CAP_BUTT);
    } else if (strncmp(p, "round", 5) == 0) {
        SetLineCap(LINE_CAP_ROUND);
    } else if (strncmp(p, "square", 6) == 0) {
        SetLineCap(LINE_CAP_SQUARE);
    }
}

void GCanvas::execSetLineJoin(const char *p) {
    if (strncmp(p, "miter", 4) == 0) {
        SetLineJoin(LINE_JOIN_MITER);
    } else if (strncmp(p, "bevel", 5) == 0) {
        SetLineJoin(LINE_JOIN_BEVEL);
    } else if (strncmp(p, "round", 5) == 0) {
        SetLineJoin(LINE_JOIN_ROUND);
    }
}

void GCanvas::execSetMiterLimit(float miterlimit) {
    SetMiterLimit(miterlimit);
}

#ifdef SUPPORT_LINEDASH
void GCanvas::execSetLineDash(std::vector<float> lineDash)
{
    SetLineDash(lineDash);
}

void GCanvas::execSetLineDashOffset(float lineDashOffset)
{
    SetLineDashOffset(lineDashOffset);
}

#endif

void GCanvas::execStrokeRect(float x, float y, float w, float h) {
    StrokeRect(x, y, w, h);
}

void GCanvas::execClearRect(float x, float y, float w, float h) {

    ClearRect(x, y, w, h);
}

void GCanvas::execClip() {
    GCanvasContext::ClipRegion();
}

void GCanvas::execResetClip() {
    ResetClip();
}

void GCanvas::execClosePath() {
    ClosePath();
}

void GCanvas::execMoveTo(float x, float y) {
    MoveTo(x, y);
}

void GCanvas::execLineTo(float x, float y) {
    LineTo(x, y);
}

void GCanvas::execQuadraticCurveTo(float cpx, float cpy, float x, float y) {
    QuadraticCurveTo(cpx, cpy, x, y);
}

void GCanvas::execBezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {
    BezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

void GCanvas::execArcTo(float x1, float y1, float x2, float y2, float radius) {
    ArcTo(x1, y1, x2, y2, radius);
}

void GCanvas::execBeginPath() {
    BeginPath();
}

void GCanvas::execFillRect(float x, float y, float w, float h) {
    BeginPath();
    FillRectBlur(x, y, w, h);
    FillRect(x, y, w, h);
}

void GCanvas::execRect(float x, float y, float w, float h) {
    Rect(x, y, w, h);
}

void GCanvas::execFill() {
    Fill();
}

void GCanvas::execStroke() {
    StrokeBlur();
    Stroke();
}

void GCanvas::execArc(float x, float y, float radius, float startAngle, float endAngle,
                      int antiClockwise) {
    Arc(x, y, radius, startAngle, endAngle, (bool) antiClockwise);
}

void GCanvas::execSetGlobalCompositeOperation(int op) {
    SetGlobalCompositeOperation(GCompositeOperation(op), GCompositeOperation(op));
}

void GCanvas::execSetTextAlign(int textAlign) {
    mCurrentState->mTextAlign = GTextAlign(textAlign);
}

void GCanvas::execSetTextBaseline(int textbaseline) {
    mCurrentState->mTextBaseline = GTextBaseline(textbaseline);
}

void GCanvas::execDrawText(const char *text, float x, float y, float maxWidth, int strLength) {
    DrawText(text, x, y, maxWidth, false, strLength);
}

void GCanvas::execStrokeText(const char *text, float x, float y, float maxWidth, int strLength) {
    DrawText(text, x, y, maxWidth, true, strLength);
}

float GCanvas::execMeasureTextWidth(const char *text, int strLength) {
    if (strLength == 0) {
        strLength = static_cast<int>(strlen(text));
    }
    if (mCurrentState->mFont == nullptr) {
        mCurrentState->mFont = new GFontStyle(nullptr, mDevicePixelRatio);
    }
    float width = mFontManager->MeasureText(text, strLength, mCurrentState->mFont);
//    float scale = mCurrentState->mTransform.a / mDevicePixelRatio;
//    return width * scale * mWidth / (2.f * mDevicePixelRatio);
    return width;
}

void GCanvas::execFont(const char *font) {
    if (mCurrentState->mFont != nullptr) {
        if (strcmp(mCurrentState->mFont->GetName().c_str(), font) == 0) {
            return;
        }
        delete mCurrentState->mFont;
    }
    mCurrentState->mFont = new GFontStyle(font, mDevicePixelRatio);


}

void GCanvas::execPutImageData(const unsigned char *rgbaData, int tw,
                               int th, int x, int y, int sx, int sy,
                               int sw, int sh) {
    SendVertexBufferToGPU();
    GLuint glID = execBindImage(rgbaData, GL_RGBA, tw, th);
    sw = sw > tw ? tw : sw;
    sh = sh > th ? th : sh;
    sw -= sx;
    sh -= sy;
    execDrawImageNew(glID, tw, th, sx, sy, sw, sh, x + sx, y + sy, sw, sh);


    SendVertexBufferToGPU();
    SetTexture(InvalidateTextureId);
    glDeleteTextures(1, &glID);
}

struct RGBA {

    RGBA &operator+=(int c) {
        r += c & 0xff;
        g += c >> 8 & 0xff;
        b += c >> 16 & 0xff;
        a += c >> 24 & 0xff;

        return *this;
    }

    operator uint32_t() {
        uint32_t c = 0;
        c += r & 0xff;
        c += g << 8 & 0xff00;
        c += b << 16 & 0xff0000;
        c += a << 24 & 0xff000000;

        return c;
    }

    uint16_t r = 0;
    uint16_t g = 0;
    uint16_t b = 0;
    uint16_t a = 0;
};

inline RGBA operator/(const RGBA &a, int b) {
    RGBA c;
    c.r = a.r / b;
    c.g = a.g / b;
    c.b = a.b / b;
    c.a = a.a / b;
    return c;
}

inline int GetPixel(int *pixels, int x, int y, int width, int height) {
    if (x < 0) {
        x = 0;
    }
    if (x > width - 1) {
        x = width - 1;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > height - 1) {
        y = height - 1;
    }
    return pixels[y * width + x];

}

inline void Sampler(int inWidth, int inHeight, int *inPixels,
                    int outWidth, int outHeight, int *outPixels) {
    for (int y = 0; y < outHeight; ++y) {
        int inY = y * inHeight / outHeight;
        int revertY = inHeight - inY - 1;
        for (int x = 0; x < outWidth; ++x) {
            int inX = x * inWidth / outWidth + 1;
            RGBA pixel;
            pixel += GetPixel(inPixels, inX, revertY, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX - 1, revertY - 1, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX, revertY - 1, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX + 1, revertY - 1, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX - 1, revertY, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX + 1, revertY, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX - 1, revertY + 1, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX, revertY + 1, inWidth, inHeight);
            pixel += GetPixel(inPixels, inX - 1, revertY + 1, inWidth, inHeight);
            outPixels[y * outWidth + x] = pixel / 9;
        }
    }
}

void GCanvas::execGetImageData(int x, int y, int width, int height, uint8_t *pixels) {
    using namespace std;
    SendVertexBufferToGPU();

    vector<int> rawPixel;

    int realX = x * mDevicePixelRatio;
    int realY = y * mDevicePixelRatio;
    int realWidth = width * mDevicePixelRatio;
    int realHeight = height * mDevicePixelRatio;

    rawPixel.resize(realWidth * realHeight);

    glReadPixels(realX, mHeight - (realY + realHeight), realWidth, realHeight, GL_RGBA,
                 GL_UNSIGNED_BYTE, &rawPixel[0]);


    Sampler(realWidth, realHeight, &rawPixel[0], width, height, reinterpret_cast<int *>(pixels));
}

void GCanvas::execGetRawImageData(int width, int height, uint8_t *pixels) {
    SendVertexBufferToGPU();


    glReadPixels(0, 0, width, height, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);


}

int GCanvas::execBindImage(const unsigned char *rgbaData, GLint format, unsigned int width,
                           unsigned int height) {
    if (nullptr == rgbaData)
        return (GLuint) -1;

    GLuint glID;
    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, rgbaData);


    return glID;
}

#ifdef ANDROID

void GCanvas::execBeginDraw() {
    BindFBO();

}

void GCanvas::execEndDraw() {
    UnbindFBO();
    glClearColor(mClearColor.rgba.r, mClearColor.rgba.g, mClearColor.rgba.b, mClearColor.rgba.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (mOnScreen) {

        drawFBO(DefaultFboName, COMPOSITE_OP_REPLACE);
    }
}

int GCanvas::execGetFBOTexture() {
    return mFboMap[DefaultFboName].mFboTexture.GetTextureID();

}

#endif

int GCanvas::execDetachFBOTexture() {
    return mFboMap[DefaultFboName].DetachTexture();

}

void GCanvas::execute2dCommands(const char *renderCommands, int length) {
    if (mContextLost) return;


    if (LOG_LEVEL_DEBUG == GetLogLevel()) {
        char log_tmp[128];
        snprintf(log_tmp, sizeof(log_tmp), "%s", renderCommands);
        LOG_D("[executeRenderCommands][%d]%s", strlen(renderCommands), log_tmp);
    }

    char tmpFont[256];
    char tmpText[1024];
    GTransform &action = mCurrentTransform;

    ClearGeometryDataBuffers();
//    ApplyTransform(1, 0, 0, 1, 0, mTyOffset);
//    action.a = 1, action.b = 0, action.c = 0, action.d = 1, action.tx = 0,
//    action.ty = mTyOffset;
    ApplyTransform(action.a, action.b, action.c, action.d, action.tx, action.ty);
    Clip clip;
    const char *p = renderCommands;
    const char *end = renderCommands + length;
    while (p < end) {
        switch (*p) {
            case 't': {
                // setTransform
                p++;
                p = parseSetTransformT(p, SET_XFORM, false, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'd': {
                p++;
                // Load the clip
                p = parseDrawImage(p, &clip);

                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                UseDefaultRenderPipeline();
                DrawImage(clip.textureID, clip.cx, clip.cy, clip.cw, clip.ch,
                          clip.px, clip.py, clip.pw, clip.ph);
//                SendVertexBufferToGPU();

                break;
            }
            case 'Q': {
                p++;

                // bind FBO
                std::string name;
                float width, height;
                p = parseBindingPara(p, name, width, height);

                if (!mIsFboSupported) {
                    break;
                }
                GFrameBufferObject *fbo = nullptr;
                std::map<std::string, GFrameBufferObject>::iterator it = mFboMap.find(name);
                if (it == mFboMap.end()) {
                    fbo = &mFboMap[name];
                    fbo->InitFBO(width * mDevicePixelRatio, height * mDevicePixelRatio,
                                 StrValueToColorRGBA("transparent_white"));
                } else {
                    fbo = &it->second;
                }
                fbo->BindFBO();
                fbo->mSavedTransform = action;
                glViewport(0, 0, width * mDevicePixelRatio, height * mDevicePixelRatio);
                CalculateProjectTransform(width * mDevicePixelRatio, height * mDevicePixelRatio);
                action = GTransformIdentity;
//                action.a = 1, action.b = 0, action.c = 0, action.d = 1, action.tx = 0,
//                action.ty = (mHeight / mDevicePixelRatio - height);
                ApplyTransform(action.a, action.b, action.c, action.d, action.tx, action.ty);
                break;
            }
            case 'Y': {
                p++;

                // unbind FBO
                std::string name;
                p = parseName(p, name);

                if (!mIsFboSupported) {
                    break;
                }
                SendVertexBufferToGPU();
                GFrameBufferObject &fbo = mFboMap[name];
                fbo.UnbindFBO();
                glViewport(0, 0, mWidth, mHeight);
                action = fbo.mSavedTransform;
                CalculateProjectTransform(mWidth, mHeight);

                ApplyTransform(action.a, action.b, action.c, action.d, action.tx, action.ty);
                break;
            }
            case 'I': {
                p++;

                // draw FBO
                std::string name;
                float sx, sy, sw, sh, dx, dy, dw, dh;
                p = parseBindingPara(p, name, sx, sy, sw, sh, dx, dy, dw, dh);
                if (!mIsFboSupported) {
                    break;
                }
                GTexture &texture = mFboMap[name].mFboTexture;
                drawFBO(name, COMPOSITE_OP_SOURCE_OVER,
                        sx * 2 * mDevicePixelRatio / texture.GetWidth(),
                        sy * 2 * mDevicePixelRatio / texture.GetHeight(),
                        sw * mDevicePixelRatio / texture.GetWidth(),
                        sh * mDevicePixelRatio / texture.GetHeight(),
                        dx * 2 * mDevicePixelRatio / mWidth,
                        dy * 2 * mDevicePixelRatio / mHeight,
                        dw * mDevicePixelRatio / mWidth,
                        dh * mDevicePixelRatio / mHeight);

                break;
            }
            case 'f': {
                // transform
                p++;
                p = parseSetTransform(p, SET_XFORM, true, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'm': {
                // resetTransform
                p++;
                p = parseSetTransform(p, IDENTITY, false, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'k': {
                // scale
                p++;
                p = parseSetTransform(p, SCALE, true, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'r': {
                // rotate
                p++;
                p = parseSetTransform(p, ROTATE, true, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'l': {
                // translate
                p++;
                p = parseSetTransform(p, TRANSLATE, true, action, &action);
                ApplyTransform(action.a, action.b, action.c, action.d,
                               action.tx, action.ty);
                break;
            }
            case 'v': {
                // save
                p++;
                mActionStack.Append(&action, 1);
                Save();
                if (*p == ';') {
                    ++p;
                }
                break;
            }
            case 'e': {
                // restore
                p++;
                if (mActionStack.GetSize() > 0) {
                    Restore();
                    action = mActionStack[mActionStack.GetSize() - 1];
                    mActionStack.SetSize(mActionStack.GetSize() - 1);
                }
                if (*p == ';') {
                    ++p;
                }
                break;
            }
            case 'a': {
                // global alpha
                p++;
                float alpha = fastFloat(p);
                SetGlobalAlpha(alpha);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'F': {
                // fillStyle
                p++;
                char str[64] = "";
                char *tmp = str;
                while (*p && *p != ';') {
                    *tmp = *p;
                    ++tmp;
                    ++p;
                }
                if (*p == ';') ++p;
                GColorRGBA color = StrValueToColorRGBA(str);
                SetFillStyle(color);
                break;
            }
            case 'S': {
                // lineStyle
                p++;
                char str[64] = "";
                char *tmp = str;
                while (*p && *p != ';') {
                    *tmp = *p;
                    ++tmp;
                    ++p;
                }
                if (*p == ';') ++p;
                GColorRGBA color = StrValueToColorRGBA(str);
                SetStrokeStyle(color);
                break;
            }
            case 'W': {
                // lineWidth
                p++;
                float w = fastFloat(p);
                SetLineWidth(w);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'C': {
                // lineCap

                p++;
                if (strncmp(p, "butt", 4) == 0) {
                    SetLineCap(LINE_CAP_BUTT);
                } else if (strncmp(p, "round", 5) == 0) {
                    SetLineCap(LINE_CAP_ROUND);
                } else if (strncmp(p, "square", 6) == 0) {
                    SetLineCap(LINE_CAP_SQUARE);
                }

                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'J': {
                // lineJoin

                p++;
                if (strncmp(p, "miter", 4) == 0) {
                    SetLineJoin(LINE_JOIN_MITER);
                } else if (strncmp(p, "bevel", 5) == 0) {
                    SetLineJoin(LINE_JOIN_BEVEL);
                } else if (strncmp(p, "round", 5) == 0) {
                    SetLineJoin(LINE_JOIN_ROUND);
                }

                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'M': {
                // miterLimit
                p++;
                float w = fastFloat(p);
                SetMiterLimit(w);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 's': {
                p++;
                // strokeRect
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                StrokeRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'c': {
                p++;
                // clearRect
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                ClearRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'p': {
                p++;
                // clip
                GCanvasContext::ClipRegion();
                if (*p == ';') ++p;
                break;
            }
            case 'q': {
                p++;
                // resetClip
                ResetClip();
                if (*p == ';') ++p;
                break;
            }
            case 'o': {
                p++;
                // closePath
                ClosePath();
                if (*p == ';') ++p;
                break;
            }
            case 'g': {
                p++;
                // moveTo
                float tokens[2] = {0, 0};
                p = parseTokens(p, tokens);
                MoveTo(tokens[0], tokens[1]);
                break;
            }
            case 'G': {
                p++;
                char id[128];
                p = extractOneParameterFromCommand(id, p);
                const int texture_id = atoi(id);
                ++p;
                p = extractOneParameterFromCommand(id, p);
                const std::string repeat_mode = id;
                ++p;

                UsePatternRenderPipeline(texture_id, repeat_mode);
                break;
            }
            case 'H': {
                // createRadialGradient()
                p++;
                char str[128];
                float start[3], end[3];
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[0] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[1] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[2] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[0] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[1] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[2] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                const int stop_count = atoi(str);

                if (mCurrentState->mFillStyle != nullptr) {
                    delete mCurrentState->mFillStyle;
                }
                mCurrentState->mFillStyle =
                        new FillStyleRadialGradient(start, end);

                for (int i = 0; i < stop_count; ++i) {
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    float pos = atof(str);
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    std::string color = str;
                    dynamic_cast< FillStyleRadialGradient * >(
                            mCurrentState->mFillStyle)
                            ->AddColorStop(pos, color);
                }
                UseRadialGradientPipeline();

                break;
            }
            case 'D': {
                // createLinearGradient()
                p++;
                char str[128];
                GPoint start, end;
                p = extractOneParameterFromCommand(str, p);
                p++;
                start.x = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start.y = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end.x = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end.y = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                const int stop_count = atoi(str);

                if (mCurrentState->mFillStyle != nullptr) {
                    delete mCurrentState->mFillStyle;
                }
                mCurrentState->mFillStyle =
                        new FillStyleLinearGradient(start, end);

                for (int i = 0; i < stop_count; ++i) {
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    float pos = atof(str);
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    std::string color = str;
                    dynamic_cast< FillStyleLinearGradient * >(
                            mCurrentState->mFillStyle)
                            ->AddColorStop(pos, color);
                }
                UseLinearGradientPipeline();

                break;
            }
            case 'i': {
                p++;
                // lineTo
                float tokens[2] = {0, 0};
                p = parseTokens(p, tokens);
                LineTo(tokens[0], tokens[1]);
                break;
            }
            case 'u': {
                p++;
                // quadraticCurveTo
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                QuadraticCurveTo(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'z': {
                p++;
                // bezierCurveTo
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                BezierCurveTo(tokens[0], tokens[1], tokens[2], tokens[3],
                              tokens[4], tokens[5]);
                break;
            }
            case 'h': {
                p++;
                // arcTo
                float tokens[6] = {0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                ArcTo(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]);
                break;
            }
            case 'b': {
                p++;
                BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'n': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                FillRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'w': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                Rect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'L': {
                p++;
                Fill();
//                BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'x': {
                p++;
                Stroke();
//                BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'y': {
                p++;
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                Arc(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                    tokens[5]);
                break;
            }
            case 'B': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);

                SetGlobalCompositeOperation(GCompositeOperation(tokens[0]));
                break;
            }
            case 'A': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);
                mCurrentState->mTextAlign = GTextAlign(tokens[0]);
                break;
            }
            case 'E': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);
                mCurrentState->mTextBaseline = GTextBaseline(tokens[0]);
                break;
            }
            case 'T': { // fillText
                p++;

                int textLen = 0;
                float tokens[3] = {0, 0, 0};
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                p++;
                p = parseTokens(p, tokens);
                if (textLen > 0) {
                    strncpy(tmpText, pStart, textLen);
                    tmpText[textLen] = 0;
                    float maxWidth = tokens[2];
                    if (maxWidth < 0.0001) {
                        maxWidth = SHRT_MAX;
                    }
                    DrawText(tmpText, tokens[0], tokens[1], maxWidth, false);
                }

                break;
            }
            case 'U': { // strokeText
                p++;

                int textLen = 0;
                float tokens[3] = {0, 0, 0};
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                p++;
                p = parseTokens(p, tokens);
                if (textLen > 0) {
                    strncpy(tmpText, pStart, textLen);
                    tmpText[textLen] = 0;
                    float maxWidth = tokens[2];
                    if (maxWidth < 0.0001) {
                        maxWidth = SHRT_MAX;
                    }
                    DrawText(tmpText, tokens[0], tokens[1], maxWidth, true);
                }

                break;
            }
            case 'j': {
                p++;
                int textLen = 0;
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                if (textLen > 0) {
                    strncpy(tmpFont, pStart, textLen);
                    tmpFont[textLen] = 0;
                    if (mCurrentState->mFont != nullptr) {
                        delete mCurrentState->mFont;
                    }
                    mCurrentState->mFont = new GFontStyle(tmpFont);
                }
                if (*p == ';') ++p;

                break;
            }
            case 'P': // PutImageData
            {
                p++;
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens, 6);
                const char *begin_pos = p;

                while (*p && *p != ';') {
                    ++p;
                }
                int data_len = static_cast<int>(p - begin_pos);
                LOG_D("[executeRenderCommands] data_len:%d", data_len);

                PutImageData(begin_pos, data_len, tokens[0], tokens[1],
                             tokens[2], tokens[3], tokens[4], tokens[5]);

                if (*p == ';') ++p;
                break;
            }
            case 'R': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens, 4);
                std::string imageDataResult;
                GetImageData(tokens[0], tokens[1], tokens[2], tokens[3], true, imageDataResult);
                setSyncResult(imageDataResult);
                break;
            }
            default: {
                LOG_W("unknown error");
                LOG_W("command: %s", p);
                p = parseUnknown(p);
            }
        }
    }
    if (LOG_LEVEL_DEBUG == GetLogLevel()) {
        Save();
        ApplyTransform(1, 0, 0, 1, 0, mTyOffset);
        SetFillStyle(StrValueToColorRGBA("black"));
#ifdef ANDROID
        if (mCurrentState->mFont != nullptr) {
            delete mCurrentState->mFont;
        }
        mCurrentState->mFont = new GFontStyle("20px");
#endif
        char fpsString[20];
//        snprintf(fpsString, sizeof(fpsString), "G-Canvas fps:%.0f", mFps);
        snprintf(fpsString, sizeof(fpsString), "%.0f", mFps);
        DrawText(fpsString, 10, 25, 100, true);
        Restore();
    }

    SendVertexBufferToGPU();
}

void GCanvas::parseTokesOpt(float *tokens, const char **pp) {
    int iToken = 0;
    const char *position[10] = {nullptr};
    const char *p = *pp;
    position[iToken++] = p;
    for (; *p != ';'; p++) {
        if (',' == *p) {
            p++;
            position[iToken++] = p;
        }
    }
    for (int i = 0; i < iToken; i++) {
        tokens[i] = atof(position[i]);
    }
    *pp = p;
    return;
}

void GCanvas::calculateFPS() {
    ++mFrames;
    if (mFrames >= mFps || mFrames >= 60) {
#ifdef ANDROID
        clock_t now = clock();
        clock_t delta = now - mLastTime;
        if (delta >= CLOCKS_PER_SEC) {
            double dSeconds =
                    static_cast<double>(delta) / static_cast<double>(CLOCKS_PER_SEC);
            mFps = static_cast<float>(static_cast<double>(mFrames) / dSeconds);
            LOG_D("[calculateFPS] fps=%.1f", mFps);
        } else if (delta >= 0) {
            return; // skip if the interval is less than a second.
        }

        mFrames = 0;
        mMessages = 0;
        mLastTime = now;
        mMsgLength = 0;
#endif

#ifdef IOS

        static timeval tvLast = {0, 0};
        struct timeval tvNow;
        gettimeofday(&tvNow, NULL);

        if (tvNow.tv_sec > tvLast.tv_sec)
        {
            double delta = (tvNow.tv_sec - tvLast.tv_sec) +
                           (tvNow.tv_usec - tvLast.tv_usec) / 1000000;
            mFps = static_cast<float>(static_cast<double>(mFrames) / delta);

            mFrames = 0;
            mMessages = 0;
            tvLast = tvNow;
            mMsgLength = 0;
        }
#endif
    }
}


void
GCanvas::
drawFBO(std::string fboName, GCompositeOperation compositeOp, float sx, float sy, float sw,
        float sh, float dx, float dy, float dw, float dh) {
    if (!mIsFboSupported) {
        return;
    }

    if (nullptr == mCurrentState || nullptr == mCurrentState->mShader) {
        return;
    }

    Save();
    glViewport(mX, mY, mWidth, mHeight);

    GFrameBufferObject &fbo = mFboMap[fboName];

    UseDefaultRenderPipeline();

    glDisable(GL_STENCIL_TEST);

    SetGlobalCompositeOperation(compositeOp, compositeOp);

#ifdef DEBUG_DRAW
    GColorRGBA debugColor = StrValueToColorRGBA("green");
    mCurrentState->mShader->SetOverideTextureColor(0);
    mCurrentState->mShader->SetHasTexture(0);
    fbo.mFboTexture.Bind();
    
    SetTransformOfShader(GTransformMake(1, 0, 0, 1, dx, -dy));
    
    PushRectangle(-1, 1-2*dh, 2 * dw, 2 * dh, sx, 1 - sh - sy, sw, sh, debugColor);
    SendVertexBufferToGPU();

#endif

    GColorRGBA color = StrValueToColorRGBA("white");
    mCurrentState->mShader->SetOverideTextureColor(0);
    mCurrentState->mShader->SetHasTexture(1);
    fbo.mFboTexture.Bind();


    PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;

    if (HasClipRegion()) {
        glEnable(GL_STENCIL_TEST);
    }


    glViewport(0, 0, mWidth, mHeight);
    Restore();
}


void GCanvas::Render(const char *renderCommands, int length) {
    if (mContextLost) {
        return;
    }
    LOG_D("GCanvas::Render: mContextType: %d", mContextType);
    if (0 != mContextType) {
        if (mRenderCount < g_clear_color_time) {
            LOG_D("clearScreen");
            ClearScreen(mClearColor);
        }
        if (length > 0) {
            calculateFPS();
            LOG_D("GCanvas::Render:[WebGL] renderCommands:%s", renderCommands);
            executeWebGLCommands(renderCommands, length);
            if (mRenderCount < g_clear_color_time) {
                ClearScreen(mClearColor);
                ++mRenderCount;
            }
        }
    } else {
        if (nullptr == mCurrentState || nullptr == mCurrentState->mShader) {
            return;
        }

        BindFBO();
        if (length > 0) {
            calculateFPS();
            LOG_D("GCanvas::Render:[2D] renderCommands:%s", renderCommands);
            execute2dCommands(renderCommands, length);
        }
        UnbindFBO();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        drawFBO(DefaultFboName);
    }

    // process any capture requests
//    int i = 0;
//    while (!mCaptureParams.IsEmpty())
//    {
//        bool isError = true;
//        LOG_D("GCanvas::Render, about to capture");
//        isError = captureGLLayer(mCaptureParams[i]);
//
//        // create callback if we have one
//        AddCallback(mCaptureParams[i]->callbackID, mCaptureParams[i]->fileName,
//                    isError);
//
//        // delete our capture command
//        mCaptureParams.RemoveAt(i++);
//        LOG_D("CANVAS::Render, capture success, left in queue: %d",
//              mCaptureParams.GetSize());
//    }
}

void GCanvas::DrawImage(int textureId, float sx, float sy, float sw, float sh,
                        float dx, float dy, float dw, float dh) {
    LOG_D("GCanvas::DrawImage texture %d", textureId);
    const TextureGroup *ptr_group = mTextureMgr.Get(textureId);
    if (NULL == ptr_group) {
        LOG_E("GCanvas::DrawImage texture %d is NULL", textureId);
        return;
    }
    const TextureGroup &group = *ptr_group;

    if (!group.IsSplit()) {
#ifdef IOS
        std::map< int, int >::iterator iter = mOfflineTextures.find(textureId);
        if (iter != mOfflineTextures.end())
        {
            Scale(1, -1);
            mOfflineTextures.erase(iter);
        }
#endif
        DrawImage1(group.mVecTexture[0]->GetWidth(),
                   group.mVecTexture[0]->GetHeight(),
                   group.mVecTexture[0]->GetGlID(), sx, sy, sw, sh, dx, dy, dw,
                   dh);
    } else {
        int start_col = sx / group.mTileWidth;
        int stop_col =
                (sx + sw - 1) / group.mTileWidth; // length is start with 1,so need
        // to minus 1 before check
        int start_row = sy / group.mTileHeight;
        int stop_row = (sy + sh - 1) / group.mTileHeight;

        int block_count = (group.mSrcWidth - 1) / group.mTileWidth +
                          1; // spilt block per row

        LOG_D("[GCanvas::drawImage] row:[%d~%d], col:[%d~%d]", start_row,
              stop_row, start_col, stop_col);

        float scale_h = dh / sh;
        float scale_w = dw / sw;
        for (int r = start_row; r <= stop_row; ++r) {
            int y1 = std::max(r * group.mTileHeight, static_cast<int>(sy));
            int y2 = std::min((r + 1) * group.mTileHeight, static_cast<int>(sy + sh));
            float new_h = (y2 - y1) * scale_h;

            for (int c = start_col; c <= stop_col; ++c) {
                int index = r * block_count + c;
                int x1 = std::max(c * group.mTileWidth, static_cast<int>(sx));
                int x2 = std::min((c + 1) * group.mTileWidth, static_cast<int>(sx + sw));
                LOG_D("[GCanvas::drawImage] rc:(%d, %d), x:(%d, %d), y:(%d, %d)",
                      r, c, x1, x2, y1, y2);

                DrawImage1(group.mVecTexture[index]->GetWidth(),
                           group.mVecTexture[index]->GetHeight(),
                           group.mVecTexture[index]->GetGlID(),
                           x1 % group.mTileWidth, y1 % group.mTileHeight,
                           x2 - x1, y2 - y1, dx + (x1 - sx) * scale_w,
                           dy + (y1 - sy) * scale_h, (x2 - x1) * scale_w,
                           new_h);
            }
        }
    }
}

void GCanvas::FillRect(float x, float y, float w, float h) {
//    GColorRGBA color = BlendFillColor(this);
//    PushRectangle(x, y, w, h, 0, 0, 1, 1, color);
    GCanvasContext::FillRect(x, y, w, h);
}


//for string cmd
void GCanvas::UsePatternRenderPipeline(int textureListId, int textureWidth, int textureHeight,
                                       const std::string &pattern, bool isStroke) {
    SendVertexBufferToGPU();

#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif

#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("PATTERN");
#endif

    if (nullptr == mCurrentState->mShader) {
        return;
    }
    mCurrentState->mShader->Bind();

    GFillStyle *style = isStroke ? mCurrentState->mStrokeStyle : mCurrentState->mFillStyle;
    if (style != nullptr) {
        delete style;
        style = nullptr;
    }

    if (textureListId >= 0 && pattern != "") {
        style = new FillStylePattern(textureListId, pattern);
        if (isStroke) {
            mCurrentState->mStrokeStyle = style;
        } else {
            mCurrentState->mFillStyle = style;
        }
        mCurrentState->mShader->SetRepeatMode(
                dynamic_cast< FillStylePattern * >(style)->GetPattern());

        mCurrentState->mFillColor = StrValueToColorRGBA("white");
        mCurrentState->textureId = textureListId;
        mCurrentState->mShader->SetTextureSize(textureWidth, textureHeight);
    }
}

//for string cmd
void GCanvas::UsePatternRenderPipeline(int textureListId,
                                       const std::string &pattern, bool isStroke) {
    SendVertexBufferToGPU();

#ifdef IOS
    mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif

#ifdef ANDROID
    mCurrentState->mShader = mShaderManager->programForKey("PATTERN");
#endif

    mCurrentState->mShader->Bind();
    if (mCurrentState->mFillStyle != nullptr) {
        delete mCurrentState->mFillStyle;
        mCurrentState->mFillStyle = nullptr;
    }
    if (textureListId >= 0 && pattern != "") {
        mCurrentState->mFillStyle =
                new FillStylePattern(textureListId, pattern);
        mCurrentState->mShader->SetRepeatMode(
                dynamic_cast< FillStylePattern * >(mCurrentState->mFillStyle)
                        ->GetPattern());
        const Texture *texture = getTextureWithOneImage(
                mCurrentState->mFillStyle->GetTextureListID());
        if (texture) {
            texture->Bind();
            mCurrentState->mShader->SetTextureSize(texture->GetWidth(),
                                                   texture->GetHeight());
        }
    }
    if (mCurrentState != nullptr) {
        SetTransformOfShader(mCurrentState->mTransform);
    }
}

void GCanvas::parseSetTransForTextform(
        float v1, float v2,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    GTransform t;
    switch (parseMode) {
        case SCALE:
            t.a = v1;
            t.d = v2;
            break;
        case TRANSLATE:
            t.tx = v1;
            t.ty = v2;
            break;
        default: ASSERT(0);
            break;
    }

    if (concat) {
        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        *transOut = t;
    }
}

void GCanvas::DrawText(const char *text, float x, float y, float maxWidth,
                       bool isStroke, int strLength) {
    if (strLength == 0) {
        strLength = static_cast<int>(strlen(text));
    }

    DrawTextWithLength(text, strLength, x, y, isStroke, maxWidth);

}

void GCanvas::DrawTextWithLength(const char *text, int strLength, float x, float y,
                                 bool isStroke, float maxWidth) {
    const GCompositeOperation old_op = mCurrentState->mGlobalCompositeOp;
    SetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER);

    //scaleWidth
    float measureWidth = execMeasureTextWidth(text);
    float scaleWidth = 1.0;
    if (measureWidth > maxWidth) {
        scaleWidth = maxWidth / measureWidth;
    }

#ifdef ANDROID

    Utf8ToUCS2 *lbData = new Utf8ToUCS2(text, strLength);

    if (mHiQuality) {
        //  theory
        //  :http://confluence.taobao.ali.com/pages/viewpage.action?pageId=241928616
        this->Save();
        GTransform action;
        action.a = 1, action.b = 0, action.c = 0, action.d = 1, action.tx = 0,
        action.ty = mTyOffset;
        ApplyTransform(1, 0, 0, 1, 0, mTyOffset);
        float scale = 1 / this->mDevicePixelRatio;
        parseSetTransForTextform(x * (1 - scale), y * (1 - scale), TRANSLATE,
                                 true, action, &action);
        ApplyTransform(action.a, action.b, action.c, action.d, action.tx,
                       action.ty);
        parseSetTransForTextform(scale, scale, SCALE, true, action, &action);
        ApplyTransform(action.a, action.b, action.c, action.d, action.tx,
                       action.ty);

        GCanvasContext::FillText(lbData->ucs2, lbData->ucs2len, x, y,
                                 isStroke);

        this->Restore();
    } else {
        GCanvasContext::FillText(lbData->ucs2, lbData->ucs2len, x, y,
                                 isStroke, scaleWidth);
    }

    delete lbData;

#endif

#ifdef IOS
    GCanvasContext::FillText((const unsigned short *)text,
                              (unsigned int)strLength, x, y, isStroke, scaleWidth);
#endif

    SetGlobalCompositeOperation(old_op);
}

void GCanvas::SetTyOffsetFlag(bool flag) {
    mTyOffsetFlag = flag;
    if (mTyOffsetFlag) {
        mTyOffset = mHeight;
        mCurrentTransform.ty = mTyOffset;
    }
}

// called from JNI or Obj-C to indicate we want to readback the GL layer into a
// file on the next render
//void GCanvas::QueueCaptureGLLayer(int x, int y, int w, int h,
//                                  const char *callbackID, const char *fn)
//{
//    CaptureParams *params = new CaptureParams(x, y, w, h, callbackID, fn);
//    mCaptureParams.Append(&params, 1);
//    LOG_D("Canvas.cpp::QueueCaptureGLLayer - queued");
//}

void
GCanvas::PutImageData(
        const char *imageData,
        int dataLength,
        float
        dx,
        float dy,
        float sw,
        float sh,
        float dw,
        float dh) {
    LOG_D("[PutImageData] xy=(%f, %f), src_wh=(%f, %f), dest_wh=(%f, %f)",

          dx, dy, sw, sh, dw, dh);

    std::string &pixels_data = mTempStr;
    if ((unsigned int) dataLength > pixels_data.length())
        pixels_data.resize(dataLength);

    GLubyte *pixels = (GLubyte *) (pixels_data.c_str());

    gcanvas::Base64DecodeBuf(reinterpret_cast<char *>(pixels), imageData, dataLength);

    GLuint glID;
    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw, sh, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);

    DrawImage1(sw, sh, glID, 0, 0, sw, sh, dx, dy, dw, dh);
    if (!mContextLost) glDeleteTextures(1, (const GLuint *) (&glID));
}

void
GCanvas::GetImageData(int
                      x, int
                      y, int
                      w, int
                      h, bool
                      base64Encode,
                      std::string &pixelsData) {
    LOG_D("GCanvas.cpp::getImageData begin... xy=(%d, %d), wh=(%d, %d)", x,

          y, w, h);

    // bounds check the parameters
    // get the dimensions of the current viewport
    int results[4];
    glGetIntegerv(GL_VIEWPORT, results);

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (-1 == w) w = results[2];
    if (-1 == h) h = results[3];
    if ((x + w) > results[2]) {
        x = 0;
        w = results[2];
    }
    if ((y + h) > results[3]) {
        y = 0;
        h = results[3];
    }

    // flip y axis to be in openGL lower left origin
    y = results[3] - y - h;
    LOG_D("GCanvas.cpp::getImageData bounds... xy=(%d, %d), wh=(%d, %d)", x,
          y, w, h);

    // use glGetPixels to get the bits from the current frame buffer
    // Make the BYTE array, factor of 3 because it's RBG.
    int buf_size = 4 * w * h;
    std::string *ptr_data = NULL;
    if (base64Encode) {
        ptr_data = &mTempStr;
        if ((unsigned int) buf_size > ptr_data->length())
            ptr_data->resize(buf_size);
    } else {
        ptr_data = &pixelsData;
        ptr_data->resize(buf_size);
    }

    unsigned char *pixels = (unsigned char *) ptr_data->c_str();

    glFinish();
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    gcanvas::FlipPixel(pixels, w, h);

    if (base64Encode) {
        pixelsData.resize(gcanvas::Base64EncodeLen(buf_size));
        gcanvas::Base64EncodeBuf((char *) (pixelsData.c_str()), ptr_data->c_str(),
                                 buf_size);
    }
    LOG_D("GCanvas.cpp::getImageData end...[%d]=%s", buf_size,
          pixelsData.c_str());
}

// called from within render when QueueCaptureGLLayer has been called
//bool GCanvas::captureGLLayer(CaptureParams *params)
//{
//    return false;
//    // get the dimensions of the current viewport
//    int results[4];
//    glGetIntegerv(GL_VIEWPORT, results);
//
//    // bounds check the parameters
//    int x = (params->x < 0) ? 0 : params->x;
//    int y = (params->y < 0) ? 0 : params->y;
//    int width = (params->width == -1) ? results[2] : params->width;
//    int height = (params->height == -1) ? results[3] : params->height;
//    if ((x + width) > results[2])
//    {
//        x = 0;
//        width = results[2];
//    }
//    if ((y + height) > results[3])
//    {
//        y = 0;
//        height = results[3];
//    }
//    // flip y axis to be in openGL lower left origin
//    y = results[3] - y - height;
//
//    // use glGetPixels to get the bits from the current frame buffer
//    // Make the BYTE array, factor of 3 because it's RBG.
//    GLubyte *pixels = new GLubyte[4 * width * height];
//    if (!pixels)
//    {
//        LOG_E("Canvas::captureGLLayer Unable to allocate buffer");
//
//        return true;
//    }
//    glFinish();
//    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//
//    // taken from example here:
//    // http://www.codesampler.com/2010/11/02/introduction-to-opengl-es-2-0/
//    // Flip and invert the PNG image since OpenGL likes to load everything
//    // backwards from what is considered normal!
//    int halfTheHeightInPixels = height / 2;
//    int heightInPixels = height;
//
//    // Assuming RGBA for 4 components per pixel.
//    int numColorComponents = 4;
//
//    // Assuming each color component is an unsigned char.
//    int widthInChars = width * numColorComponents;
//
//    unsigned char *top = nullptr;
//    unsigned char *bottom = nullptr;
//    unsigned char temp = 0;
//
//    for (int h = 0; h < halfTheHeightInPixels; ++h)
//    {
//        top = pixels + h * widthInChars;
//        bottom = pixels + (heightInPixels - h - 1) * widthInChars;
//
//        for (int w = 0; w < widthInChars; ++w)
//        {
//            // Swap the chars around.
//            temp = *top;
//            *top = *bottom;
//            *bottom = temp;
//
//            ++top;
//            ++bottom;
//        }
//    }
//
//    // use loadpng library to write the rawbits to png
//    // Encode the image
//    int error = PngLoader::Instance().EncodePng(params->fileName, pixels, width,
//                                                height);
//
//    // Free memory
//    // delete pixels;
//    delete[] pixels;
//
//    // if there's an error, display it
//    if (error)
//    {
//        LOG_D("Canvas::captureGLLayer Error %d", error);
//        params->fileName[CaptureParams::ALLOCATED - 1] = 0;
//        // strcpy(params->fileName, lodepng_error_text(error));
//        return true;
//    }
//    else
//    {
//        LOG_D("Canvas::captureGLLayer png written: %s", params->fileName);
//
//        return false;
//    }
//}

#ifdef ANDROID

// Get the front of the callback queue
Callback *GCanvas::GetNextCallback() {
    if (!mCallbacks.IsEmpty())
        return mCallbacks[0];
    else
        return nullptr;
}

// delete the front of the callback queue
void GCanvas::PopCallbacks() {
    if (!mCallbacks.IsEmpty()) mCallbacks.RemoveAt(0);
}

// push to the end of the callback queue
void GCanvas::AddCallback(const char *callbackID, const char *result,
                          bool isError) {
    if (callbackID != nullptr && *callbackID != '\0') {
        Callback *cb = new Callback(callbackID, result, isError);
        mCallbacks.Append(&cb, 1);
        LOG_D("Canvas::AddCallback - Callback created: %s, %s, %d",
              callbackID, result, isError);
    }
}

#endif

void GCanvas::setSyncResult(std::string result) {
    mResult = result;
}

#ifdef ANDROID

std::string GCanvas::exeSyncCmd(int type, const char *&args) {

    if (mContextLost) {
        return "";
    }

    LOG_D("[GCanvas::exeSyncCmd] type=%d,context type=%d,TODATAURL=%d,width=%d,height=%d,args=%s\n",
          type, mContextType, TODATAURL, mWidth, mHeight, args);


    if (0 == mContextType) {
    }


    return "";
}

int GCanvas::getCmdType(int type) {
    return type >> 30;
}

int GCanvas::getSyncAttrib(int type) {
    return (type >> 29) && 0x1;
}

int GCanvas::getOpType(int type) {
    return type & 0x1fff;
}

void GCanvas::signalUpGLthread() {

//    JNIEnv *je = GetEnv();
//    if(je == NULL) {
//        LOG_D("attach environment failed.");
//    }
//
//    jclass cls = je->FindClass("com/taobao/gcanvas/GCanvas");
//    jmethodID render = je->GetMethodID(cls,"requestRender", "()V");
//    jobject ins = je->NewObject(cls,render);
//
//    je->CallVoidMethod(ins,render);

    return;
}


std::string GCanvas::canvasProc(int op, int sync, std::string args) {

    LOG_D("canvasProc start.");

    if (args.c_str() == nullptr || args.length() == 0 ||
        !strcmp(args.c_str(), "")) {
        return "";
    }

    LOG_D("canvasProc,length = %d,args = %s\n", args.length(),
          args.c_str());



//    switch (op) {
//        case OP_RENDER:
//        {
    this->Render(args.c_str(), (int) args.length());
//            break;
//        }
//        default:{
//            break;
//        }
//    }

    LOG_D("canvasProc finished.");

    return "";
}

std::string GCanvas::webglProc(int op, int sync, std::string args) {


//    LOG_D("webglProc start.");

    if (args.c_str() == nullptr || args.length() == 0 ||
        !strcmp(args.c_str(), "")) {
        return nullptr;
    }


//    LOG_D("webglProc,args = %s\n", args.c_str());


    const char *commands = args.c_str();
    executeWebGLCommands(commands, (int) args.length());

//    switch (op) {
//        case OP_RENDER:
//        {
//            this->Render(args.c_str(), (int)args.length());
//            break;
//        }
//        default:
//        {
//            break;
//        }
//    }


//    LOG_D("webglProc finished.");

    return "";
}

std::string GCanvas::vulkanProc(int op, int sync, std::string args) {
    switch (op) {

    }

    return "";
}

std::string GCanvas::metalProc(int op, int sync, std::string args) {
    switch (op) {

    }

    return "";
}

const char *GCanvas::CallNative(int type, const std::string &args) {
    if (mContextLost) {
        LOG_E("context lost, return");
        return "";
    }

    if (mExit) {
        LOG_E("request exit, return");
        return "";
    }

    mResult = "";

    int sync = getSyncAttrib(type);

    struct GCanvasCmd *p = new struct GCanvasCmd();
    p->type = type;
    p->args = args;

    mCmdQueue.push(p);

    signalUpGLthread();


    if (sync == SYNC) {
        LOG_D("call native sync call, start wait. cmd is %s", args.c_str());
        gcanvas::waitUtilTimeout(&mSyncSem, GCANVAS_TIMEOUT);
        LOG_D("call native sync result: %s", mResult.c_str());
        return mResult.c_str();
    }

    return nullptr;
}

void GCanvas::clearCmdQueue() {

    GCanvasManager *theManager = GCanvasManager::GetManager();
    theManager->clearQueueByContextId(mContextId);
    while (!mCmdQueue.empty()) {
        struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *> (mCmdQueue.front());
        mCmdQueue.pop();
        delete p;
    }

    while (!mBitmapQueue.empty()) {
        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd *> (mBitmapQueue.front());
        mBitmapQueue.pop();
        delete p;
    }

}

void GCanvas::QueueProc(std::queue<struct GCanvasCmd *> *queue) {
    LOG_D("enter QueueProc");
    if (queue == nullptr) {
        return;
    }
    LOG_D("queue is not null! %d", queue->size());
    if (!queue->empty()) {
        struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *>(queue->front());
        int type = p->type;
        int cmd = getCmdType(type);
        int op = getOpType(type);
        int sync = getSyncAttrib(type);

        std::string args = p->args;

        LOG_D("start to process queue cmd.");

        switch (cmd) {
            case CANVAS: {
                canvasProc(op, sync, args);
                break;
            }
            case WEBGL: {
                webglProc(op, sync, args);
                break;
            }
            case VULKAN: {
                vulkanProc(op, sync, args);
                break;
            }
            case METAL: {
                metalProc(op, sync, args);
                break;
            }
            default: {
                break;
            }
        }

        if (op == 1) {
            setRefreshFlag(true);
        }
//        if (sync == SYNC) {
//            LOG_D("LinkNativeGLProc,sync proc result=%s\n", mResult.c_str());
//#ifdef ANDROID
//            sem_post(&mSyncSem);
//#endif
//        }

        queue->pop();

        delete p;

        if (sync == SYNC) {
            setSyncFlag();
        }
    }
}

void GCanvas::LinkNativeGLProc() {
    if (mContextLost) {
        LOG_D("in LinkNativeGLProc mContextLost");
        clearCmdQueue();
        return;
    }

    GCanvasManager *theManager = GCanvasManager::GetManager();
    std::queue<struct GCanvasCmd *> *queue = theManager->getQueueByContextId(
            mContextId);
    if (queue != nullptr) {
        LOG_D("in LinkNativeProc QueueProc queue");
        QueueProc(queue);
    }
//    LOG_D("in LinkNativeProc QueueProc mCmdQueue");
    QueueProc(&mCmdQueue);

    if (queue != nullptr) {
        delete queue;
    }
}

void GCanvas::setSyncFlag() {
    mSync = true;
}

void GCanvas::setThreadExit() {
    mExit = true;
}

void GCanvas::setContextLost(bool lost) {
    mContextLost = lost;
}

void GCanvas::finishProc() {
    if (mSync == SYNC) {
        LOG_D("LinkNativeGLProc,sync proc result=%s\n", mResult.c_str());
        sem_post(&mSyncSem);
        mSync = false;
    }
}

void GCanvas::setRefreshFlag(bool refresh) {
    LOG_D("start to set refresh flag in gcanvas.");
}


void GCanvas::addBitmapQueue(struct BitmapCmd *p) {
    mBitmapQueue.push(p);
}

void GCanvas::bindTexture(struct BitmapCmd cmd) {
//    while (!mBitmapQueue.empty()) {
//        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd * >(mBitmapQueue.front());
    GLuint glID;
    LOG_D("DO BIND TEXTURE. context type = %d", mContextType);

    //step 1:bindtexture
    if (mContextType == 0) {
        glGenTextures(1, &glID);
        glBindTexture(GL_TEXTURE_2D, glID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    }

    glTexImage2D(cmd.target, cmd.level, cmd.interformat, cmd.width,
                 cmd.height, 0, cmd.format,
                 cmd.type, cmd.Bitmap);

    //step 2:save textureid
    if (mContextType == 0) {
        glGenerateMipmap(GL_TEXTURE_2D);
        AddTexture(cmd.id, glID, cmd.width, cmd.height);
    }
}

void GCanvas::bindTexture(GTexture *texture) {


    if (mContextType == 0) {
        texture->Bind();
//        AddTexture(texture->GetTextureID(), texture->GetTextureID(), texture->GetWidth(), texture->GetHeight());

    }


}

void GCanvas::texSubImage2D(struct BitmapCmd cmd) {
//    while (!mBitmapQueue.empty()) {
//        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd * >(mBitmapQueue.front());

    LOG_D("start to texSubImage2D in grenderer.");

//        glTexSubImage2D(p->target, p->level, p->µ, p->yoffset,
//                        p->width, p->height,
//                        p->format,
//                        p->type, p->Bitmap);

    glTexSubImage2D(cmd.target, cmd.level, cmd.xoffset, cmd.yoffset,
                    cmd.width, cmd.height,
                    cmd.format,
                    cmd.type, cmd.Bitmap);
}


bool GCanvas::continueProcess() {

    if (mContextLost) {
        return false;
    }

    return !mBitmapQueue.empty() || !mCmdQueue.empty();

}

Callback::Callback(const char *id, const char *res,
                   bool error) {
    strncpy(callbackId, id, ALLOCATED - 1);
    callbackId[ALLOCATED - 1] = 0;

    strncpy(result, res, ALLOCATED - 1);
    result[ALLOCATED - 1] = 0;
    isError = error;
}

#endif

//-------------------------------------------------------------------------------------------------

//CaptureParams::CaptureParams()
//{
//    LOG_D("CaptureParams::CaptureParams()");
//
//    int results[4];
//    glGetIntegerv(GL_VIEWPORT, results);
//    x = results[0];
//    y = results[1];
//    width = results[2];
//    height = results[3];
//    const char *defName = "screenshot.png";
//    strncpy(fileName, defName, ALLOCATED - 1);
//    fileName[ALLOCATED - 1] = 0;
//}
//
//CaptureParams::CaptureParams(int
//                             xPos, int
//                             yPos, int
//                             w, int
//                             h,
//                             const char *callback,
//                             const char *fn)
//{
//    LOG_D("CaptureParams::CaptureParams(int,int,int,int const char*, const "
//                  "char *)");
//
//    x = xPos;
//    y = yPos;
//    width = w;
//    height = h;
//
//    strncpy(callbackID, callback, ALLOCATED - 1);
//    callbackID[ALLOCATED - 1] = 0;
//    strncpy(fileName, fn, ALLOCATED - 1);
//    fileName[ALLOCATED - 1] = 0;
//
//    LOG_D("CaptureParams::CaptureParams(int,int,int,int const char*, const "
//                  "char *) - success");
//
//}
//
