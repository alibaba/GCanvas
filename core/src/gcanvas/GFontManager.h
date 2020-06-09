/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GFONTMANAGER_H
#define GCANVAS_GFONTMANAGER_H

#include "GPoint.h"
#include "GTexture.h"
#include "GGlyphCache.h"
#include "GTreemap.h"
#include <map>
#include <string>
#include <vector>


class GCanvasContext;

#define FontTextureWidth        2048
#define FontTextureHeight       2048

namespace gcanvas
{
    class GFontStyle;
}

class GFontManager
{
public:

    static GFontManager *NewInstance();
    virtual  ~GFontManager();

    bool PrepareGlyphTexture(int w, int h, GRect& rect);

    bool AddGlyph(std::string& fontFileName, std::string& glyphKey, GGlyph& glyph, bool isStroke);

    bool LoadGlyphToTexture(GGlyph& glyph);

    
    virtual GTexture* GetOrCreateFontTexture();
        
    virtual void DrawText(const unsigned short *text, unsigned int text_length, float x, float y,
                          bool isStroke, GCanvasContext* context, float scaleX=1, float scaleY=1)=0;

    virtual float MeasureText(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle)=0;
    
    // deprecated, use MeasureTextMetrics
    virtual float* MeasureTextExt(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle)=0;

    //return float[4]，0：top，1：height，2：ascender，3：descender
    float* MeasureTextMetrics(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle);

    virtual float* PreMeasureTextHeight(const char *text, unsigned int text_length, GCanvasContext* context);

protected:

    GFontManager(unsigned w = FontTextureWidth, unsigned h = FontTextureHeight);

    void ClearFontBuffer();

public:

    GTexture* mFontTexture;

    GGlyphCache mGlyphCache;
    GTreemap mTreemap;
};

#endif /* GCANVAS_GFONTMANAGER_H */
