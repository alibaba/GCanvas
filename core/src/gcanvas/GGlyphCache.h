/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GGLYPHCACHE_H
#define GCANVAS_GGLYPHCACHE_H

#include "GPoint.h"
#include "GTexture.h"

#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

class GCanvasContext;

typedef std::tuple<std::string, wchar_t, std::string, bool> key_tuple;

struct key_hash : public std::unary_function<key_tuple, std::size_t>
{
    std::size_t operator()(const key_tuple &k) const
    {
        return std::hash<std::string>{}(std::get<0>(k)) ^ std::get<1>(k) ^
                std::hash<std::string>{}(std::get<2>(k)) ^ std::get<3>(k);
    }
};

struct key_equal : public std::binary_function<key_tuple, key_tuple, bool>
{
    bool operator()(const key_tuple &v0, const key_tuple &v1) const
    {
        return (
                std::get<0>(v0) == std::get<0>(v1) &&
                std::get<1>(v0) == std::get<1>(v1) &&
                std::get<2>(v0) == std::get<2>(v1) &&
                std::get<3>(v0) == std::get<3>(v1)
        );
    }
};


struct GGlyph
{
    /**
     * Wide character this glyph represents
     */
    wchar_t charcode;

    GTexture *texture;
    unsigned char *bitmapBuffer;
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

};

typedef std::unordered_map<key_tuple, GGlyph, key_hash, key_equal> GGlyphMap;

class GFontManager;

class GGlyphCache
{
public:
    GGlyphCache(GCanvasContext *context, GFontManager &fontManager);

    ~GGlyphCache() {};

    const GGlyph *GetGlyph(const std::string &fontName, const wchar_t charcode,
                           const std::string &font, bool isStroke);

    void Erase(const std::string &fontName, const wchar_t charcode, const std::string &font, bool isStroke);

    void
    Insert(const std::string &fontName, const wchar_t charcode, const std::string &font, bool isStroke,
           const GGlyph &glyph);

    void ClearGlyphsTexture();

private:
    bool LoadGlyphTexture(GGlyph &glyph);

private:
    GCanvasContext *mContext;
    GFontManager &mFontManager;
    GGlyphMap mGlyphs;

};

#endif /* GCANVAS_GGLYPHCACHE_H */
