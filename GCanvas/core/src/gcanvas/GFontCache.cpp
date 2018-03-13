/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GFontCache.h"
#include <sstream>

static GFontCache *sSharedFontInstance;

GFontCache *GFontCache::sharedInstance()
{
    if (sSharedFontInstance == nullptr)
    {
        sSharedFontInstance = new GFontCache;
    }
    return sSharedFontInstance;
}

void GFontCache::ClearInstance()
{
    delete sSharedFontInstance;
    sSharedFontInstance = nullptr;
}

GFontCache::~GFontCache() { clear(); }

void GFontCache::clear()
{
    std::map< std::string, GFont * >::iterator iter = mFontCache.begin();
    for (; iter != mFontCache.end(); ++iter)
    {
        delete iter->second;
        iter->second = nullptr;
    }

    mFontCache.clear();
}

#ifdef GFONT_LOAD_BY_FREETYPE
GFont *GFontCache::GetOrCreateFont(const std::string contextId,const std::string &fontName,
                                     const float size)
{
    char temp[256];
    snprintf(temp, 256, "%f_%s_%s", size, fontName.c_str(),contextId.c_str());

    std::string key(temp);

    std::map< std::string, GFont * >::iterator iter = mFontCache.find(key);
    if (iter != mFontCache.end())
    {
        return iter->second;
    }

    GFont *font = new GFont(fontName.c_str(), size);
    mFontCache.insert(std::pair< std::string, GFont * >(key, font));
    return font;
}
#else
GFont *GFontCache::getOrCreateFont(const std::string &key)
{
    std::map< std::string, GFont * >::iterator iter = m_fontCache.find(key);
    if (iter != m_fontCache.end())
    {
        return iter->second;
    }

    GFont *font = new GFont(key.c_str());
    m_fontCache.insert(std::pair< std::string, GFont * >(key, font));
    return font;
}
#endif

void GFontCache::ReadyToRemoveCacheForFonts(
        const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted)
{
    if (mCachedPages.size() == 3)
    {
        RemoveCacheForFonts(mCachedPages.front());
        mCachedPages.pop();
    }

    mCachedPages.push(fontsToBeDeleted);
}

void GFontCache::RemoveCacheForFonts(
        const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted)
{
    std::map< GFont *, std::vector< wchar_t > >::const_iterator iter =
        fontsToBeDeleted.begin();
    for (; iter != fontsToBeDeleted.end(); ++iter)
    {
        GFont *font = iter->first;
        const std::vector< wchar_t > &vecChars = iter->second;
        std::vector< wchar_t >::const_iterator charIter = vecChars.begin();
        for (; charIter != vecChars.end(); ++charIter)
        {
            font->RemoveGlyph(*charIter);
        }
    }
}