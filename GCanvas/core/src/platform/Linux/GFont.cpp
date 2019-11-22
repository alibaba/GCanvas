/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <iostream>
#include <sstream>

#include "GFont.h"
#include "GCanvas2dContext.h"
#include "GCanvas.hpp"
#include "GSystemFontInformation.h"
#include <assert.h>
#include <cstring>
#ifdef GFONT_LOAD_BY_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s)  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct
{
    int code;
    const char *message;
} FT_Errors[] =

#include FT_ERRORS_H

#endif

#define PT_TO_PX(pt) ceilf((pt) * (1.0f + (1.0f / 3.0f)))

void * (*GFont::getFontCallback)(const char *fontDefinition) = nullptr;


bool (*GFont::getFontImageCallback)(void *font, wchar_t charcode,
                                    int &ftBitmapWidth, int &ftBitmapHeight,
                                    unsigned char *&bitmapBuffer, int &left,
                                    int &top, float &advanceX,
                                    float &advanceY) = nullptr;

GFontMetrics::GFontMetrics() : unitsPerEM(0), ascender(0.0f), descender(0.0f)
{
}

#ifdef GFONT_LOAD_BY_FREETYPE

GFont::GFont(GCanvasContext *context,
             GFontManager& fontManager,
             const char *fontName,
             const float size)
        : mContext(context),
          mFontManager(fontManager),
          mPointSize(size),
          mFontName(fontName),
          mHinting(1),
          mOutlineType(1),
          mOutlineThickness(1)
{
}

#else
GFont::GFont(const char *fontDefinition)
    : m_fontDefinition(fontDefinition), m_font(0)
{
}
#endif



bool LoadFace(FT_Library *library, const char *filename,
              const float size, FT_Face *face, float scaleFontX, float scaleFontY)
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

    float sizeW = scaleFontX;//size * mCurrentState->mscaleFontX;
    float sizeH = scaleFontY;//size * mCurrentState->mscaleFontY;
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



