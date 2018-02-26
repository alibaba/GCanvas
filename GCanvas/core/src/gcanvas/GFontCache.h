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

class GFont;

class GFontCache
{
public:
    static GFontCache *sharedInstance();
    static void ClearInstance();
#ifdef GFONT_LOAD_BY_FREETYPE
    GFont *GetOrCreateFont(const std::string contextId,const std::string &fontName, const float size);
#else
    GFont *GetOrCreateFont(const std::string &key);
#endif
    void ReadyToRemoveCacheForFonts(
            const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted);
    void RemoveCacheForFonts(
            const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted);

private:
    GFontCache() {}
    ~GFontCache();

    void clear();

    static GFontCache *mSharedFontInstance;

private:
    std::map< std::string, GFont * > mFontCache;
    std::queue< std::map< GFont *, std::vector< wchar_t > > > mCachedPages;
};

#endif
