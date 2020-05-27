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
#include "GSystemFontInformation.h"
#include <assert.h>
#include "support/Log.h"
#include "utils.hpp"
#include "FontTool.hpp"

using NSFontTool::TypefaceLoader;
using NSFontTool::TypefaceProvider;

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
        //delete iter->second.fallbackFont;
    }

    mFontCache.clear();
}

#ifdef GFONT_LOAD_BY_FREETYPE

GFont *
GFontCache::GetOrCreateFont(GCanvasContext *context, std::string contextId, GFontStyle *fontStyle,
                            wchar_t charCode, const float size)
{
  std::string fontName = fontStyle->GetName();
  char key[256] = {0};
  snprintf(key, 256, "%s_%d_%f", fontName.c_str(), charCode, size);
  std::map<std::string, GFontSet>::iterator iter = mFontCache.find(key);
  if (iter != mFontCache.end()) {
    return iter->second.font;
  }

  TypefaceProvider *tp = TypefaceProvider::getInstance();
  ASSERT(tp);
  TypefaceProvider::Typeface *face;
//   printf("call selectTypeface \n");
  face = tp->selectTypeface(charCode, fontName);
  if (!face || (face->sourceType == TypefaceLoader::TST_NET)) {
    // printf("tp->selectTypeface called \n");
    face = tp->selectTypeface(charCode);
  }
  if (!face || (face->sourceType == TypefaceLoader::TST_NET)) {
    face = tp->selectFallbackTypeface(fontName);
  }
  if (!face || (face->sourceType == TypefaceLoader::TST_NET)) {
    face = tp->selectFallbackTypeface();
  }
  if (!face) {
    //  printf("No typeface selected \n"); 
    WARN("No typeface selected.");
    return nullptr;
  }
  if (face->sourceType == TypefaceLoader::TST_NET) {
    WARN("Webfont is not supported yet.");
    return nullptr;
  }

  GFont *font = new GFont(context, mFontManager, face->source.c_str(), size);
  GFontSet &fontSet = mFontCache[key];
  fontSet.font = font;
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
    auto defaultFontFile = "wqy-microhei.ttc";

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
        assert(filename == 0);
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

    error = FT_Set_Char_Size(*face, (int) (size * 64), 0, (FT_UInt) 72 * hres, 72);
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
