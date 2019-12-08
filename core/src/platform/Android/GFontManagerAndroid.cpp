/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GFont.h"
#include "GCanvas.hpp"
#include "GFontCache.h"
#include "support/CharacterSet.h"
#include "GFontManagerAndroid.h"
#include "GCanvas2DContextAndroid.h"
#include "gcanvas/GFontManager.h"
#include "gcanvas/GCanvas2dContext.h"
#include "support/Log.h"
#include "support/CharacterSet.h"
#include "gcanvas/GTextDefine.h"

#include <assert.h>
GFontManagerAndroid::GFontManagerAndroid(GCanvasContext *context) : GFontManager(context) {

}


GFontManagerAndroid::~GFontManagerAndroid() {
    mTreemap.Clear();
    mGlyphCache.ClearGlyphsTexture();
}


void GFontManagerAndroid::DrawText(const unsigned short *text,
                                   unsigned int text_length, float x, float y,
                                   bool isStroke, gcanvas::GFontStyle *fontStyle) {
    if (text == nullptr || text_length == 0) {
        return;
    }
    std::vector<GFont *> fonts;

    for (unsigned int i = 0; i < text_length; ++i) {
        fonts.push_back(GetFontByCharCode(text[i], fontStyle));
    }

    AdjustTextPenPoint(fonts, text, text_length, isStroke, x, y);

    for (unsigned int i = 0; i < text_length; ++i) {
        FillTextInternal(fonts[i], isStroke, text[i], x, y);
    }
}


float *GFontManagerAndroid::MeasureTextWidthHeight(const char *text, unsigned int textLength,
                                                   gcanvas::GFontStyle *fontStyle) {
    if (text == nullptr || textLength == 0) {
        float *ret = new float[4];
        ret[0] = ret[1] = ret[2] = ret[3] = 0.0;
        return ret;
    }

    Utf8ToUCS2 *lbData = new Utf8ToUCS2(text, textLength);
    unsigned short *ucs = lbData->ucs2;
    textLength = lbData->ucs2len;

    std::vector<GFont *> fonts;

    for (unsigned int i = 0; i < textLength; ++i) {
        fonts.push_back(GetFontByCharCode(ucs[i], fontStyle));
    }

    float deltaX = 0;
    float maxHeight = 0;
    for (unsigned int i = 0; i < textLength; ++i) {
        auto glyph = fonts[i]->GetGlyph(ucs[i], false);

        if (glyph != nullptr) {
            deltaX += glyph->advanceX / mContext->mCurrentState->mscaleFontX;
        }
    }

    if (fonts.size() > 0) {
        //文本高度本来应该去除上下空白的，但是laya1，还是有点问题，所以这个高度，还是用老的高度ascender+descender绝对值
        maxHeight = fabs(fonts[0]->GetMetrics()->ascender / mContext->mCurrentState->mscaleFontY) +
                    fabs(fonts[0]->GetMetrics()->descender / mContext->mCurrentState->mscaleFontY);
    }

    //如果满足条件则，返回不带空白的文字高度
    float m_ascender = mContext->mCurrentState->mFont->GetAscender();
    float m_descender = mContext->mCurrentState->mFont->GetDescender();
    if (m_ascender > 0 && m_descender > 0) {
        maxHeight = m_ascender + m_descender;
    }

    delete lbData;

    float *ret = new float[4];
    ret[0] = deltaX;
    ret[1] = maxHeight;
    ret[2] = ret[3] = 0.0;
    return ret;
}

float GFontManagerAndroid::MeasureText(const char *text,
                                       unsigned int textLength, gcanvas::GFontStyle *fontStyle) {
    float *tmpMeasure = MeasureTextWidthHeight(text, textLength, fontStyle);
    float width = tmpMeasure[0];
    delete tmpMeasure;
    return width;
}


float *GFontManagerAndroid::MeasureTextExt(const char *text, unsigned int textLength,
                                           gcanvas::GFontStyle *fontStyle) {
    float *tmpMeasure = MeasureTextWidthHeight(text, textLength, fontStyle);
    return tmpMeasure;
}

