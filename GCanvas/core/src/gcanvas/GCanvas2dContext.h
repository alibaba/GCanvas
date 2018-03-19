/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GCanvasContext__
#define __GCanvas__GCanvasContext__

#include <iostream>
#ifndef _WIN32

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#else
#include <GLES2/gl2.h>
#endif
#include "GShader.h"
#include "GPath.h"
#include "GTransform.h"
#include "GFillStyle.h"

#ifdef ANDROID
#include "GFont.h"
#include "GSystemFontInformation.h"
#include "GShaderManager.h"
#endif



#include "GTextDefine.h"
#include "GTexture.h"

const GColorRGBA DEFAULT_CLEAR_COLOR =
    StrValueToColorRGBA("transparent_white");

typedef enum {
    LINE_CAP_BUTT = 0,
    LINE_CAP_ROUND,
    LINE_CAP_SQUARE
} GLineCap;

typedef enum {
    LINE_JOIN_MITER = 0,
    LINE_JOIN_BEVEL,
    LINE_JOIN_ROUND
} GLineJoin;

typedef enum {
    DIRECTION_INHERIT,
    DIRECTION_LEFT_TO_RIGHT,
    DIRECTION_RIGHT_TO_LEFT
} GDirection;

typedef enum {
    COMPOSITE_OP_SOURCE_OVER,
    COMPOSITE_OP_LIGHTER,
    COMPOSITE_OP_DARKER,
    COMPOSITE_OP_DESTINATION_OUT,
    COMPOSITE_OP_DESTINATION_OVER,
    COMPOSITE_OP_SOURCE_ATOP,
    COMPOSITE_OP_XOR,
    COMPOSITE_OP_REPLACE,
    COMPOSITE_OP_ALPHA,
} GCompositeOperation;

static const struct
{
    GLenum source;
    GLenum destination;
} GCompositeOperationFuncs[] = {
    {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA},
    {GL_SRC_ALPHA, GL_ONE},
    {GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA},
    {GL_ZERO, GL_ONE_MINUS_SRC_ALPHA},
    {GL_SRC_ALPHA_SATURATE, GL_DST_ALPHA},
    {GL_DST_ALPHA, GL_ZERO},
    {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA},
    {GL_ONE, GL_ZERO},
    {GL_SRC_ALPHA, GL_DST_ALPHA}};

class GCanvasState
{
public:
    GCanvasState();
    GCanvasState(const GCanvasState &state);
    GCanvasState &operator=(const GCanvasState &state);
    ~GCanvasState();

    GCompositeOperation mGlobalCompositeOp;
    GColorRGBA mFillColor;
    GColorRGBA mStrokeColor;
    float mGlobalAlpha;
    float mLineWidth;
    GLineCap mLineCap;
    GLineJoin mLineJoin;
    float mMiterLimit;
#ifdef ANDROID
    gcanvas::GFontStyle *mFont;
#endif

#ifdef IOS
    std::string mFontStyle;
#endif

    GTextAlign mTextAlign;
    GTextBaseline mTextBaseline;
    GTransform mTransform;
    GTransform mClipTransform;
    GPath *mClipPath;
    GShader *mShader;
    GFillStyle *mFillStyle;
};

typedef void (*G2DFontDrawTextFunc)(const unsigned short *text, unsigned int textLength, float x,
                                    float y, bool isStroke, GCanvasContext *canvasContext);
typedef void (*GWebGLTxtImage2DFunc)(GLenum target, GLint level, GLenum internalformat,
                                     GLenum format, GLenum type,  const char *src);
typedef void (*GWebGLTxtSubImage2DFunc)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                        GLenum format, GLenum type, const char *src);
typedef void (*GWebGLBindToGLKViewFunc)(std::string contextId);



class GCanvasContext
{
public:
    static const int GCANVAS_STATE_STACK_SIZE = 16;
    static const int GCANVAS_VERTEX_BUFFER_SIZE = 2048;

    GCanvasContext(short w, short h);
    virtual ~GCanvasContext();

