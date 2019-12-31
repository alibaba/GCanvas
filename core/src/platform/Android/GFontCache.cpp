/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <sstream>
#include <assert.h>

#include "GFontCache.h"
#include "GSystemFontInformation.h"
#include "support/Log.h"
#include "GFontManager.h"
#include "GCanvas2dContext.h"


static GFontCache *sSharedFontInstance;

using namespace gcanvas;

//GFontCache *GFontCache::sharedInstance(GFontManager &fontManager)
//{
//    if (sSharedFontInstance == nullptr)
//    {
//        sSharedFontInstance = new GFontCache(fontManager);
//    }
//    return sSharedFontInstance;
//}
//
//void GFontCache::ClearInstance()
//{
//    delete sSharedFontInstance;
//    sSharedFontInstance = nullptr;
//}

GFontCache::GFontCache(GFontManager &fontManager) : mFontManager(fontManager)
{

}

GFontCache::~GFontCache() { clear(); }

void GFontCache::clear()
{
    std::map<std::string, GFontSet>::iterator iter = mFontCache.begin();
    for (; iter != mFontCache.end(); ++iter)
    {
        delete iter->second.font;
        delete iter->second.fallbackFont;
    }

    mFontCache.clear();
}

#ifdef GFONT_LOAD_BY_FREETYPE

GFont *
GFontCache::GetOrCreateFont(GCanvasContext *context, std::string contextId, GFontStyle *fontStyle,
                            wchar_t charCode, const float size)
{
    char key[256] = {0};
    snprintf(key, 256, "%s_%s_", contextId.c_str(),
             GetCurrentScaleFontName(context).c_str());
    std::map<std::string, GFontSet>::iterator iter = mFontCache.find(key);
    if (iter != mFontCache.end())
    {
        if (iter->second.font->IsGlyphExistedInFont(charCode))
        {
            return iter->second.font;

        }
        if (iter->second.fallbackFont)
        {
            return iter->second.fallbackFont;
        }
    }

    const char *defaultSystemFontLocation = "/system/fonts/";

    auto systemFontLocation = SystemFontInformation::GetSystemFontInformation()
            ->GetSystemFontLocation();
    const char *currentFontLocation = (systemFontLocation != nullptr)
                                      ? systemFontLocation
                                      : defaultSystemFontLocation;

    const char *currentFontFile = nullptr;
    const char *currentFontFileCopy = nullptr;


    auto fontFamily = SystemFontInformation::GetSystemFontInformation()->FindFontFamily(
            fontStyle->GetFamily().c_str());

    if (fontFamily != nullptr)
    {
        currentFontFile = fontFamily->MatchFamilyStyle(*fontStyle);
        currentFontFileCopy = currentFontFile;
    }


    if (nullptr != currentFontFile)
    {
        currentFontFile = TrySpecFont(charCode, size, currentFontLocation,
                                      currentFontFile);
    }


    //用于外部字体逻辑
    const char *extraFontLocation = SystemFontInformation::GetSystemFontInformation()
            ->GetExtraFontLocation();
    if (currentFontFile == nullptr && currentFontFileCopy != nullptr && extraFontLocation != nullptr) {
        currentFontFile = TrySpecFont(charCode, size, extraFontLocation,
                                      currentFontFileCopy);
        if (currentFontFile != nullptr) {
            currentFontLocation = extraFontLocation;
        }
    }


    if (currentFontFile == nullptr)
    {
        currentFontFile = TryDefaultFont(charCode, size, currentFontLocation);

        if (nullptr == currentFontFile)
        {
            currentFontFile = TryDefaultFallbackFont(charCode, size, currentFontLocation);

            if (nullptr == currentFontFile)
            {
                currentFontFile = TryOtherFallbackFont(context, charCode, size, currentFontLocation,
                                                       fontStyle);
            }
        }
    }

    std::string fontFileFullPath = currentFontLocation;
    if (currentFontFile[0] == '/')
    {
        fontFileFullPath = currentFontFile;

    }
    else
    {
        fontFileFullPath += currentFontFile;
    }

    GFont *font = new GFont(context, mFontManager, fontFileFullPath.c_str(), size);
    GFontSet &fontSet = mFontCache[key];
    if (fontSet.font)
    {
        fontSet.fallbackFont = font;
    }
    else
    {
        fontSet.font = font;
    }
    return font;
}

