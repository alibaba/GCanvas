/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvasState.h"

using namespace gcanvas;

GCanvasState::GCanvasState()
{
    mGlobalCompositeOp = COMPOSITE_OP_NONE;
    mFillColor.rgba = {0.0f, 0.0f, 0.0f, 1.0f};
    mStrokeColor.rgba = {0.0f, 0.0f, 0.0f, 1.0f};
    mGlobalAlpha = 1.0;
    mLineWidth = 1;
    mLineCap = LINE_CAP_BUTT;
    mLineJoin = LINE_JOIN_MITER;
    mMiterLimit = 10;
    mLineDashOffset = 0;
    
    mFont = nullptr; // delay-load: "10px sans-serif"
    
    mTextAlign = TEXT_ALIGN_START;
    mTextBaseline = TEXT_BASELINE_ALPHABETIC;
    mTransform = GTransformIdentity;
//    mClipTransform = GTransformIdentity;
    mClipPath = nullptr;
    mShader = nullptr;
    mFillStyle = nullptr;
    mStrokeStyle = nullptr;
    
    mShadowColor = StrValueToColorRGBA("transparent");
    mShadowBlur = 0;
    mShadowOffsetX = 0;
    mShadowOffsetY = 0;
    
#ifdef CANVAS_EXTENSION
    mKerning = 0;
    mStrokeDirection = STROKE_DIRECTION_CENTER;
    mOverrideShadowColor = false;
    mShadowSpread = 0;
#endif
}

GCanvasState::GCanvasState(const GCanvasState &state)
{
    mGlobalCompositeOp = state.mGlobalCompositeOp;
    mFillColor.rgba = state.mFillColor.rgba;
    mStrokeColor.rgba = state.mStrokeColor.rgba;
    mGlobalAlpha = state.mGlobalAlpha;
    mLineWidth = state.mLineWidth;
    mLineCap = state.mLineCap;
    mLineJoin = state.mLineJoin;
    mMiterLimit = state.mMiterLimit;
    mShader = state.mShader;
    
    mFillStyle = nullptr;
    if (state.mFillStyle != nullptr)
    {
        mFillStyle = state.mFillStyle->Clone();
    }
    mStrokeStyle = nullptr;
    if ( state.mStrokeStyle != nullptr )
    {
        mStrokeStyle = state.mStrokeStyle->Clone();
    }
    
    if (state.mFont != nullptr)
    {
        mFont = new GFontStyle(*state.mFont);
    }
    else
    {
        mFont = nullptr;
    }
    
    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;
    
//    mClipTransform = state.mClipTransform;
    if (state.mClipPath != nullptr)
    {
        mClipPath = new GPath(*(state.mClipPath));
    }
    else
    {
        mClipPath = nullptr;
    }
    mTextureId = state.mTextureId;
    
    mShadowColor = state.mShadowColor;
    mShadowBlur = state.mShadowBlur;
    mShadowOffsetX = state.mShadowOffsetX;
    mShadowOffsetY = state.mShadowOffsetY;
    
#ifdef CANVAS_EXTENSION
    mKerning = state.mKerning;
    mStrokeDirection = state.mStrokeDirection;
    mOverrideShadowColor = state.mOverrideShadowColor;
    mShadowSpread = state.mShadowSpread;
#endif
}

GCanvasState &GCanvasState::operator=(const GCanvasState &state)
{
    mGlobalCompositeOp = state.mGlobalCompositeOp;
    mFillColor.rgba = state.mFillColor.rgba;
    mStrokeColor.rgba = state.mStrokeColor.rgba;
    mGlobalAlpha = state.mGlobalAlpha;
    mLineWidth = state.mLineWidth;
    mLineCap = state.mLineCap;
    mLineJoin = state.mLineJoin;
    mMiterLimit = state.mMiterLimit;
    mShader = state.mShader;
    
    mFillStyle = nullptr;
    if (state.mFillStyle != nullptr)
    {
        mFillStyle = state.mFillStyle->Clone();
    }
    mStrokeStyle = nullptr;
    if (state.mStrokeStyle != nullptr)
    {
        mStrokeStyle = state.mStrokeStyle->Clone();
    }
    if (mFont != nullptr)
    {
        delete mFont;
        mFont = nullptr;
    }
    
    if (state.mFont != nullptr)
    {
        mFont = new GFontStyle(*state.mFont);
    }
    
    
    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;
    
//    mClipTransform = state.mClipTransform;
    if (mClipPath != nullptr)
    {
        delete mClipPath;
        mClipPath = nullptr;
    }
    
    if (state.mClipPath != nullptr)
    {
        mClipPath = new GPath(*(state.mClipPath));
    }
    mShadowColor = state.mShadowColor;
    mShadowBlur = state.mShadowBlur;
    mShadowOffsetX = state.mShadowOffsetX;
    mShadowOffsetY = state.mShadowOffsetY;
    
#ifdef CANVAS_EXTENSION
    mKerning = state.mKerning;
    mStrokeDirection = state.mStrokeDirection;
    mOverrideShadowColor = state.mOverrideShadowColor;
    mShadowSpread = state.mShadowSpread;
#endif
    
    return *this;
}

GCanvasState::~GCanvasState()
{
    if (mClipPath != nullptr)
    {
        delete mClipPath;
    }
    if (mFont != nullptr)
    {
        delete mFont;
        mFont = nullptr;
    }
    if (mFillStyle != nullptr)
    {
        delete mFillStyle;
    }
}