bool IsGlyphExistedInFont(const wchar_t charCode, const float size,
                          std::string filename, float scaleFontX, float scaleFontY)
{
    FT_Library library;
    FT_Face face;
    bool existed = true;

    if (!LoadFace(&library, filename.c_str(), size, &face, scaleFontX, scaleFontY))
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


std::string FontStyleNameForScale(std::string& fontName, float scaleFontX, float scaleFontY) {
    std::ostringstream xStr;
    std::ostringstream yStr;
    xStr << scaleFontX;
    yStr << scaleFontY;

    return  fontName + "_" + xStr.str() + yStr.str();
}



std::string GetCurrentScaleFontName(GCanvasContext *context) {
    std::string fontName = context->mCurrentState->mFont->GetName();
    return FontStyleNameForScale(fontName, context->GetCurrentState()->mscaleFontX,
                              context->GetCurrentState()->mscaleFontY);
}



GFont::~GFont()
{
    FT_Done_Face(mFace);
    FT_Done_FreeType(mLibrary);
}

void GFont::DrawText(wchar_t text, GCanvasContext *context, float &x, float y,
                     GColorRGBA color, bool isStroke)
{
    const GGlyph *glyph = GetGlyph(text, isStroke);
    if (glyph != nullptr)
    {
        drawGlyph(glyph, context, x, y, color);
        x += glyph->advanceX / mContext->mCurrentState->mscaleFontX;
    }

}

void GFont::DrawText(const wchar_t *text, GCanvasContext *context, float &x,
                     float y, GColorRGBA color, bool isStroke)
{
    if (text == nullptr || wcslen(text) == 0)
    {
        return;
    }

    for (size_t i = 0; i < wcslen(text); ++i)
    {
        const GGlyph *glyph = GetGlyph(text[i],isStroke);

        if (glyph != nullptr)
        {
            drawGlyph(glyph, context, x, y, color);
            x += glyph->advanceX / mContext->mCurrentState->mscaleFontX;
        }
    }

}

void GFont::drawGlyph(const GGlyph *glyph, GCanvasContext *context, float x,
                      float y, GColorRGBA color)
{
    context->SetTexture(glyph->texture->GetTextureID());

    float x0 = (float) (x + (glyph->offsetX / mContext->mCurrentState->mscaleFontX));
    float y0 = (float) (y - (glyph->offsetY / mContext->mCurrentState->mscaleFontY));
    float w = glyph->width / mContext->mCurrentState->mscaleFontX;
    float h = glyph->height / mContext->mCurrentState->mscaleFontY;
    float s0 = glyph->s0;
    float t0 = glyph->t0;
    float s1 = glyph->s1;
    float t1 = glyph->t1;

    context->PushRectangle(x0, y0, w, h, s0, t0, s1 - s0, t1 - t0,
                           color);
}

void GFont::SetFontCallback(
        void *(*getFontCB)(const char *fontDefinition),
        bool (*getFontImageCB)(void *font, wchar_t charcode, int &ftBitmapWidth,
                               int &ftBitmapHeight, unsigned char *&bitmapBuffer,
                               int &left, int &top, float &advanceX,
                               float &advanceY))
{
    GFont::getFontCallback = getFontCB;
    GFont::getFontImageCallback = getFontImageCB;
}

const GGlyph *GFont::GetGlyph(const wchar_t charcode, bool isStroke)
{

    const GGlyph *glyph = mFontManager.mGlyphCache.GetGlyph(mFontName, charcode,
                                                            GetCurrentScaleFontName(mContext), isStroke);
    if (glyph)
    {
        return glyph;

    }


#ifdef GFONT_LOAD_BY_FREETYPE

    wchar_t buffer[2] = {0, 0};
    buffer[0] = charcode;
    loadGlyphs(buffer, isStroke);
    glyph = mFontManager.mGlyphCache.GetGlyph(mFontName, charcode, GetCurrentScaleFontName(mContext), isStroke);
    assert(glyph);
    return glyph;


#else
    bool found = false;
    if (nullptr == m_font && nullptr != getFontCallback)
    {
        m_font = getFontCallback(m_fontDefinition.c_str());
    }

    if (nullptr != m_font && getFontImageCallback != nullptr)
    {
        int ftBitmapWidth, ftBitmapHeight, left, top;
        float advanceX, advanceY;
        unsigned char *bitmapBuffer;
        found = getFontImageCallback(m_font, charcode, ftBitmapWidth,
                                     ftBitmapHeight, bitmapBuffer, left, top,
                                     advanceX, advanceY);
        if (found)
        {
            loadGlyph(charcode, ftBitmapWidth, ftBitmapHeight, bitmapBuffer,
                      left, top, advanceX, advanceY);
            iter = m_glyphs.find(charcode);
            assert(iter != m_glyphs.end());
            return &(iter->second);
        }
    }
    return nullptr;
#endif
}

void GFont::RemoveGlyph(const wchar_t charcode, bool isStroke)
{
    mFontManager.mGlyphCache.Erase(mFontName, charcode, GetCurrentScaleFontName(mContext), isStroke);
}

void GFont::LoadGlyph(wchar_t charcode, int ftBitmapWidth, int ftBitmapHeight,
                      unsigned char *bitmapBuffer, int left, int top,
                      float advanceX, float advanceY, bool isStroke)
{

    GTexture *texture = mContext->GetFontTexture();
    GTreemap &treemap = mFontManager.mTreemap;

    GRect rect;
    GSize size(ftBitmapWidth, ftBitmapHeight);
    bool failed = true;
    while (failed)
    {
        failed = !treemap.Add(size, rect);
        if (failed)
        {
            mContext->SendVertexBufferToGPU();
            treemap.Clear();
            mFontManager.mGlyphCache.ClearGlyphsTexture();
        }
        else
        {
            texture->UpdateTexture(bitmapBuffer, rect.x, rect.y, rect.width, rect.height);
            GGlyph glyph;
            glyph.charcode = charcode;
            glyph.texture = texture;
            glyph.bitmapBuffer = new unsigned char[ftBitmapWidth * ftBitmapHeight];
            memcpy(glyph.bitmapBuffer, bitmapBuffer, ftBitmapWidth * ftBitmapHeight);

            glyph.width = ftBitmapWidth;
            glyph.height = ftBitmapHeight;
            glyph.outlineType = 0;
            glyph.outlineThickness = 0;
            glyph.offsetX = left;
            glyph.offsetY = top;
            glyph.s0 = (float) rect.x / treemap.GetWidth();
            glyph.t0 = (float) rect.y / treemap.GetHeight();
            glyph.s1 = (float) (rect.x + rect.width) / treemap.GetWidth();
            glyph.t1 = (float) (rect.y + rect.height) / treemap.GetHeight();
            glyph.advanceX = advanceX;
            glyph.advanceY = advanceY;

            mFontManager.mGlyphCache.Insert(mFontName, glyph.charcode,
                                            GetCurrentScaleFontName(mContext), isStroke, glyph);
        }

    }

}


#ifdef GFONT_LOAD_BY_FREETYPE



void GFont::loadGlyphs(const wchar_t *charcodes,bool isStroke)
{

    FT_Glyph ft_glyph;
    FT_GlyphSlot slot;
    FT_Bitmap ft_bitmap;

    assert(charcodes);

    if (mFace)
    {
    }
    else if (!LoadFace( mFontName.c_str(), mPointSize, &mFace))
    {
        return;
    }

    FT_Library library=mLibrary;
    FT_Face face=mFace;

    FT_Int32 flags = 0;
    flags |= FT_LOAD_NO_BITMAP;

    if (mHinting)
    {
        flags |= FT_LOAD_FORCE_AUTOHINT;
    }
    else
    {
        flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
    }

    /* Load each glyph */
    for (size_t i = 0; i < wcslen(charcodes); ++i)
    {
        int ft_bitmap_width = 0;
        int ft_bitmap_rows = 0;
        int ft_bitmap_pitch = 0;
        int ft_glyph_top = 0;
        int ft_glyph_left = 0;
        FT_UInt glyph_index = FT_Get_Char_Index(face, charcodes[i]);
        FT_Error error = FT_Load_Glyph(face, glyph_index, flags);
        if (error)
        {
            return;
        }

        auto defaultFontFile = gcanvas::SystemFontInformation::GetSystemFontInformation()->GetDefaultFontFile();
        if (defaultFontFile != nullptr && mFontName.length() > 0 && std::strstr(mFontName.c_str(),defaultFontFile) != nullptr) {
            int mstyle = (int)mContext->mCurrentState->mFont->GetStyle();
            if ((mstyle & (int)gcanvas::GFontStyle::Style::ITALIC) || (mstyle & (int)gcanvas::GFontStyle::Style::OBLIQUE))
            {
                FT_Matrix m_ItalicMatrix;
                m_ItalicMatrix.xx = 1 << 16;
                m_ItalicMatrix.xy = 0x5800;
                m_ItalicMatrix.yx = 0;
                m_ItalicMatrix.yy = 1 << 16;
                FT_Outline_Transform(&face->glyph->outline, &m_ItalicMatrix);
            }

            int mweight = (int)mContext->mCurrentState->mFont->GetWeight();
            if (mweight > static_cast<int>(gcanvas::GFontStyle::Weight::MEDIUM))
            {
                if (mFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE){
                    int boldPixel = 0;
                    if(mPointSize <= 35) {
                        boldPixel = 1;
                    }else if(mPointSize <= 60) {
                        boldPixel = 2;
                    }else if(mPointSize <= 80){
                        boldPixel = 3;
                    }else if(mPointSize <= 100) {
                        boldPixel = 4;
                    }else if(mPointSize <= 150){
                        boldPixel = 6;
                    }else {
                        boldPixel = 9;
                    }
                    if(isStroke) {
                        boldPixel = boldPixel + 1;
                    }
                    FT_Outline_Embolden(&face->glyph->outline, 64*boldPixel);
                }
            }
        }

        if (isStroke)
        {
            if(mStroker)
            {

            }
            else if (!LoadStroke( mFontName.c_str(), mPointSize, &mStroker))
            {
                return;
            }
            FT_Stroker stroker=mStroker;
            FT_BitmapGlyph ft_bitmap_glyph;
            error = FT_Get_Glyph(face->glyph, &ft_glyph);
            if (error)
            {
                return;
            }



            if (mOutlineType == 1)
            {
                error = FT_Glyph_Stroke(&ft_glyph, stroker, 1);
            }
            else if (mOutlineType == 2)
            {
                error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 0, 1);
            }
            else if (mOutlineType == 3)
            {
                error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 1, 1);
            }
            if (error)
            {
                return;
            }

            error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
            if (error)
            {
                return;
            }

            ft_bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(ft_glyph);
            ft_bitmap = ft_bitmap_glyph->bitmap;
            ft_bitmap_width = ft_bitmap.width;
            ft_bitmap_rows = ft_bitmap.rows;
            ft_bitmap_pitch = ft_bitmap.pitch;
            ft_glyph_top = ft_bitmap_glyph->top;
            ft_glyph_left = ft_bitmap_glyph->left;
        }
        else
        {
            error = FT_Render_Glyph(face->glyph,FT_RENDER_MODE_NORMAL);
            if (error)
            {
                return;
            }
            slot = face->glyph;
            ft_bitmap = slot->bitmap;
            ft_bitmap_width = slot->bitmap.width;
            ft_bitmap_rows = slot->bitmap.rows;
            ft_bitmap_pitch = slot->bitmap.pitch;
            ft_glyph_top = slot->bitmap_top;
            ft_glyph_left = slot->bitmap_left;

            (void) ft_bitmap_pitch;
        }

        slot = face->glyph;
        float advanceX = (float) (slot->advance.x / 64.0f);
        float advanceY = (float) (slot->advance.y / 64.0f);
        LoadGlyph(charcodes[i], ft_bitmap_width, ft_bitmap_rows,
                  ft_bitmap.buffer, ft_glyph_left, ft_glyph_top, advanceX,
                  advanceY,  isStroke);

        if (isStroke )
        {
            FT_Done_Glyph(ft_glyph);
        }
    }

    {
        this->mFontMetrics.unitsPerEM = face->units_per_EM;
        // 26.6 pixel format: convert it from font units
        this->mFontMetrics.ascender =
                (float) (face->size->metrics.ascender) / 64.0f;
        this->mFontMetrics.descender =
                (float) (face->size->metrics.descender) / 64.0f;
    }

}

