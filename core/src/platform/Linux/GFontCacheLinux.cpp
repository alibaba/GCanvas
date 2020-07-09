/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GFontCacheLinux.hpp"
#include <sstream>
#include "GSystemFontInformation.h"
#include <assert.h>
#include "support/Log.h"
#include "utils.hpp"
#include "FontTool.hpp"

using NSFontTool::TypefaceLoader;
using NSFontTool::TypefaceProvider;

static GFontCache *sSharedFontInstance;

using namespace gcanvas;
GFontCache::GFontCache(GFontManager &fontManager) : mFontManager(fontManager)
{
}

GFontCache::~GFontCache() { Clear(); }

void GFontCache::Clear()
{
    std::map<std::string, GFontSet>::iterator iter = mFontCache.begin();
    for (; iter != mFontCache.end(); ++iter)
    {
        delete iter->second.font;
        //delete iter->second.fallbackFont;
    }

     gcanvas::GFT_DisposeLibrarySafe(mFtLibrary);
    mFtLibrary = nullptr;

    mFontCache.clear();
}

#ifdef GFONT_LOAD_BY_FREETYPE

GFont *
GFontCache::GetOrCreateFont(GFontStyle *fontStyle,
                            wchar_t charCode, const float size)
{
    if (!this->LazyInitFontLibrary())
    {
        return nullptr;
    }
    std::string fontName = fontStyle->GetName();
    char key[256] = {0};
    snprintf(key, 256, "%s_%d_%f", fontName.c_str(), charCode, size);
    std::map<std::string, GFontSet>::iterator iter = mFontCache.find(key);
    if (iter != mFontCache.end())
    {
        return iter->second.font;
    }
    TypefaceProvider *tp = TypefaceProvider::getInstance();
    ASSERT(tp);
    TypefaceProvider::Typeface *face;
    //   INFO("call selectTypeface \n");
    face = tp->selectTypeface(charCode, fontName);
    if (!face || (face->sourceType == TypefaceLoader::TST_NET))
    {
        // INFO("tp->selectTypeface called \n");
        face = tp->selectTypeface(charCode);
    }
    if (!face || (face->sourceType == TypefaceLoader::TST_NET))
    {
        face = tp->selectFallbackTypeface(fontName);
    }
    if (!face || (face->sourceType == TypefaceLoader::TST_NET))
    {
        face = tp->selectFallbackTypeface();
    }
    if (!face)
    {
        //  INFO("No typeface selected \n");
        WARN("No typeface selected.");
        return nullptr;
    }
    if (face->sourceType == TypefaceLoader::TST_NET)
    {
        WARN("Webfont is not supported yet.");
        return nullptr;
    }

    GFont *font = new GFont(mFontManager, face->source.data());
    font->SetFtLibrary(this->mFtLibrary);
    GFontSet &fontSet = mFontCache[key];
    fontSet.font = font;
    return font;
}

bool GFontCache::LazyInitFontLibrary()
{
    if (mFtLibrary == nullptr)
    {
        return gcanvas::GFT_InitLibrary(&mFtLibrary);
    }
    return true;
}

#endif
