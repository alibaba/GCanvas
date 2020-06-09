/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GFREETYPEWRAP_H
#define GCANVAS_GFREETYPEWRAP_H


#define GFONT_LOAD_BY_FREETYPE

#ifdef GFONT_LOAD_BY_FREETYPE
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftstroke.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftglyph.h>
#endif


namespace gcanvas
{


    bool GFT_InitLibrary(FT_Library *library);


    bool GFT_LoadFace(FT_Library library, FT_Face *face,
                      const char *filename, float width = 0, float height = 0);


    bool GFT_SetFaceCharSize(FT_Face face, float newWidth, float newHeight);


    bool GFT_IsCharInFontFile(FT_Library library, const wchar_t charCode,
                              const char *filename, float width, float height);


    bool GFT_IsCharInFace(FT_Face face, const wchar_t charCode);


    void GFT_DisposeLibrarySafe(FT_Library face);


    void GFT_DisposeFaceSafe(FT_Face face);


    void GFT_DisposeGlyphSafe(FT_Glyph glyph);


    void GFT_DisposeStrokeSafe(FT_Stroker stroker);

}



#endif //GCANVAS_GFREETYPEWRAP_H