    void SetDevicePixelRatio(const float ratio);
    void CalculateProjectTransform();
    bool InitializeGLEnvironment();
    void ResetStateStack();
    void BindVertexBuffer();
    void BindPositionVertexBuffer();
    GLuint PositionSlot();
    void SaveRenderPipeline();
    void RestoreRenderPipeline();
    void UseDefaultRenderPipeline();
    void UsePatternRenderPipeline();
    void UseLinearGradientPipeline();
    void UseRadialGradientPipeline();
    void SetTexture(const GTexture *texture);
    void ClearGeometryDataBuffers();
    void SendVertexBufferToGPU(const GLenum geometry_type = GL_TRIANGLES);
    void PushTriangle(GPoint v1, GPoint v2, GPoint v3, GColorRGBA color);
    void PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                  GColorRGBA color);
    void PushRectangle(float x, float y, float w, float h, float tx, float ty,
                       float tw, float th, GColorRGBA color);
    void PushPoints(const std::vector< GPoint > &points, GColorRGBA color);
    short Width() { return mWidth; }
    short Height() { return mHeight; }
    void Save();
    bool Restore();
    void Scale(float x, float y);
    void Rotate(float angle);
    void Translate(float x, float y);
    void ApplyTransform(float m11, float m12, float m21, float m22, float dx,
                        float dy);
    void SetTransformOfShader(const GTransform &trans);
    void ResetTransform();
    float GlobalAlpha() const
    {
        return mCurrentState->mGlobalAlpha;
    }
    void SetGlobalAlpha(float a);
    GCompositeOperation GlobalCompositeOperation();
    void SetGlobalCompositeOperation(GCompositeOperation op);

    GColorRGBA StrokeStyle() const
    {
        return mCurrentState->mStrokeColor;
    }
    void SetStrokeStyle(GColorRGBA c);
    GColorRGBA GetFillStyle() const
    {
        return mCurrentState->mFillColor;
    }
    void SetFillStyle(GColorRGBA c);
    void SetClearColor(const GColorRGBA &c);
    GColorRGBA GetClearColor() const { return mClearColor; }
    void ClearScreen(const GColorRGBA &color);
    void ClearRect(float x, float y, float w, float h);
    void FillRect(float x, float y, float w, float h);
    void StrokeRect(float x, float y, float w, float h);
    void Fill();
    void Stroke();
    void ClipRegion();
    void ResetClip();
    bool HasClipRegion() const { return mHasClipRegion; }
    void SetClipFlag(const bool flag) { mHasClipRegion = flag; }

    void FillText(const unsigned short *text, unsigned int textLen, float x,
                  float y, bool isStroke);
    void DrawImage1(float w, float h, int TextureId, float sx, float sy,
                    float sw, float sh, float dx, float dy, float dw, float dh);

    float LineWidth() const
    {
        return mCurrentState->mLineWidth;
    }
    void SetLineWidth(float w) { mCurrentState->mLineWidth = w; }
    GLineCap LineCap() const
    {
        return mCurrentState->mLineCap;
    }
    void SetLineCap(GLineCap cap) { mCurrentState->mLineCap = cap; }
    GLineJoin LineJoin() const
    {
        return mCurrentState->mLineJoin;
    }
    void SetLineJoin(GLineJoin join) { mCurrentState->mLineJoin = join; }
    float MiterLimit() const
    {
        return mCurrentState->mMiterLimit;
    }
    void SetMiterLimit(float limit) { mCurrentState->mMiterLimit = limit; }
    GTextAlign TextAlign() const { return mCurrentState->mTextAlign; }
    void SetTextAlign(GTextAlign align) { mCurrentState->mTextAlign = align; }
    GTextBaseline TextBaseline() const
    {
        return mCurrentState->mTextBaseline;
    }
    void SetTextBaseline(GTextBaseline baseLine)
    {
        mCurrentState->mTextBaseline = baseLine;
    }
    GDirection Direction() const { return DIRECTION_LEFT_TO_RIGHT; }
    void SetDirection(GDirection direction) {}
    void BeginPath();
    void ClosePath();
    void MoveTo(float x, float y);
    void LineTo(float x, float y);
    void QuadraticCurveTo(float cpx, float cpy, float x, float y);
    void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
    void ArcTo(float x1, float y1, float x2, float y2, float radius);
    void ArcTo(float x1, float y1, float x2, float y2, float radiusX,
               float radiusY, float rotation);
    void Rect(float x, float y, float w, float h);
    void Arc(float x, float y, float radius, float startAngle, float endAngle,
             bool anticlockwise = false);
    void DeleteFBO();
    void InitFBO();
    void BindFBO();
    void UnbindFBO();
