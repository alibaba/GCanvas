/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GFONTMANAGERANDROID_H
#define GCANVAS_GFONTMANAGERANDROID_H


class GFont;

#include <GFontManager.h>

#include "GCanvas2DContextAndroid.h"
#include "gcanvas/GFontStyle.h"



class GFontManagerAndroid : public GFontManager
{
public:

    GFontManagerAndroid(GCanvasContext *context);

    virtual ~GFontManagerAndroid();

    void DrawText(const unsigned short *text,
                  unsigned int text_length, float x, float y,
                  bool isStroke, gcanvas::GFontStyle *fontStyle);
    float* MeasureTextWidthHeight(const char *text,
                                  unsigned int text_length, gcanvas::GFontStyle *fontStyle);
    float MeasureText(const char *text,
                      unsigned int text_length, gcanvas::GFontStyle *fontStyle);
    float* MeasureTextExt(const char *text,
                          unsigned int text_length, gcanvas::GFontStyle *fontStyle);
    float* PreMeasureTextHeight(const char *text,
                                unsigned int text_length, gcanvas::GFontStyle *fontStyle);

    void SetFontCache(GFontCache *fontCache);
private:
    void AdjustTextPenPoint(std::vector<GFont *> font,
                            const unsigned short *text,
                            unsigned int textLength,
                            bool isStroke,
            /*out*/ float &x,
            /*out*/ float &y);

    GFont *GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle);

    void FillTextInternal(GFont *font, bool isStroke, wchar_t text, float &x, float y);

    GFontCache *mFontCache = nullptr;
};




#endif //GCANVAS_GFONTMANAGERANDROID_H
