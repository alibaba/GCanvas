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


struct GFontKeySet {
    std::string fontFileName;

    std::string fallbackFontFileName;
};



class GFontManager;

class GFontCache
{
public:

    GFontCache(GFontManager* fontManager);


    ~GFontCache();


#ifdef GFONT_LOAD_BY_FREETYPE

    GFont *GetOrCreateFont(gcanvas::GFontStyle *fontStyle, wchar_t charCode);

#else
    GFont *GetOrCreateFont(const std::string &key);
#endif


private:

    void Clear();


    bool LazyInitFontLibrary();


    char *TryDefaultFont(const wchar_t charCode,
                         const char *currentFontLocation);


    char *TryDefaultFallbackFont(const wchar_t charCode,
                                 const char *currentFontLocation);


    char *TrySpecFont(const wchar_t charCode,
                      const char *currentFontLocation,
                      const char *specFontFile);


    char *TryFontFile(const wchar_t charCode, const char *fileFullPath, const char* fileName);


    GFont* LoadAndSaveFont(const char* fontFileName);


private:

    GFontManager* mFontManager;

    FT_Library mFtLibrary = nullptr;

    std::unordered_map<std::string, GFontKeySet> mFontRefMap;

    std::unordered_map<std::string, GFont*> mFontMap;

};


#endif