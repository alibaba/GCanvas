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
#include "GGlyphCache.h"
#include "GTreemap.h"

#include <map>
#include <string>
#include <vector>

#define GFONT_LOAD_BY_FREETYPE

#ifdef GFONT_LOAD_BY_FREETYPE
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftstroke.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftglyph.h>
#endif


class GCanvasContext;





bool LoadFace(FT_Library *library, const char *filename,
              const float size, FT_Face *face, float scaleFontX, float scaleFontY);



bool IsGlyphExistedInFont(const wchar_t charCode, const float size,
                          std::string filename, float scaleFontX, float scaleFontY);



std::string FontStyleNameForScale(std::string& fontName, float scaleFontX, float scaleFontY);


std::string GetCurrentScaleFontName(GCanvasContext *context);



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

class GFont
{
public:
#ifdef GFONT_LOAD_BY_FREETYPE
    GFont(GCanvasContext* context, GFontManager& fontManager, const char *fontName, const float size);
#else
    GFont(const char *fontDefinition);
#endif
    ~GFont();

    void DrawText(wchar_t text, GCanvasContext *context, float &x, float y,
                  GColorRGBA color, bool isStroke);
    void DrawText(const wchar_t *text, GCanvasContext *context, float &x,
                  float y, GColorRGBA color, bool isStroke);

    const GGlyph *GetGlyph(const wchar_t charcode, bool isStroke);
    void RemoveGlyph(const wchar_t charcode, bool isStroke);

    void LoadGlyph(wchar_t charcode, int ftBitmapWidth, int ftBitmapHeight,
                   unsigned char *bitmapBuffer, int left, int top,
                   float advanceX, float advanceY, bool isStroke);
    void LoadGlyphTexture( GGlyph& glyph);

    GFontMetrics *GetMetrics() { return &mFontMetrics; }

    const std::string &GetFontName() const;

    static void
    SetFontCallback(void *(*getFontCallback)(const char *fontDefinition),
                    bool (*getFontImageCallback)(
                            void *font, wchar_t charcode, int &ftBitmapWidth,
                            int &ftBitmapHeight, unsigned char *&bitmapBuffer,
                            int &left, int &top, float &advanceX, float &advanceY));

    bool IsGlyphExistedInFont(const wchar_t charCode);
private:
    void drawGlyph(const GGlyph *glyph, GCanvasContext *context, float x,
                   float y, GColorRGBA color);

    static void *(*getFontCallback)(const char *fontDefinition);
    static bool (*getFontImageCallback)(void *font, wchar_t charcode,
                                        int &ftBitmapWidth, int &ftBitmapHeight,
                                        unsigned char *&bitmapBuffer, int &left,
                                        int &top, float &advanceX,
                                        float &advanceY);

    bool TryLoadFaceIfNotValid();

#ifdef GFONT_LOAD_BY_FREETYPE
    void loadGlyphs(const wchar_t *charcodes,bool isStroke);

    void DisposeFreeTypeFace();
    void DisposeFreeType();

#endif
    bool LoadStroke(const char *filename, const float size,
                    FT_Stroker *stroker);
    bool LoadFace(const char *filename, const float size,
                  FT_Face *face);

private:
    GCanvasContext* mContext;
    GFontManager& mFontManager;
#ifdef GFONT_LOAD_BY_FREETYPE
    float mPointSize;
    std::string mFontName;

    int mHinting;            // whether to use autohint when rendering font
    int mOutlineType;        //(0 = None, 1 = line, 2 = inner, 3 = outer)
    float mOutlineThickness; //
#endif

    std::string mFontDefinition;
    bool mHasSetMetrics;
    GFontMetrics mFontMetrics;

    FT_Library mLibrary= nullptr;
    FT_Face mFace= nullptr;
    FT_Stroker mStroker= nullptr;
};

#endif