char *GFontCache::TrySpecFont(const wchar_t charCode, const float size,
                              const char *currentFontLocation,
                              const char *specFontFile)
{
    std::string fontFileFullPath = currentFontLocation;
    if (specFontFile[0] == '/')
    {
        fontFileFullPath = specFontFile;
    }
    else
    {
        fontFileFullPath += specFontFile;
    }

    bool exist = IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    if (exist)
    {
        return (char *) specFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GFontCache::TryDefaultFont(const wchar_t charCode, const float size,
                                 const char *currentFontLocation)
{
    auto defaultFontFile =
            SystemFontInformation::GetSystemFontInformation()->GetDefaultFontFile();

    if (defaultFontFile == nullptr)
    {
        return nullptr;
    }

    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += defaultFontFile;

    bool exist = this->IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    // LOG_E("TryDefaultFont: %s, %c, exist=%i", defaultFontFile, charCode, exist);

    if (exist)
    {
        return defaultFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GFontCache::TryDefaultFallbackFont(const wchar_t charCode,
                                         const float size,
                                         const char *currentFontLocation)
{
    auto defaultFontFile = "DroidSans.ttf";

    std::string fontFileFullPath = currentFontLocation;
    fontFileFullPath += defaultFontFile;

    bool exist = this->IsGlyphExistedInFont(charCode, size, fontFileFullPath);
    if (exist)
    {
        return (char *) defaultFontFile;
    }
    else
    {
        return nullptr;
    }
}

char *GFontCache::TryOtherFallbackFont(GCanvasContext *context, const wchar_t charCode,
                                       const float size,
                                       const char *currentFontLocation, GFontStyle *fontStyle)
{
    return SystemFontInformation::GetSystemFontInformation()
            ->GetClosestFontFamily(context, currentFontLocation, charCode, size,
                                   *fontStyle);
}

bool GFontCache::IsGlyphExistedInFont(const wchar_t charCode,
                                      const float size,
                                      const std::string &filename)
{
    FT_Library library;
    FT_Face face;
    bool existed = true;

    if (!LoadFace(&library, filename.c_str(), size, &face))
    {
        return false;
    }

    FT_UInt glyphIndex = FT_Get_Char_Index(face, charCode);
    if (glyphIndex == 0)
    {
        existed = false;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return existed;
}

const char *getErrorMessage(FT_Error err)
{
#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s)  case e: return s;
#define FT_ERROR_START_LIST     switch (err) {
#define FT_ERROR_END_LIST       }

#include FT_ERRORS_H
    return "(Unknown error)";
}

bool GFontCache::LoadFace(FT_Library *library, const char *filename,
                          const float size, FT_Face *face)
{
    size_t hres = 64;
    FT_Matrix matrix = {(int) ((1.0 / hres) * 0x10000L), (int) ((0.0) * 0x10000L),
                        (int) ((0.0) * 0x10000L), (int) ((1.0) * 0x10000L)};

    assert(library);
    assert(filename);
    assert(size);

    FT_Error error = FT_Init_FreeType(library);
    if (error)
    {
        return false;
    }

    error = FT_New_Face(*library, filename, 0, face);
    if (error)
    {
        LOG_E("load font %s error:%s", filename, getErrorMessage(error));
        FT_Done_FreeType(*library);
        return false;
    }

    error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
    if (error)
    {
        FT_Done_Face(*face);
        FT_Done_FreeType(*library);
        return false;
    }

    float sizeW = size * mFontManager.mContext->mCurrentState->mscaleFontX;
    float sizeH = size * mFontManager.mContext->mCurrentState->mscaleFontY;
    error = FT_Set_Char_Size(*face, (int)(sizeW * 64), (int)(sizeH * 64), (FT_UInt) 72 * hres, 72);
    if (error)
    {
        FT_Done_Face(*face);
        FT_Done_FreeType(*library);
        return false;
    }

    FT_Set_Transform(*face, &matrix, nullptr);

    return true;
}

#endif

void GFontCache::ReadyToRemoveCacheForFonts(
        const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted, bool isStroke)
{
    if (mCachedPages.size() == 3)
    {
        RemoveCacheForFonts(mCachedPages.front(), isStroke);
        mCachedPages.pop();
    }

    mCachedPages.push(fontsToBeDeleted);
}


void GFontCache::RemoveCacheForFonts(
        const std::map<GFont *, std::vector<wchar_t> > &fontsToBeDeleted, bool isStroke)
{
    std::map<GFont *, std::vector<wchar_t> >::const_iterator iter =
            fontsToBeDeleted.begin();
    for (; iter != fontsToBeDeleted.end(); ++iter)
    {
        GFont *font = iter->first;
        const std::vector<wchar_t> &vecChars = iter->second;
        std::vector<wchar_t>::const_iterator charIter = vecChars.begin();
        for (; charIter != vecChars.end(); ++charIter)
        {
            font->RemoveGlyph(*charIter, isStroke);
        }
    }
}
