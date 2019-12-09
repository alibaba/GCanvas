/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "../../gcanvas/GFontManager.h"
#include "../../gcanvas/GCanvas2dContext.h"
#include <assert.h>
#include "../../support/Log.h"
#include "../../gcanvas/GFont.h"
#include "../../GCanvas.hpp"
#include "../../gcanvas/GFontCache.h"
#include "support/CharacterSet.h"


class GFontManagerImplement : public GFontManager
{
public:
    GFontManagerImplement(GCanvasContext *context);

    virtual ~GFontManagerImplement() {};

    void DrawText(const unsigned short *text,
                  unsigned int text_length, float x, float y,
                  bool isStroke, gcanvas::GFontStyle *fontStyle);

    float MeasureText(const char *text,
                      unsigned int text_length, gcanvas::GFontStyle *fontStyle);

private:
    void adjustTextPenPoint(std::vector<GFont *> font,
                            const unsigned short *text,
                            unsigned int textLength,
                            bool isStroke,
            /*out*/ float &x,
            /*out*/ float &y);

    GFont *GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle);

    void FillTextInternal(GFont *font, bool isStroke, wchar_t text, float &x, float y);


};

GFontManager *GFontManager::NewInstance(GCanvasContext *context)
{
    return new GFontManagerImplement(context);
}

GFontManagerImplement::GFontManagerImplement(GCanvasContext *context) : GFontManager(context)
{

}

void GFontManagerImplement::DrawText(const unsigned short *text,
                                     unsigned int text_length, float x, float y,
                                     bool isStroke, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || text_length == 0)
    {
        return;
    }
    std::vector<GFont *> fonts;

    for (unsigned int i = 0; i < text_length; ++i)
    {
        fonts.push_back(GetFontByCharCode(text[i], fontStyle));
    }

    adjustTextPenPoint(fonts, text, text_length, isStroke, x, y);

    for (unsigned int i = 0; i < text_length; ++i)
    {
        FillTextInternal(fonts[i], isStroke, text[i], x, y);
    }

}

float GFontManagerImplement::MeasureText(const char *text,
                                         unsigned int textLength, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || textLength == 0)
    {
        return 0;
    }

    Utf8ToUCS2 *lbData = new Utf8ToUCS2(text, textLength);

    unsigned short *ucs = lbData->ucs2;
    textLength = lbData->ucs2len;

    std::vector<GFont *> fonts;

    for (unsigned int i = 0; i < textLength; ++i)
    {
        fonts.push_back(GetFontByCharCode(ucs[i], fontStyle));
    }

    int deltaX = 0;
    for (unsigned int i = 0; i < textLength; ++i)
    {
        auto glyph = fonts[i]->GetGlyph(ucs[i], false);

        if (glyph != nullptr)
        {
            deltaX += glyph->advanceX;
        }
    }

    delete lbData;

    return deltaX;
}

void GFontManagerImplement::adjustTextPenPoint(std::vector<GFont *> font,
                                               const unsigned short *text,
                                               unsigned int textLength,
                                               bool isStroke,
        /*out*/ float &x,
        /*out*/ float &y)
{
    if (mContext->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_START &&
        mContext->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_LEFT)
    {
        auto left_x = x;
        auto delta_x = 0.0f;
        for (unsigned int i = 0; i < textLength; ++i)
        {
            auto glyph = font[i]->GetGlyph(text[i], isStroke);

            if (glyph != nullptr)
            {
                delta_x += glyph->advanceX;
            }
        }

        if (mContext->mCurrentState->mTextAlign == GTextAlign::TEXT_ALIGN_CENTER)
        {
            x = left_x - delta_x / 2.0f;
        }
        else // textAlign is "Right" or "End"
        {
            x = left_x - delta_x;
        }
    }

    GFont *font0 = font[0];
    font0->GetGlyph(text[0], isStroke);
    auto font_metrics = font0->GetMetrics();
    auto ascender = font_metrics->ascender;
    auto descender = font_metrics->descender;

    switch (mContext->mCurrentState->mTextBaseline)
    {
        case TEXT_BASELINE_TOP:
        case TEXT_BASELINE_HANGING:
            y += fabs(ascender);
            break;
        case TEXT_BASELINE_BOTTOM:
        case TEXT_BASELINE_IDEOGRAPHIC:
            y -= fabs(descender);
            break;
        case TEXT_BASELINE_MIDDLE:
            y += (fabs(ascender) - fabs(descender)) / 2.0f;
            break;
        case TEXT_BASELINE_ALPHABETIC:
        default:
            break;
    }

}

GFont *GFontManagerImplement::GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle)
{

    float devicePixelRatio = 1;
    if (mContext->mHiQuality) devicePixelRatio = mContext->mDevicePixelRatio;

    float size = fontStyle->GetSize() * devicePixelRatio;


    GFont *font = mFontCache->GetOrCreateFont(mContext,
                                              (static_cast<GCanvas *>(mContext))->mContextId,
                                              fontStyle, charCode, size);

    return font;
}

void GFontManagerImplement::FillTextInternal(GFont *font, bool isStroke, wchar_t charcode,
                                             float &x, float y)
{
    if(isStroke)
    {
        font->DrawText(charcode, mContext, x, y, mContext->StrokeStyle(), isStroke);

    }
    else
    {
        font->DrawText(charcode, mContext, x, y, mContext->GetFillStyle(), isStroke);

    }
}
