//
// Created by zhanqu on 2020/3/25.
//

#include "../support/Log.h"
#include "GFontManager.h"
#include "GL/GTexture.h"
#include <vector>


GFontManager::GFontManager(unsigned w, unsigned h) : mGlyphCache(*this), mTreemap(w, h) {
    mFontTexture = nullptr;
};


GFontManager::~GFontManager() {
    ClearFontBuffer();
}


GTexture *GFontManager::GetOrCreateFontTexture() {
    if (mFontTexture == nullptr) {
        std::vector<GCanvasLog> logVec;
        mFontTexture = new GTexture(mTreemap.GetWidth(), mTreemap.GetHeight(), GL_ALPHA, nullptr,
                                    &logVec);
        // FIXME
        // LOG_EXCEPTION_VECTOR(mHooks, mContextId.c_str(), logVec);
    }
    return mFontTexture;
}


/**
 * dumb impl
 */
float* GFontManager::PreMeasureTextHeight(const char *text, unsigned int text_length, GCanvasContext* context) {
    float *ret = new float[4];
    return ret;
}


float* GFontManager::MeasureTextMetrics(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle) {
    return MeasureTextExt(text, text_length, fontStyle);
}


void GFontManager::ClearFontBuffer() {
    mTreemap.Clear();
    mGlyphCache.ClearGlyphsTexture();

    if (mFontTexture != nullptr) {
        delete mFontTexture;
        mFontTexture = nullptr;
    }
}


bool GFontManager::LoadGlyphToTexture(GGlyph &glyph) {
    GTexture *texture = GetOrCreateFontTexture();
    GRect rect;
    bool flag = PrepareGlyphTexture((int)glyph.width, (int)glyph.height, rect);
    if (!flag) {
        ClearFontBuffer();
        texture = GetOrCreateFontTexture();
        flag = PrepareGlyphTexture((int)glyph.width, (int)glyph.height, rect);
    }
    if(texture==nullptr){
        printf("GetOrCreateFontTexture is null \n");
    }
    if (!flag) {
        return false;
    } else {
        unsigned int texWidth = texture->GetWidth();
        unsigned int texHeight = texture->GetHeight();
        if (glyph.bitmapBuffer != nullptr) {
            texture->UpdateTexture(glyph.bitmapBuffer, rect.x, rect.y, rect.width, rect.height);
            glyph.texture = texture;
            glyph.s0 = (float) rect.x / texWidth;
            glyph.t0 = (float) rect.y / texHeight;
            glyph.s1 = (float) (rect.x + rect.width) / texWidth;
            glyph.t1 = (float) (rect.y + rect.height) / texHeight;
//
//            LOG_E("LoadGlyphToTexture: s0=%f, t0=%f, s1=%f, s2=%f, textur=%p, bitmapBuffer=%p",
//                glyph.s0, glyph.t0, glyph.s1, glyph.t1, texture, glyph.bitmapBuffer);

            delete[] glyph.bitmapBuffer;
            glyph.bitmapBuffer = nullptr;

            return true;
        } else {
            return false;
        }
    }
}


bool GFontManager::AddGlyph(std::string& fontFileName, std::string& glyphKey, GGlyph& glyph, bool isStroke) {
    bool flag = LoadGlyphToTexture(glyph);
    if (flag) {
        mGlyphCache.Insert(fontFileName, glyph.charcode, glyphKey, isStroke, glyph);
    } else {
        return false;
    }
    return true;
}


bool GFontManager::PrepareGlyphTexture(int w, int h, GRect& rect) {
    GSize size(w, h);
    return mTreemap.Add(size, rect);
}

