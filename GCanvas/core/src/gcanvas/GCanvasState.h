/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCanvasState_h
#define GCanvasState_h

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
    
    //矩阵信息
    GTransform mTransform;
    
    //ClipPath
    GPath *mClipPath;
    
    //颜色/样式/阴影
    GColorRGBA mFillColor;
    GFillStyle *mFillStyle;
    
    GColorRGBA mStrokeColor;
    GFillStyle *mStrokeStyle;
    
    GColorRGBA mShadowColor;
    int mShadowBlur;
    float mShadowOffsetX;
    float mShadowOffsetY;
    
    //线条样式
    GLineCap mLineCap;
    GLineJoin mLineJoin;
    float mLineWidth;
    float mMiterLimit;
    
    std::vector<float> mLineDash;
    float mLineDashOffset;
    
    //文本
    gcanvas::GFontStyle *mFont;
    GTextAlign mTextAlign;
    GTextBaseline mTextBaseline;
    
    //合成
    float mGlobalAlpha;
    GCompositeOperation mGlobalCompositeOp;
    
    
    //其他
//    GTransform mClipTransform;
    GShader *mShader;
    int mTextureId = InvalidateTextureId;
    
    
#ifdef CANVAS_EXTENSION
    float mKerning; //文字间距
    GStrokeDirection mStrokeDirection; //stroke方向
    bool mOverrideShadowColor; //是否覆盖shadowColor, false使用shadowColor, true使用原来的颜色
    float mShadowSpread; //阴影的扩散速度
#endif
};


#endif /* GCanvasState_h */
