/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GFontCache__
#define __GCanvas__GFontCache__

#include "GFont.h"
#include <iostream>
#include <map>
#include <queue>
#include <string>


class GCanvas2DContextAndroid;


class GFont;
namespace gcanvas
{
    class GFontStyle;
}

struct GFontSet
{
    GFont *font = nullptr;
    GFont *fallbackFont = nullptr;
};

class GFontManager;

class GFontCache
{
public:
    GFontCache(GFontManager& fontManager);


    ~GFontCache();

#ifdef GFONT_LOAD_BY_FREETYPE

    GFont *GetOrCreateFont(GCanvasContext *context, const std::string contextId,
                           gcanvas::GFontStyle *fontStyle, wchar_t charCode, const float size);

#else
    GFont *GetOrCreateFont(const std::string &key);
#endif

    void ReadyToRemoveCacheForFonts(
            const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted, bool isStroke);

    void RemoveCacheForFonts(
            const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted, bool isStroke);

private:

    void clear();

    bool LoadFace(FT_Library *library, const char *filename, const float size,
                  FT_Face *face);

    char *TryDefaultFont(const wchar_t charCode, const float size,
                         const char *currentFontLocation);

    char *TryDefaultFallbackFont(const wchar_t charCode, const float size,
                                 const char *currentFontLocation);

    char *TryOtherFallbackFont(GCanvasContext *context, const wchar_t charCode, const float size,
                               const char *currentFontLocation, gcanvas::GFontStyle *fontStyle);

    char *TrySpecFont(const wchar_t charCode, const float size,
                      const char *currentFontLocation,
                      const char *specFontFile);

    bool IsGlyphExistedInFont(const wchar_t charCode, const float size,
                              const std::string &filename);

private:
    GFontManager& mFontManager;
    std::map<std::string, GFontSet> mFontCache;
    std::queue<std::map<GFont *, std::vector<wchar_t> > > mCachedPages;
};

#endif
