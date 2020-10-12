/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCANVASSTATE_H
#define GCANVAS_GCANVASSTATE_H

#include "GShader.h"
#include "GPath.h"
#include "GTransform.h"
#include "GFillStyle.h"
#include "GFontStyle.h"
#include "GTextDefine.h"
#include "GFontStyle.h"

#include <stdio.h>

const int InvalidateTextureId = -1;


class GCanvasState
{
public:
    GCanvasState();

    GCanvasState(const GCanvasState &state);

    GCanvasState &operator=(const GCanvasState &state);

    ~GCanvasState();

    void Clone(const GCanvasState&st);

    void ClearStyle();

    void ClearClip();
    
    //transfrom
    GTransform mTransform;
    
    // Clip Paths
    std::vector<GPath*> clipPaths;


    //fill style,stroke style,shadow
    GColorRGBA mFillColor;
    GFillStyle *mFillStyle;

    GColorRGBA mStrokeColor;
    GFillStyle *mStrokeStyle;

    GColorRGBA mShadowColor;
    float mShadowBlur;
    float mShadowOffsetX;
    float mShadowOffsetY;
    
    //line style
    GLineCap mLineCap;
    GLineJoin mLineJoin;
    float mLineWidth;
    float mMiterLimit;
    
    std::vector<float> mLineDash;
    float mLineDashOffset;
    
    //font style
    gcanvas::GFontStyle *mFont;
    GTextAlign mTextAlign;
    GTextBaseline mTextBaseline;
    
    //global alpha / composite
    float mGlobalAlpha;
    GCompositeOperation mGlobalCompositeOp;

    // TODO manager shader pointer
    // shader & texture
    GShader *mShader;
    int mTextureId = InvalidateTextureId;

};


#endif /* GCANVAS_GCANVASSTATE_H */