const std::string &GFont::GetFontName() const
{
    return mFontName;
}



bool GFont::TryLoadFaceIfNotValid()
{
    // check library valid
    if (mLibrary == nullptr)
    {
        return LoadFace(mFontName.c_str(), mPointSize, &mFace);
    }
    else
    {
        if (mFace == nullptr)
        {
            if (!LoadFace(mFontName.c_str(), mPointSize, &mFace))
            {
                return false;
            }
        }

        return true;
    }
}


//bool GFont::IsGlyphExistedInFont(const wchar_t charCode)
//{
//    if (!TryLoadFaceIfNotValid())
//    {
//        return false;
//    }
//
//    FT_UInt glyphIndex = FT_Get_Char_Index(mFace, charCode);
//    if (glyphIndex == 0)
//    {
//        return false;
//    }
//
//    return true;
//}


bool GFont::IsGlyphExistedInFont(const wchar_t charCode)
{
    if (mLibrary)
    {
        assert(mFace);
    }
    else
    {
        if (!LoadFace(mFontName.c_str(), mPointSize, &mFace))
        {
            return false;
        }
    }

    // 强制校验非空，防止闪退
    if (mFace == nullptr) {
        return false;
    }

    FT_UInt glyphIndex = FT_Get_Char_Index(mFace, charCode);
    if (glyphIndex == 0)
    {
        return false;
    }

    return true;
}


