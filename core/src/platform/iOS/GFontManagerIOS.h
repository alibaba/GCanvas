//
//  GFontManagerIOS.h
//  GCanvas4Alipay
//
//  Created by cpengu on 2020/4/14.
//  Copyright © 2020 taobao. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "../../gcanvas/GFontManager.h"

class GFontManagerIOS : public GFontManager
{
public:
    GFontManagerIOS();
    virtual ~GFontManagerIOS();

    void DrawText(const unsigned short *text, unsigned int text_length, float x, float y,
                  bool isStroke, GCanvasContext* context, float scaleX=1, float scaleY=1);
    
    float MeasureText(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle);
    
    float* MeasureTextExt(const char *text, unsigned int text_length, gcanvas::GFontStyle *fontStyle);
};




