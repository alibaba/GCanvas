/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#import "GFontManagerIOS.h"
#include "../../gcanvas/GFontManager.h"
#include "../../gcanvas/GCanvas2dContext.h"
#include "../../support/Log.h"
#include "../../GCanvas.hpp"
#import "GCVFont.h"

#include <assert.h>


extern void iOS_GCanvas_Draw_Text(const unsigned short *text, unsigned int text_length, float x, float y, bool isStroke, GCanvasContext *context, void* fontContext);
extern float iOS_GCanvas_Measure_Text(const char *text, unsigned int text_length, void* fontContext, gcanvas::GFontStyle *fontStyle);
float* iOS_GCanvas_Measure_TextExt(const char *text, unsigned int text_length, void* fontContext, gcanvas::GFontStyle *fontStyle);



GFontManager* GFontManager::NewInstance()
{
    return new GFontManagerIOS();
}

GFontManagerIOS::~GFontManagerIOS()
{
    // clear font
    NSString *key = [NSString stringWithFormat:@"%p", this];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    [curFont cleanFont];
}

GFontManagerIOS::GFontManagerIOS()
{
    NSString *key = [NSString stringWithFormat:@"%p", this];
    GCVFont *curFont = [GCVFont createGCFontWithKey:key];

    [curFont setGlyphCache: &mGlyphCache];
    [curFont setTreemap: &mTreemap];
}

void GFontManagerIOS::DrawText(const unsigned short *text, unsigned int text_length, float x, float y, bool isStroke, GCanvasContext* context, float scaleX, float scaleY)
{
    if (text == nullptr || text_length == 0)
    {
        return;
    }

    NSString *key = [NSString stringWithFormat:@"%p", this];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    iOS_GCanvas_Draw_Text(text, text_length, x, y, isStroke, context, (__bridge void*)curFont);
}

float GFontManagerIOS::MeasureText(const char *text, unsigned int textLength, gcanvas::GFontStyle *fontStyle)
{
    NSString *key = [NSString stringWithFormat:@"%p", this];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    return iOS_GCanvas_Measure_Text(text, textLength, (__bridge void*)curFont, fontStyle);
}


float* GFontManagerIOS::MeasureTextExt(const char *text, unsigned int textLength, gcanvas::GFontStyle *fontStyle)
{
    NSString *key = [NSString stringWithFormat:@"%p", this];
    GCVFont *curFont = [GCVFont getGCVFontWithKey:key];
    return iOS_GCanvas_Measure_TextExt(text, textLength, (__bridge void*)curFont, fontStyle);
}



#pragma mark - iOS implementation of Font & texImage
/**
 * Draw GCanvas2D Text with GCVFont.
 *
 * @param text          text string to draw
 * @param text_length   text length
 * @param x             draw position x
 * @param y             draw position y
 * @param isStroke      isStroke flag
 * @param context       see GCanvasContext
 *
 */
void iOS_GCanvas_Draw_Text(const unsigned short *text, unsigned int text_length, float x, float y, bool isStroke, GCanvasContext *context, void* fontContext)
{
    if (text == nullptr || text_length == 0 || context == nullptr || fontContext == nullptr) {
        return;
    }
    
    GCanvasState *current_state_ = context->GetCurrentState();
    gcanvas::GFontStyle *fontStyle = current_state_->mFont;
    GCVFont *curFont = (__bridge GCVFont*)fontContext;
    [curFont resetWithFontStyle:fontStyle isStroke:isStroke context:context];
    
    NSString *string = [[NSString alloc] initWithBytes:text length:text_length encoding:NSUTF8StringEncoding];
    GFontLayout *fontLayout = [curFont getLayoutForString:string withFontName:[curFont getFontNameWithCurrentScale:fontStyle context:context]];
    CGPoint destPoint = [curFont adjustTextPenPoint:CGPointMake(x, y)
                                          textAlign:current_state_->mTextAlign
                                           baseLine:current_state_->mTextBaseline
                                            metrics:fontLayout.metrics];
    if (current_state_->mShader) {
        current_state_->mShader->SetOverideTextureColor(1);
    }
    
    glActiveTexture(GL_TEXTURE0);
    
    [curFont drawString:string withFontName:[curFont getFontNameWithCurrentScale:fontStyle context:context] withLayout:fontLayout withPosition:destPoint context:context];
}

float iOS_GCanvas_Measure_Text(const char *text, unsigned int text_length, void* fontContext, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || text_length == 0 || fontContext == nullptr) {
        return 0;
    }
    
    GCVFont *curFont = (__bridge GCVFont*)fontContext;
    [curFont resetWithFontStyle:fontStyle isStroke:false context:nullptr];
    
    NSString *string = [[NSString alloc] initWithBytes:text length:text_length encoding:NSUTF8StringEncoding];
    GFontLayout *fontLayout = [curFont getLayoutForString:string withFontName:[curFont getFontNameWithCurrentScale:fontStyle context:nullptr]];
    
    return fontLayout.metrics.width;
}


float* iOS_GCanvas_Measure_TextExt(const char *text, unsigned int text_length, void* fontContext, gcanvas::GFontStyle *fontStyle)
{
    if (text == nullptr || text_length == 0 || fontContext == nullptr) {
        float *ret = new float[4];
        ret[0] = ret[1] = ret[2] = ret[3] = 0;
        return ret;
    }
    
    GCVFont *curFont = (__bridge GCVFont*)fontContext;
    [curFont resetWithFontStyle:fontStyle isStroke:false context:nullptr];
    
    NSString *string = [[NSString alloc] initWithBytes:text length:text_length encoding:NSUTF8StringEncoding];
    GFontLayout *fontLayout = [curFont getLayoutForString:string withFontName:[curFont getFontNameWithCurrentScale:fontStyle context:nullptr]];
    
    float *ret = new float[4];
    ret[0] = fontLayout.metrics.width;
    ret[1] = fontLayout.metrics.ascent + fontLayout.metrics.descent;
    ret[2] = ret[3] = 0;
    return ret;
}
