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
#include "GFontCache.h"

#define ANDROID_FONT_TEXTURE_SIZE 1024

class GFontCache;


class GFontManagerAndroid : public GFontManager {

public:

    API_EXPORT GFontManagerAndroid(unsigned int w = ANDROID_FONT_TEXTURE_SIZE,
            unsigned int h = ANDROID_FONT_TEXTURE_SIZE);


    virtual ~GFontManagerAndroid();


    void DrawText(const unsigned short *text, unsigned int text_length, float x, float y,
                  bool isStroke, GCanvasContext* context, float scaleX = 1, float scaleY = 1) override ;


    float MeasureText(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle) override;


    float* MeasureTextExt(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle) override;


    float* PreMeasureTextHeight(const char *text, unsigned int text_length, GCanvasContext* context) override;


    GTexture* GetOrCreateFontTexture() override;


private:


    float* MeasureTextWidthHeight(const char *text, unsigned int text_length,
                            gcanvas::GFontStyle *fontStyle);


    void AdjustTextPenPoint(GCanvasContext* context, std::vector<GFont *>& font,
                            const unsigned short *text,
                            unsigned int textLength,
                            bool isStroke,
                            float &x, float &y, float sx, float sy);


    GFont *GetFontByCharCode(wchar_t charCode, gcanvas::GFontStyle *fontStyle);


    void DrawTextInternal(GCanvasContext *context, GFont *font, bool isStroke, wchar_t text,
            float& x, float y, float sx, float sy);

    // global FontCache
    GFontCache *mFontCache;

};




#endif //GCANVAS_GFONTMANAGERANDROID_H
