/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include <string>
#include <support/Log.h>
#include "GFreeTypeWrap.h"

#ifdef GFONT_LOAD_BY_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) {e, s},
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST \
    {                     \
        0, 0              \
    }                     \
    }                     \
    ;
const struct
{
    int code;
    const char *message;
} FT_Errors[] =

#include FT_ERRORS_H

#endif

    namespace gcanvas
{

    const char *getErrorMessage(FT_Error err)
    {
#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) \
    case e:                  \
        return s;
#define FT_ERROR_START_LIST \
    switch (err)            \
    {
#define FT_ERROR_END_LIST }

#include FT_ERRORS_H
        return "(Unknown error)";
    }

    /**
     * init FT_Library
     */
    bool GFT_InitLibrary(FT_Library * library)
    {
        if (library == nullptr)
        {
            return false;
        }

        FT_Error error = FT_Init_FreeType(library);
        if (error)
        {
            LOG_E("GFT_InitLibrary:error,%s", getErrorMessage(error));
            return false;
        }
        return true;
    }

    /**
     * try load filename face, if not
     */
    bool GFT_LoadFace(FT_Library library, FT_Face * face,
                      const char *filename, float width, float height)
    {
        if (library == nullptr)
        {
            printf("library is nullptr \n");
            return false;
        }

        size_t hres = 64;
        FT_Matrix matrix = {(int)((1.0 / hres) * 0x10000L), (int)((0.0) * 0x10000L),
                            (int)((0.0) * 0x10000L), (int)((1.0) * 0x10000L)};

        FT_Error error;
        // 如果走的是这个默认字体，那么默认0采用的是日文JP(会导致【复/关】等不正常)，强制改为走中文SC才可以
        FT_Long faceIndex = 0;
        if (strstr(filename, "NotoSansCJK-Regular.ttc"))
        {
            FT_Long i, num_faces;
            FT_Face tmpFace;
            FT_Open_Args args;
            memset(&args, 0, sizeof(args));
            args.flags = FT_OPEN_PATHNAME;
            args.pathname = (FT_String *)filename;

            error = FT_Open_Face(library, &args, -1, &tmpFace);
            if (error)
            {
                return false;
            }
            num_faces = tmpFace->num_faces;

            for (i = 0; i < num_faces; i++)
            {
                FT_Done_Face(tmpFace); //这里释放的是前一次（或循环外的第一次）
                error = FT_Open_Face(library, &args, i, &tmpFace);
                if (error)
                {
                    return false;
                }
                else
                {
                    FT_String *tmpFamily = tmpFace->family_name;
                    if (strstr(tmpFamily, "SC"))
                    {
                        faceIndex = i;
                        break;
                    }
                }
            }
            FT_Done_Face(tmpFace); //这里释放是当循环的最后一次（或没进入循环过的第一次）
        }

        error = FT_New_Face(library, filename, faceIndex, face);
        if (error)
        {
            printf("FT_New_Face error \n");
            LOG_E("GFT_LoadFace %s fail: FT_New_Face error=%s", filename, getErrorMessage(error));
            return false;
        }

        error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
        if (error)
        {
            printf("FT_Select_Charmap error \n");
            LOG_E("GFT_LoadFace %s fail: FT_Select_Charmap error=%s", filename,
                  getErrorMessage(error));
            return false;
        }

        bool flag;
        if (width > 0 && height > 0)
        {
            flag = GFT_SetFaceCharSize(*face, width, height);
            if (!flag)
            {
                return false;
            }
        }

        FT_Set_Transform(*face, &matrix, nullptr);
        return true;
    }

    bool GFT_SetFaceCharSize(FT_Face face, float newWidth, float newHeight)
    {
        size_t hres = 64;
        // FT_Matrix matrix = {(int) ((1.0 / hres) * 0x10000L), (int) ((0.0) * 0x10000L),
        //                    (int) ((0.0) * 0x10000L), (int) ((1.0) * 0x10000L)};

        FT_Error error = FT_Set_Char_Size(face, (int)(newWidth * 64), (int)(newHeight * 64), (FT_UInt)72 * hres, 72);
        if (error)
        {
            return false;
        }
        return true;
    }

    /**
     * @FIXME
     * remove width/height parameter, we do no need size just for check charcode exist
     * check char is in font file, will load face first
     * this call is costly, use carefully
     */
    bool GFT_IsCharInFontFile(FT_Library library, const wchar_t charCode,
                              const char *filename, float width, float height)
    {
        FT_Face face = nullptr;
        bool exist = false;
        if (GFT_LoadFace(library, &face, filename, width, height))
        {
            exist = GFT_IsCharInFace(face, charCode);
        }
        else
        {
            // load face fail, ignore
        }
        GFT_DisposeFaceSafe(face);
        return exist;
    }

    /**
     * check charCode in FT_Face
     */
    bool GFT_IsCharInFace(FT_Face face, const wchar_t charCode)
    {
        bool exist = true;
        if (face == nullptr)
        {
            return false;
        }
        if (charCode == 0)
        {
            return false;
        }

        FT_UInt glyphIndex = FT_Get_Char_Index(face, charCode);
        if (glyphIndex == 0)
        {
            exist = false;
        }
        return exist;
    }

    void GFT_DisposeFaceSafe(FT_Face face)
    {
        if (face == nullptr)
        {
            return;
        }
        FT_Done_Face(face);
    }

    void GFT_DisposeLibrarySafe(FT_Library library)
    {
        if (library == nullptr)
        {
            return;
        }
        FT_Done_FreeType(library);
    }

    void GFT_DisposeGlyphSafe(FT_Glyph glyph)
    {
        if (glyph == nullptr)
        {
            return;
        }
        FT_Done_Glyph(glyph);
    }

    void GFT_DisposeStrokeSafe(FT_Stroker stroker)
    {
        if (stroker == nullptr)
        {
            return;
        }
        FT_Stroker_Done(stroker);
    }
}
