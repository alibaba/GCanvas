/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GFontManager__
#define __GCanvas__GFontManager__

#include <map>
#include <string>
#include <vector>
#include "GPoint.h"
#include "GTexture.h"
#include "GGlyphCache.h"
#include "GTreemap.h"

class GCanvasContext;


namespace gcanvas
{
    class GFontStyle;
}

class GFontCache;

class GFontManager
{
public:
    static const int FontTextureWidth = 2048;
    static const int FontTextureHeight = 2048;

    static GFontManager *NewInstance(GCanvasContext *context);

    virtual  ~GFontManager() = default;

    void SetFontCache(GFontCache *fontCache)
    {
        mFontCache = fontCache;
    };

    virtual void DrawText(const unsigned short *text,
                          unsigned int text_length, float x, float y,
                          bool isStroke, gcanvas::GFontStyle *fontStyle)=0;

    virtual float MeasureText(const char *text,
                              unsigned int text_length, gcanvas::GFontStyle *fontStyle)=0;

protected:
    GFontManager(GCanvasContext *context) : mContext(context), mGlyphCache(context, *this),
                                            mTreemap(FontTextureWidth, FontTextureHeight) {};
public:
    GCanvasContext *mContext;
    GGlyphCache mGlyphCache;
    GTreemap mTreemap;
    GFontCache *mFontCache = nullptr;
};

#endif
