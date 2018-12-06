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
#include "GFrameBufferObject.h"
#include "GFontStyle.h"

#ifdef ANDROID

#include "GFont.h"
#include "GSystemFontInformation.h"
#include "GShaderManager.h"

#endif


#include "GTextDefine.h"
#include "GTexture.h"
#include "GConvert.h"
#include "GTreemap.h"
#include "GFontManager.h"

const GColorRGBA DEFAULT_CLEAR_COLOR =
        StrValueToColorRGBA("transparent_white");

typedef enum
{
    LINE_CAP_BUTT = 0,
    LINE_CAP_ROUND,
    LINE_CAP_SQUARE
} GLineCap;

typedef enum
{
    LINE_JOIN_MITER = 0,
    LINE_JOIN_BEVEL,
    LINE_JOIN_ROUND
} GLineJoin;

typedef enum
{
    DIRECTION_INHERIT,
    DIRECTION_LEFT_TO_RIGHT,
    DIRECTION_RIGHT_TO_LEFT
} GDirection;

typedef enum
{
    COMPOSITE_OP_SOURCE_OVER = 0,
    COMPOSITE_OP_LIGHTER = 1,
    COMPOSITE_OP_DARKER = 2,
    COMPOSITE_OP_DESTINATION_OUT = 3,
    COMPOSITE_OP_DESTINATION_OVER = 4,
    COMPOSITE_OP_SOURCE_ATOP = 5,
    COMPOSITE_OP_XOR = 6,
    COMPOSITE_OP_REPLACE = 7,
    COMPOSITE_OP_ALPHA = 8,
    COMPOSITE_OP_ADD = 9,
    COMPOSITE_OP_SOURCE_OUT = 10,
    COMPOSITE_OP_DESTINATION_IN = 11,
    COMPOSITE_OP_NONE,
} GCompositeOperation;

static const struct
{
    GLenum source;
    GLenum destination;
} GCompositeOperationFuncs[] = {
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

const int InvalidateTextureId = -1;

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

    GColorRGBA mShadowColor;
    int mShadowBlur;
    float mShadowOffsetX;
    float mShadowOffsetY;

    float mGlobalAlpha;
    float mLineWidth;
    GLineCap mLineCap;
    GLineJoin mLineJoin;
    float mMiterLimit;
    std::vector<float> mLineDash;
    float mLineDashOffset;
    gcanvas::GFontStyle *mFont;

    GTextAlign mTextAlign;
    GTextBaseline mTextBaseline;
    GTransform mTransform;
    GTransform mClipTransform;
    GPath *mClipPath;
    GShader *mShader;
    GFillStyle *mFillStyle;
    GFillStyle *mStrokeStyle;
    int textureId = InvalidateTextureId;

};

//typedef void (*G2DFontDrawTextFunc)(const unsigned short *text, unsigned int textLength, float x,
//                                    float y, bool isStroke, GCanvasContext *canvasContext, void *fontContext);

typedef void (*GWebGLTxtImage2DFunc)(GLenum target, GLint level, GLenum internalformat,
                                     GLenum format, GLenum type, const char *src);

typedef void (*GWebGLTxtSubImage2DFunc)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                        GLenum format, GLenum type, const char *src);
typedef void (*GWebGLBindToGLKViewFunc)(std::string contextId);


extern GColorRGBA BlendColor(GCanvasContext *context, GColorRGBA color );
extern GColorRGBA BlendWhiteColor(GCanvasContext *context);
extern GColorRGBA BlendFillColor(GCanvasContext *context);
extern GColorRGBA BlendStrokeColor(GCanvasContext *context);

class GFontCache;

class GCanvasContext
{
public:
    static const int GCANVAS_STATE_STACK_SIZE = 16;
    static const int GCANVAS_VERTEX_BUFFER_SIZE = 2048;

    GCanvasContext(short w, short h, bool flip);

    virtual ~GCanvasContext();

    void SetDevicePixelRatio(const float ratio);
    void CalculateProjectTransform(int w, int h);


    bool InitializeGLEnvironment();

    void ResetStateStack();

    void BindVertexBuffer();

    void BindPositionVertexBuffer();

    GLuint PositionSlot();

    void SaveRenderPipeline();

    void RestoreRenderPipeline();

    void UseDefaultRenderPipeline();

    void UseTextureRenderPipeline();

    void UseShadowRenderPipeline();

    void UseShadowRenderPipeline(double radius);

    void UsePatternRenderPipeline(bool isStroke=false);

    void UseLinearGradientPipeline(bool isStroke=false);

    void UseRadialGradientPipeline(bool isStroke=false);
    
    void ApplyFillStylePipeline(bool isStroke=false);

    void SetTexture(const GTexture *texture);

    void SetTexture(int textureId);

    void ClearGeometryDataBuffers();

    void SendVertexBufferToGPU(const GLenum geometry_type = GL_TRIANGLES);

    void PushTriangle(GPoint v1, GPoint v2, GPoint v3, GColorRGBA color, std::vector<GVertex> *vec = NULL);

    void PushQuad(GPoint v1, GPoint v2, GPoint v3, GPoint v4,
                  GColorRGBA color, std::vector<GVertex> *vec = NULL);

    void PushRectangle(float x, float y, float w, float h, float tx, float ty,
                       float tw, float th, GColorRGBA color, bool flipY=false);
    void PushReverseRectangle(float x, float y, float w, float h, float tx, float ty,
                              float tw, float th, GColorRGBA color);

    void PushPoints(const std::vector<GPoint> &points, GColorRGBA color);
    
