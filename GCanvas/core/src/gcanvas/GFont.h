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

#include <map>
#include <string>
#include <vector>
#include "GPoint.h"
#include "GTexture.h"

#define GFONT_LOAD_BY_FREETYPE

#ifdef GFONT_LOAD_BY_FREETYPE
#include <ft2build.h>
#include <freetype/freetype.h>
#endif

#define G_FONT_TEXTURE_SIZE 256

class GCanvasContext;

typedef struct
{
    /**
     * Wide character this glyph represents
     */
    wchar_t charcode;


    GTexture *texture;

    /**
     * Glyph's width in pixels.
     */
    size_t width;

    /**
     * Glyph's height in pixels.
     */
    size_t height;

    /**
     * Glyph's left bearing expressed in integer pixels.
     */
    int offsetX;

    /**
     * Glyphs's top bearing expressed in integer pixels.
     *
     * Remember that this is the distance from the baseline to the top-most
     * glyph scanline, upwards y coordinates being positive.
     */
    int offsetY;

    /**
     * For horizontal text layouts, this is the horizontal distance (in
     * fractional pixels) used to increment the pen position when the glyph is
     * drawn as part of a string of text.
     */
    float advanceX;

    /**
     * For vertical text layouts, this is the vertical distance (in fractional
     * pixels) used to increment the pen position when the glyph is drawn as
     * part of a string of text.
     */
    float advanceY;

    /**
     * First normalized texture coordinate (x) of top-left corner
     */
    float s0;

    /**
     * Second normalized texture coordinate (y) of top-left corner
     */
    float t0;

    /**
     * First normalized texture coordinate (x) of bottom-right corner
     */
    float s1;

    /**
     * Second normalized texture coordinate (y) of bottom-right corner
     */
    float t1;

    /**
     * Glyph outline type (0 = None, 1 = line, 2 = inner, 3 = outer)
     */
    int outlineType;

    /**
     * Glyph outline thickness
     */
    float outlineThickness;

    unsigned int size();
} GGlyph;

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

class GFont
{
public:
#ifdef GFONT_LOAD_BY_FREETYPE
    GFont(const char *fontName, const float size);
#else
    GFont(const char *fontDefinition);
#endif
    ~GFont();

    void DrawText(wchar_t text, GCanvasContext *context, float &x, float y,
                  GColorRGBA color);
    void DrawText(const wchar_t *text, GCanvasContext *context, float &x,
                  float y, GColorRGBA color);

    const GGlyph *GetGlyph(const wchar_t charcode);
    void RemoveGlyph(const wchar_t charcode);

    void LoadGlyph(wchar_t charcode, int ftBitmapWidth, int ftBitmapHeight,
                   unsigned char *bitmapBuffer, int left, int top,
                   float advanceX, float advanceY);

    GFontMetrics *GetMetrics() { return &mFontMetrics; }

    static void
    SetFontCallback(void *(*getFontCallback)(const char *fontDefinition),
                    bool (*getFontImageCallback)(
                            void *font, wchar_t charcode, int &ftBitmapWidth,
                            int &ftBitmapHeight, unsigned char *&bitmapBuffer,
                            int &left, int &top, float &advanceX, float &advanceY));

private:
    void drawGlyph(const GGlyph *glyph, GCanvasContext *context, float x,
                   float y, GColorRGBA color);
    static void *(*getFontCallback)(const char *fontDefinition);
    static bool (*getFontImageCallback)(void *font, wchar_t charcode,
                                        int &ftBitmapWidth, int &ftBitmapHeight,
                                        unsigned char *&bitmapBuffer, int &left,
                                        int &top, float &advanceX,
                                        float &advanceY);
#ifdef GFONT_LOAD_BY_FREETYPE
    void loadGlyphs(const wchar_t *charcodes);
    static bool loadFace(FT_Library *library, const char *filename,
                         const float size, FT_Face *face);
#endif

private:
#ifdef GFONT_LOAD_BY_FREETYPE
    float mPointSize;
    std::string mFontName;

    int mHinting;            // whether to use autohint when rendering font
    int mOutlineType;        //(0 = None, 1 = line, 2 = inner, 3 = outer)
    float mOutlineThickness; //
#endif

    std::string mFontDefinition;
    void *mFont;
    GFontMetrics mFontMetrics;
    std::map< wchar_t, GGlyph > mGlyphs;
};

#endif