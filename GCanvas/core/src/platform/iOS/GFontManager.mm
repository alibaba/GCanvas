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

#include "../../GCanvas.hpp"

#import "../../../../ios/Classes/GCVFont.h"


void iOS_GCanvas_Draw_Text(const unsigned short *text, unsigned int text_length, float x, float y, bool isStroke, GCanvasContext *context, void* fontContext);
float iOS_GCanvas_Mesure_Text(const char *text, unsigned int text_length, GCanvasContext *context, void* fontContext);



class GFontManagerImplement : public GFontManager
{
public:
    GFontManagerImplement(GCanvasContext *context);

    virtual ~GFontManagerImplement();

    void DrawText(const unsigned short *text,
                  unsigned int text_length, float x, float y,
                  bool isStroke, gcanvas::GFontStyle *fontStyle);
    
    float MeasureText(const char *text,
                    unsigned int text_length, gcanvas::GFontStyle *fontStyle);
};

GFontManager *GFontManager::NewInstance(GCanvasContext *context)
{
    return new GFontManagerImplement(context);
}


GFontManagerImplement::~GFontManagerImplement()
{
    mGlyphCache.ClearGlyphsTexture();
}

GFontManagerImplement::GFontManagerImplement(GCanvasContext *context) : GFontManager(context)
{
    NSString *key = [NSString stringWithFormat:@"%p", context];
    GCVFont *curFont = [GCVFont createGCFontWithKey:key];

    [curFont setContext: context];
    [curFont setGlyphCache: &mGlyphCache];
    [curFont setTreemap: &mTreemap];
}

void GFontManagerImplement::DrawText(const unsigned short *text,
                                     unsigned int text_length, float x, float y,
                                     bool isStroke, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || text_length == 0)
    {
        return;
    }

    NSString *key = [NSString stringWithFormat:@"%p", mContext];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    iOS_GCanvas_Draw_Text(text, text_length, x, y, isStroke, mContext, (__bridge void*)curFont);
}

float GFontManagerImplement::MeasureText(const char *text,
                                       unsigned int textLength, gcanvas::GFontStyle *fontStyle)
{
    NSString *key = [NSString stringWithFormat:@"%p", mContext];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    return iOS_GCanvas_Mesure_Text(text,textLength,mContext, (__bridge void*)curFont);
}