float *GFontManagerAndroid::PreMeasureTextHeight(const char *text,
                                                 unsigned int text_length,
                                                 gcanvas::GFontStyle *fontStyle) {
    if (text == nullptr || text_length == 0) {
        float *ret = new float[4];
        ret[0] = ret[1] = ret[2] = ret[3] = 0.0;
        return ret;
    }

    Utf8ToUCS2 *lbData = new Utf8ToUCS2(text, text_length);

    unsigned short *ucs = lbData->ucs2;
    text_length = lbData->ucs2len;

    std::vector<GFont *> fonts;

    for (unsigned int i = 0; i < text_length; ++i) {
        fonts.push_back(GetFontByCharCode(ucs[i], fontStyle));
    }

    float top = 0;
    float height = 0;
    float ascender = 0;
    float descender = 0;
    for (unsigned int i = 0; i < text_length; ++i) {
        auto glyph = fonts[i]->GetGlyph(ucs[i], false);

        if (glyph != nullptr) {
            top = glyph->offsetY / mContext->mCurrentState->mscaleFontY;
            height = glyph->height / mContext->mCurrentState->mscaleFontY;
            ascender = fonts[i]->GetMetrics()->ascender / mContext->mCurrentState->mscaleFontY;
            descender = fonts[0]->GetMetrics()->descender / mContext->mCurrentState->mscaleFontY;
        }
    }

    delete lbData;

    float *ret = new float[4];
    ret[0] = top;
    ret[1] = height;
    ret[2] = ascender;
    ret[3] = descender;
    return ret;
}

void GFontManagerAndroid::AdjustTextPenPoint(std::vector<GFont *> font,
                                             const unsigned short *text,
                                             unsigned int textLength,
                                             bool isStroke,
        /*out*/ float &x,
        /*out*/ float &y) {
    if (mContext->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_START &&
        mContext->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_LEFT) {
        auto left_x = x;
        auto delta_x = 0.0f;
        for (unsigned int i = 0; i < textLength; ++i) {
            auto glyph = font[i]->GetGlyph(text[i], isStroke);

            if (glyph != nullptr) {
                delta_x += glyph->advanceX / mContext->mCurrentState->mscaleFontX;
            }
        }

        if (mContext->mCurrentState->mTextAlign == GTextAlign::TEXT_ALIGN_CENTER) {
            x = left_x - delta_x / 2.0f;
        } else // textAlign is "Right" or "End"
        {
            x = left_x - delta_x;
        }
    }

    GFont *font0 = font[0];
    font0->GetGlyph(text[0], isStroke);
    auto font_metrics = font0->GetMetrics();
    auto ascender = font_metrics->ascender / mContext->mCurrentState->mscaleFontY;
    auto descender = font_metrics->descender / mContext->mCurrentState->mscaleFontY;

    //m_ascender，m_descender不能除scale，因为在PreMeasureTextHeight获取这两个值时，已经除过了
    float m_ascender = mContext->mCurrentState->mFont->GetAscender();
    float m_descender = mContext->mCurrentState->mFont->GetDescender();

    switch (mContext->mCurrentState->mTextBaseline) {
        case TEXT_BASELINE_TOP:
        case TEXT_BASELINE_HANGING:
            if (m_ascender > 0 && m_descender > 0) {
                y += fabs(m_ascender);
            } else {
                y += fabs(ascender);
            }
            break;
        case TEXT_BASELINE_BOTTOM:
        case TEXT_BASELINE_IDEOGRAPHIC:
            if (m_ascender > 0 && m_descender > 0) {
                y -= fabs(m_descender);
            } else {
                y -= fabs(descender);
            }
            break;
        case TEXT_BASELINE_MIDDLE:
            if (m_ascender > 0 && m_descender > 0) {
                y += (fabs(m_ascender) - fabs(m_descender)) / 2.0f;
            } else {
                y += (fabs(ascender) - fabs(descender)) / 2.0f;
            }
            break;
        case TEXT_BASELINE_ALPHABETIC:
        default:
            break;
    }

}


GFont *GFontManagerAndroid::GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle) {
    float devicePixelRatio = 1;
    if (mContext->GetHiQuality()) {
        devicePixelRatio = mContext->mDevicePixelRatio;
    }
    float size = fontStyle->GetSize() * devicePixelRatio;

    GFont *font = mFontCache->GetOrCreateFont(mContext,
                                              mContext->mContextId,
                                              fontStyle, charCode, size);
    return font;
}


void GFontManagerAndroid::FillTextInternal(GFont *font, bool isStroke, wchar_t charcode,
                                           float &x, float y) {
    if (isStroke) {
        font->DrawText(charcode, mContext, x, y, mContext->StrokeStyle(), isStroke);

    } else {
        font->DrawText(charcode, mContext, x, y, mContext->FillStyle(), isStroke);

    }
}

void GFontManagerAndroid::SetFontCache(GFontCache *fontCache) {
    this->mFontCache = fontCache;
}
