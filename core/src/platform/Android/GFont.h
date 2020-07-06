/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GFont__
#define __GCanvas__GFont__

#include "GPoint.h"
#include "GTexture.h"
#include "GFontStyle.h"
#include "GGlyphCache.h"
#include "GTreemap.h"

#include <map>
#include <string>
#include <vector>

#include "GFreeTypeWrap.h"


class GCanvasContext;



std::string GetGlyphKey(std::string& fontFileName, gcanvas::GFontStyle* fontStyle,
        float scaleFontX, float scaleFontY);


//std::string FontStyleNameForScale(std::string& fontName, float scaleFontX, float scaleFontY);


//std::string GetScaleFontName(GCanvasContext *context, float scaleFontX, float scaleFontY);


//std::string GetCurrentScaleFontName(GCanvasContext *context);



typedef struct GFontMetrics
{
    GFontMetrics();
    // units of EM square: font units
    unsigned short unitsPerEM;
    // sized ascender: 26.6 pixel format
    float ascender;
    // sized descender" 26.6 pixel format: This could be negative.
    float descender;
} GFontMetrics;



class GFontManager;


/**
 * Font Draw Impl
 * A GFont is a FreeType face wrap, different style(weight, italic) use different GFont,
 * a GFont can generate any size glyph and font bitmap
 */
class GFont
{
public:
#ifdef GFONT_LOAD_BY_FREETYPE
    GFont(GFontManager& fontManager, const char *fontFileName);
#else
    GFont(const char *fontDefinition);
#endif
    ~GFont();


    void DrawText(GCanvasContext *context, wchar_t text, float &x, float y,
                  GColorRGBA color, float scaleX, float scaleY, bool isStroke);


    void DrawText(GCanvasContext *context, const wchar_t *text, float &x,
                  float y, GColorRGBA color, float scaleX, float scaleY, bool isStroke);


    const GGlyph *GetOrLoadGlyph(gcanvas::GFontStyle* fontStyle, const wchar_t charcode, bool isStroke,
                           float scaleX, float scaleY,float lineWidth=1.0,float deviceRatio=1.0);


    // deprecated
    void RemoveGlyph(const wchar_t charcode, bool isStroke);


    GFontMetrics *GetMetrics() { return &mFontMetrics; }


    const std::string &GetFontFileName() const;


    static void SetFontCallback(void *(*getFontCallback)(const char *fontDefinition),
                    bool (*getFontImageCallback)(
                            void *font, wchar_t charcode, int &ftBitmapWidth,
                            int &ftBitmapHeight, unsigned char *&bitmapBuffer,
                            int &left, int &top, float &advanceX, float &advanceY));


    bool IsCharInFont(const wchar_t charCode);


    void SetFtLibrary(FT_Library libraryPtr);

private:

  void DrawGlyph(GCanvasContext *context, const GGlyph *glyph, float x, float y,
                float scaleX, float scaleY, GColorRGBA color, bool needDrawShadow);


    static void *(*getFontCallback)(const char *fontDefinition);


    static bool (*getFontImageCallback)(void *font, wchar_t charCode,
                                        int &ftBitmapWidth, int &ftBitmapHeight,
                                        unsigned char *&bitmapBuffer, int &left,
                                        int &top, float &advanceX,
                                        float &advanceY);

#ifdef GFONT_LOAD_BY_FREETYPE

    void LoadGlyphs(gcanvas::GFontStyle* style, const wchar_t *charCodes, bool isStroke,
                    float scaleX, float scaleY,float lineWidth,float deviceRatio);

#endif

    bool LoadStroke(const char *filename, FT_Stroker *stroker, float scaleX, float scaleY,float lineWidth,float deviceRatio);


    bool LoadFaceIfNot();


    bool PrepareLoadGlyph(float fontSize, float scaleX, float scaleY);


    void UpdateCurrentTextMetrics();


private:

    GFontManager& mFontManager;

#ifdef GFONT_LOAD_BY_FREETYPE
    float mPointSize;

    // font file path
    std::string mFontFileName;

    int mHinting;            // whether to use autohint when rendering font
    int mOutlineType;        //(0 = None, 1 = line, 2 = inner, 3 = outer)
    float mOutlineThickness;
#endif

    std::string mFontDefinition;
    bool mHasSetMetrics;

    GFontMetrics mFontMetrics;

    // not own FT_Library, set from external, do not dispose!!
    FT_Library mLibrary = nullptr;

    FT_Face mFace = nullptr;

    // FT_Stroker mStroker = nullptr;

};

#endif