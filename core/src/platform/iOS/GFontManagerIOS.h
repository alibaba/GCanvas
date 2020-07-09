/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

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