#ifdef ANDROID
    bool LoadFace(FT_Library *library, const char *filename, const float size,
                  FT_Face *face);
    bool IsGlyphExistedInFont(const wchar_t charCode, const float size,
                              std::string filename);
    char *GetDefaultFont();
//    char *GetDefaultFallbackFont();
    char *TryDefaultFont(const wchar_t charCode, const float size,
                         const char *currentFontLocation);
    char *TryDefaultFallbackFont(const wchar_t charCode, const float size,
                                 const char *currentFontLocation);
    char *TryOtherFallbackFont(const wchar_t charCode, const float size,
                               const char *currentFontLocation);
    char *TrySpecFont(const wchar_t charCode, const float size,
                      const char *currentFontLocation,
                      const char *specFontFile);
    void FillTextInternal(GFont *font, wchar_t text, float &x, float y);
    GFont *GetFontByCharCode(wchar_t charCode);
#endif
    bool IsFboSupport() const { return mIsFboSupported; }
    void SetHiQuality(bool isHiQuality) { mHiQuality = isHiQuality; }
    void SetG2DFontDrawTextFunc(G2DFontDrawTextFunc func)
    {
        g2d_font_draw_text_func_ = func;
    }
    GCanvasState *GetCurrentState() { return mCurrentState; }

    GWebGLTxtImage2DFunc GetGWebGLTxtImage2DFunc()
    {
        return gwebgl_txtImage_2d_func_;
    }
    void SetGWebGLTxtImage2DFunc(GWebGLTxtImage2DFunc func)
    {
        gwebgl_txtImage_2d_func_ = func;
    }
    
    GWebGLTxtSubImage2DFunc GetGWebGLTxtSubImage2DFunc()
    {
        return gwebgl_txtsubImage_2d_func_;
    }
    void SetGWebGLTxtSubImage2DFunc(GWebGLTxtSubImage2DFunc func)
    {
        gwebgl_txtsubImage_2d_func_ = func;
    }
#ifdef IOS
    GWebGLBindToGLKViewFunc GetGWebGLBindToGLKViewFunc()
    {
        return gwebgl_bind_glkview_func_;
    }
    void SetGWebGLBindToGLKViewFunc(GWebGLBindToGLKViewFunc func)
    {
        gwebgl_bind_glkview_func_ = func;
    }
#endif

protected:
    G2DFontDrawTextFunc g2d_font_draw_text_func_;
    GWebGLTxtImage2DFunc gwebgl_txtImage_2d_func_;
    GWebGLTxtSubImage2DFunc gwebgl_txtsubImage_2d_func_;
#ifdef IOS
    GWebGLBindToGLKViewFunc gwebgl_bind_glkview_func_;
#endif
    
    bool mIsFboSupported;
    short mWidth;
    short mHeight;
    GTransform mProjectTransform;
    GPath mPath;
    std::vector< GCanvasState > mStateStack;
    GCanvasState *mCurrentState;
    bool mHasClipRegion;
    bool mHiQuality;
    int mVertexBufferIndex;
    GShader *mSaveShader;
//    GTexture mFboTexture;
    GLuint mFboFrame;
    GLuint mFboStencil;
    GLint mSaveFboFrame;
    GColorRGBA mClearColor;

#ifdef ANDROID
    GShaderManager *mShaderManager;
#endif

public:
    GTexture mFboTexture;
    float mDevicePixelRatio;
    int mContextType; // 0--2d;1--webgl
    bool mContextLost;
#ifdef ANDROID
private:
    void adjustTextPenPoint(std::vector< GFont * > font,
                            const unsigned short *text,
                            unsigned int textLength,
                            /*out*/ float &x,
                            /*out*/ float &y);
#endif
};

#endif /* defined(__GCanvas__GCanvasContext__) */
