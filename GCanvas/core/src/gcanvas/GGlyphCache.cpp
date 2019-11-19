/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GGlyphCache.h"
#include "GCanvas2dContext.h"

GGlyphCache::GGlyphCache(GCanvasContext *context, GFontManager &fontManager) : mContext(context),
                                                                               mFontManager(
                                                                                       fontManager)
{

}

const GGlyph *GGlyphCache::GetGlyph(const std::string& fontName,
                                    const wchar_t charcode,
                                    const std::string &font, bool isStroke)
{

    GGlyphMap::iterator iter = mGlyphs.find(
            make_tuple(fontName, charcode, font,isStroke));
    if (iter != mGlyphs.end())
    {
        if (!iter->second.texture)
        {
            if (!LoadGlyphTexture(iter->second)) {
                return nullptr;
            }
        }

        return &(iter->second);

    }

    return nullptr;

}

bool GGlyphCache::LoadGlyphTexture(GGlyph &glyph)
{
    GTexture *texture = mContext->GetFontTexture();
    GTreemap &treemap = mFontManager.mTreemap;

    GRect rect;
    GSize size((int)glyph.width, (int)glyph.height);
    bool failed = true;
    while (failed)
    {
        failed = !treemap.Add(size, rect);
        if (failed)
        {
            mContext->SendVertexBufferToGPU();
            treemap.Clear();
            ClearGlyphsTexture();

            return false;
        }
        else
        {
            texture->UpdateTexture(glyph.bitmapBuffer, rect.x, rect.y, rect.width, rect.height);
            glyph.texture = texture;
            glyph.s0 = (float) rect.x / treemap.GetWidth();
            glyph.t0 = (float) rect.y / treemap.GetHeight();
            glyph.s1 = (float) (rect.x + rect.width) / treemap.GetWidth();
            glyph.t1 = (float) (rect.y + rect.height) / treemap.GetHeight();
        }
    }

    return true;
}

void GGlyphCache::ClearGlyphsTexture()
{
    for (GGlyphMap::iterator i = mGlyphs.begin(); i != mGlyphs.end(); ++i)
    {
        unsigned char *buffer = i->second.bitmapBuffer;
        if ( buffer != nullptr ) {
            delete buffer;
            buffer = nullptr;
        }
        i->second.bitmapBuffer = nullptr;
        i->second.texture = nullptr;
    }
    mGlyphs.clear();
}

void GGlyphCache::Erase(const std::string& fontName,
                        const wchar_t charcode,
                        const std::string &font,
                        bool isStroke)
{
    mGlyphs.erase(make_tuple(fontName, charcode, font,isStroke));
}

void GGlyphCache::Insert(const std::string& fontName,
                         const wchar_t charcode,
                         const std::string &font,
                         bool isStroke,
                         const GGlyph &glyph)
{
    mGlyphs.insert(GGlyphMap::value_type(make_tuple(fontName, charcode, font,isStroke), glyph));
}