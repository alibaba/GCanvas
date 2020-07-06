/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GFontManagerLinux.hpp"

GFontManager *GFontManager::NewInstance()
{
    return new GFontManagerImplementLinux(1024, 1024);
}

GFontManagerImplementLinux::GFontManagerImplementLinux(unsigned w, unsigned h) : GFontManager(w, h)
{
    this->mFontCache = new GFontCache(*this);
    using NSFontTool::TypefaceLoader;
    TypefaceLoader *tl = TypefaceLoader::getInstance();
    ASSERT(tl);
    std::string home(getenv("HOME"));
    std::string path = home + FONT_PATH ".fontcache";
    //加载fontcache
    ASSERT(tl->importFontCache(path));
}

void GFontManagerImplementLinux::DrawText(const unsigned short *text, unsigned int text_length, float x, float y,
                                     bool isStroke, GCanvasContext *context, float scaleX, float scaleY)
{
    if (text == nullptr || text_length == 0)
    {
        return;
    }
    std::vector<GFont *> fonts;
    gcanvas::GFontStyle *fontStyle = context->mCurrentState->mFont;
    for (unsigned int i = 0; i < text_length; ++i)
    {
        fonts.push_back(GetFontByCharCode(text[i], fontStyle));
    }
    AdjustTextPenPoint(context, fonts, text, text_length, isStroke, x, y, scaleX, scaleY);
    for (unsigned int i = 0; i < text_length; ++i)
    {
        DrawTextInternal(context, fonts[i], isStroke, text[i], x, y, scaleX, scaleY);
    }
}

float GFontManagerImplementLinux::MeasureText(const char *text,
                                         unsigned int textLength, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || textLength == 0)
    {
        float *ret = new float[4];
        ret[0] = ret[1] = ret[2] = ret[3] = 0.0;
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
        //todo fixme
        auto glyph = fonts[i]->GetOrLoadGlyph(fontStyle, ucs[i], false, 1, 1);

        if (glyph != nullptr)
        {
            deltaX += glyph->advanceX;
        }
    }

    delete lbData;
    return deltaX;
}

void GFontManagerImplementLinux::AdjustTextPenPoint(GCanvasContext *context,const std::vector<GFont *>&font,
                                               const unsigned short *text,
                                               unsigned int textLength,
                                               bool isStroke,
                                               float &x, float &y, float sx, float sy)
{
    gcanvas::GFontStyle *fontStyle = context->mCurrentState->mFont;
    if (context->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_START &&
        context->mCurrentState->mTextAlign != GTextAlign::TEXT_ALIGN_LEFT)
    {
        auto left_x = x;
        auto delta_x = 0.0f;
        for (unsigned int textIndex = 0; textIndex < textLength; ++textIndex)
        {
            auto glyph = font[textIndex]->GetOrLoadGlyph(fontStyle, text[textIndex], isStroke, sx, sy,
            context->mCurrentState->mLineWidth,context->GetDevicePixelRatio());

            if (glyph != nullptr)
            {
                delta_x += glyph->advanceX;
            }
        }

        if (context->mCurrentState->mTextAlign == GTextAlign::TEXT_ALIGN_CENTER)
        {
            x = left_x - delta_x / 2.0f;
        }
        else // textAlign is "Right" or "End"
        {
            x = left_x - delta_x;
        }
    }

    GFont *font0 = font[0];
    const GGlyph *glyph = font0->GetOrLoadGlyph(fontStyle, text[0], isStroke, sx, sy,
    context->mCurrentState->mLineWidth,context->GetDevicePixelRatio());

    if (glyph == nullptr)
    { // fail
        return;
    }
    auto font_metrics = font0->GetMetrics();
    auto ascender = font_metrics->ascender;
    auto descender = font_metrics->descender;

    switch (context->mCurrentState->mTextBaseline)
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

GFont *GFontManagerImplementLinux::GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle)
{

    float fontSize = fontStyle->GetSize();
    return mFontCache->GetOrCreateFont(fontStyle, charCode, fontSize);
}

GTexture *GFontManagerImplementLinux::GetOrCreateFontTexture()
{
    if (mFontTexture == nullptr)
    {
        std::vector<GCanvasLog> logVec;
        mFontTexture = new GTexture(mTreemap.GetWidth(), mTreemap.GetHeight(), GL_ALPHA, nullptr,
                                    &logVec);
        // FIXME
        // LOG_EXCEPTION_VECTOR(mHooks, mContextId.c_str(), logVec);
    }
    return mFontTexture;
}

void GFontManagerImplementLinux::DrawTextInternal(GCanvasContext *context, GFont *font, bool isStroke, wchar_t text,
                                             float &x, float y, float sx, float sy)
{
    if (isStroke)
    {
        font->DrawText(context, text, x, y, context->StrokeStyle(), sx, sy, isStroke);
    }
    else
    {
        font->DrawText(context, text, x, y, context->FillStyle(), sx, sy, isStroke);
    }
}