bool GFont::LoadFace(const char *filename,
                     const float size, FT_Face *face)
{
    size_t hres = 64;
    FT_Matrix matrix = {(int) ((1.0 / hres) * 0x10000L), (int) ((0.0) * 0x10000L),
                        (int) ((0.0) * 0x10000L), (int) ((1.0) * 0x10000L)};

    assert(filename);
    assert(size);

    FT_Error error = FT_Init_FreeType(&mLibrary);
    if (error)
    {
        return false;
    }

    //如果走的是这个默认字体，那么默认0采用的是日文JP(会导致【复/关】等不正常)，强制改为走中文SC才可以
    FT_Long mface_index = 0;
    if (strstr(filename,"NotoSansCJK-Regular.ttc")) {
        FT_Long  i, num_faces;
        FT_Face face_tmp;
        FT_Open_Args args;
        memset(&args, 0, sizeof(args));
        args.flags = FT_OPEN_PATHNAME;
        args.pathname = (FT_String *)filename;

        error = FT_Open_Face(mLibrary, &args, -1, &face_tmp);
        if (error) {
            DisposeFreeType();
            return false;
        }
        num_faces = face_tmp->num_faces;

        for (i = 0; i < num_faces; i++ ) {
            FT_Done_Face(face_tmp); //这里释放的是前一次（或循环外的第一次）
            error = FT_Open_Face(mLibrary, &args, i, &face_tmp);
            if (error) {
                DisposeFreeType();
                return false;
            }else {
                FT_String* tmpFamily = face_tmp->family_name;
                if(strstr(tmpFamily,"SC")) {
                    mface_index = i;
                    break;
                }
            }
        }
        FT_Done_Face(face_tmp); //这里释放是当循环的最后一次（或没进入循环过的第一次）
    }

    error = FT_New_Face(mLibrary, filename, mface_index, &mFace);
    if (error)
    {
        assert(filename == 0);
        DisposeFreeType();
        return false;
    }

    error = FT_Select_Charmap(mFace, FT_ENCODING_UNICODE);
    if (error)
    {
        DisposeFreeTypeFace();
        DisposeFreeType();
        return false;
    }

    float sizeW = size * mContext->mCurrentState->mscaleFontX;
    float sizeH = size * mContext->mCurrentState->mscaleFontY;
    error = FT_Set_Char_Size(mFace, (int)(sizeW * 64), (int)(sizeH * 64), (FT_UInt) 72 * hres, 72);
    if (error)
    {
        DisposeFreeTypeFace();
        DisposeFreeType();
        return false;
    }

    FT_Set_Transform(mFace, &matrix, nullptr);

    return true;
}

bool GFont::LoadStroke(const char *filename,
                       const float size, FT_Stroker *stroker)
{

    assert(filename);
    assert(size);

    FT_Error error  ;
    if(!mLibrary)
    {
        error = FT_Init_FreeType(&mLibrary);
        if (error)
        {
            return false;
        }
    }

    error = FT_Stroker_New(mLibrary, stroker);
    if (error)
    {
        FT_Stroker_Done(*stroker);
        return false;
    }

    FT_Stroker_Set(*stroker, (int) (mOutlineThickness * 64),
                   FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND,
                   0);
    if (error)
    {
        FT_Stroker_Done(*stroker);
        return false;
    }


    return true;
}


void GFont::DisposeFreeTypeFace()
{
    if (mFace != nullptr)
    {
        FT_Done_Face(mFace);
        mFace = nullptr;
    }
}


void GFont::DisposeFreeType()
{
    if (mLibrary != nullptr) {
        FT_Done_FreeType(mLibrary);
        mLibrary = nullptr;
    }
}


#endif