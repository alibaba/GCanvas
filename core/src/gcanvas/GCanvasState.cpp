/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvasState.h"

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
    mClipPath = nullptr;
    mShader = nullptr;
    mFillStyle = nullptr;
    mStrokeStyle = nullptr;
    
    mShadowColor = gcanvas::StrValueToColorRGBA("transparent");
    mShadowBlur = 0;
    mShadowOffsetX = 0;
    mShadowOffsetY = 0;

    mscaleFontX = 1.0f;
    mscaleFontY = 1.0f;
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
    mLineDashOffset = state.mLineDashOffset;
    mLineDash = state.mLineDash;
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
        mFont = new gcanvas::GFontStyle(*state.mFont);
    }
    else
    {
        mFont = nullptr;
    }
    
    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;
    
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

    mscaleFontX = state.mscaleFontX;
    mscaleFontY = state.mscaleFontY;
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
    mLineDashOffset = state.mLineDashOffset;
    mLineDash = state.mLineDash;
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
        mFont = new gcanvas::GFontStyle(*state.mFont);
    }
    
    
    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;
    
    if (mClipPath != nullptr)
    {
        delete mClipPath;
        mClipPath = nullptr;
    }
    
    if (state.mClipPath != nullptr)
    {
        mClipPath = new GPath(*(state.mClipPath));
    }
    
    mTextureId = state.mTextureId;

    mShadowColor = state.mShadowColor;
    mShadowBlur = state.mShadowBlur;
    mShadowOffsetX = state.mShadowOffsetX;
    mShadowOffsetY = state.mShadowOffsetY;

    mscaleFontX = state.mscaleFontX;
    mscaleFontY = state.mscaleFontY;
    
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
    if (mStrokeStyle != nullptr) {
        delete mStrokeStyle;
        mStrokeStyle = nullptr;
    }
}