    void PushVertexs(const std::vector<GVertex> &vertexs);

    void PushTriangleFanPoints(const std::vector<GPoint> &points, GColorRGBA color);


    short Width() { return mWidth; }

    short Height() { return mHeight; }

    void Save();

    bool Restore();

    void Scale(float x, float y);

    void Rotate(float angle);

    void Translate(float x, float y);

    void ApplyTransform(float m11, float m12, float m21, float m22, float dx,
                        float dy);

    void ApplyTransform(GTransform t);

    void SetTransformOfShader(const GTransform &trans);

    void ResetTransform();

    float GlobalAlpha() const
    {
        return mCurrentState->mGlobalAlpha;
    }

    void SetGlobalAlpha(float a);

    GCompositeOperation GlobalCompositeOperation();
    void SetGlobalCompositeOperation(GCompositeOperation op, GCompositeOperation alphaOp=COMPOSITE_OP_ADD);
    
    void SetGlobalCompositeOperationSeprate(GCompositeOperation op,
                                            GCompositeOperation alphaOp);
    
    GColorRGBA StrokeStyle() const 
	{
        return mCurrentState->mStrokeColor;
    }

    void SetStrokeStyle(const GColorRGBA &c);

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

    void Blur(const GRectf &rect, float blur, float scale, std::function<void()> f,
              GFrameBufferObjectPtr &originFbo);

    void Blur(const GRectf &rect, float blur, std::function<void()> f);

    void FillRectBlur(float x, float y, float w, float h);

    void StrokeRect(float x, float y, float w, float h);

    void ImageBlur(float w, float h, int TextureId, float sx,
                   float sy, float sw, float sh, float dx,
                   float dy, float dw, float dh);

    void Fill();

    void FillBlur();

    void Stroke();

    void StrokeBlur();

    void ClipRegion();

    void BeforeClip();

    void AfterClip();

    void ResetClip();

    bool HasClipRegion() const { return mHasClipRegion; }

    void SetClipFlag(const bool flag) { mHasClipRegion = flag; }
    
    void FillText(const unsigned short *text, unsigned int text_length,
                  float x, float y, bool isStroke, float scaleWidth=1.0);

    void DrawImage1(float w, float h, int TextureId, float sx, float sy,
                    float sw, float sh, float dx, float dy, float dw, float dh, bool flipY=false);

    void DrawCanvas(float w, float h, int TextureId, float sx, float sy,
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
    
#ifdef SUPPORT_LINEDASH
    void SetLineDash(std::vector<float> lineDash) {mCurrentState->mLineDash = lineDash;}
    
    std::vector<float> LineDash() const {
        return mCurrentState->mLineDash;
    }
    
    void SetLineDashOffset(float offset) {mCurrentState->mLineDashOffset = offset;}
    
    float LineDashOffset() const {
        return mCurrentState->mLineDashOffset;
    }

#endif
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


    void PrepareDrawElemetToFBO(GFrameBufferObject &fbo);

    void DrawFBOToFBO(GFrameBufferObject &src, GFrameBufferObject &dest);

    void DrawFBOToScreen(GFrameBufferObject &fbo, float x, float y, float w, float h,
                         GColorRGBA color);

    GTexture *GetFboTexture();

    unsigned char *GetFboBuffer();

    void UpdateFboBuffer();

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

//    void SetOverideTextureColor(int value);
#endif

    bool IsFboSupport() const { return mIsFboSupported; }

    void SetHiQuality(bool isHiQuality) { mHiQuality = isHiQuality; }

//    void SetG2DFontDrawTextFunc(G2DFontDrawTextFunc func) {
//        g2d_font_draw_text_func_ = func;
//    }

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

    GTexture *GetFontTexture();

protected:
//    G2DFontDrawTextFunc g2d_font_draw_text_func_;
    GWebGLTxtImage2DFunc gwebgl_txtImage_2d_func_;
    GWebGLTxtSubImage2DFunc gwebgl_txtsubImage_2d_func_;
#ifdef IOS
    GWebGLBindToGLKViewFunc gwebgl_bind_glkview_func_;
#endif
    bool mIsFboSupported;
    short mX;
    short mY;
    short mWidth;
    short mHeight;

public:
    short GetWidth() const { return mWidth; }

    short GetHeight() const { return mHeight; }

protected:
    GTransform mProjectTransform;
    GPath mPath;
    std::vector<GCanvasState> mStateStack;

    bool mHasClipRegion;

    int mVertexBufferIndex;
    GShader *mSaveShader;

    GColorRGBA mClearColor;
    std::map<std::string, GFrameBufferObject> mFboMap;

    static constexpr const char *DefaultFboName = "default";

    int mOverideTextureColor = 0;

#ifdef ANDROID
    GShaderManager *mShaderManager;
#endif
    GTexture *mFontTexture = nullptr;

public:
    bool mHiQuality;
    GCanvasState *mCurrentState;
    float mDevicePixelRatio;
    int mContextType; // 0--2d;1--webgl
    bool mContextLost;

    GFontManager *mFontManager = nullptr;
    GFontCache *mFontCache = nullptr;

    GVertex CanvasVertexBuffer[GCanvasContext::GCANVAS_VERTEX_BUFFER_SIZE];

#ifdef ANDROID
private:
    void adjustTextPenPoint(std::vector<GFont *> font,
                            const unsigned short *text,
                            unsigned int textLength,
            /*out*/ float &x,
            /*out*/ float &y);

#endif

private:
    bool mFlip;
    GFrameBufferObjectPool mFrameBufferPool;
};

#endif /* defined(__GCanvas__GCanvasContext__) */
