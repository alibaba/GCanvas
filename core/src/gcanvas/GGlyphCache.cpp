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

GGlyphCache::GGlyphCache(GFontManager &fontManager) :mFontManager(fontManager) {

}

const GGlyph *GGlyphCache::GetGlyph(const std::string& fontName,
                                    const wchar_t charCode,
                                    const std::string &glyphKey,
                                    bool isStroke, bool autoLoadTexture) {
    GGlyphMap::iterator iter = mGlyphs.find(make_tuple(fontName, charCode, glyphKey, isStroke));
    if (iter != mGlyphs.end()) {
        if (autoLoadTexture) {
            printf("the autoLoadTexture \n");   
            if (!iter->second.texture) {
                // if texture is empty, we save glyph to texture and then get textureId
                if (!mFontManager.LoadGlyphToTexture(iter->second)) {
                    LOG_E("GetGlyph:LoadGlyphToTexture fail,char=%i", charCode);
                    return nullptr;
                }
            }
        }
        return &(iter->second);
    } else { // no glyph cache
        // LOG_E("GetGlyph query null:fontName=%s,glyphKey=%s, char=%i, stroke=%i",
        //        fontName.data(), glyphKey.data(), charCode, isStroke);
    }
    return nullptr;
}


void GGlyphCache::ClearGlyphsTexture() {
    for (GGlyphMap::iterator i = mGlyphs.begin(); i != mGlyphs.end(); ++i) {
        unsigned char *buffer = i->second.bitmapBuffer;
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
        i->second.bitmapBuffer = nullptr;
        i->second.texture = nullptr;
    }
    mGlyphs.clear();
}


void GGlyphCache::Erase(const std::string& fontName,
                        const wchar_t charCode,
                        const std::string &font,
                        bool isStroke) {
    mGlyphs.erase(make_tuple(fontName, charCode, font,isStroke));
}


void GGlyphCache::Insert(const std::string& fontName,
                         const wchar_t charCode,
                         const std::string &glyphKey,
                         bool isStroke,
                         const GGlyph &glyph) {
    mGlyphs.insert(GGlyphMap::value_type(make_tuple(fontName, charCode, glyphKey, isStroke), glyph));
}
