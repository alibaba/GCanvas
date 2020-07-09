/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvasState.h"

GCanvasState::GCanvasState() {
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
    mShader = nullptr;
    mFillStyle = nullptr;
    mStrokeStyle = nullptr;
    
    mShadowColor = gcanvas::StrValueToColorRGBA("transparent");
    mShadowBlur = 0;
    mShadowOffsetX = 0;
    mShadowOffsetY = 0;
}


GCanvasState::GCanvasState(const GCanvasState &state) {
    mShader = nullptr;
    mFillStyle = nullptr;
    mStrokeStyle = nullptr;
    mFont = nullptr;

    Clone(state);
}


GCanvasState &GCanvasState::operator=(const GCanvasState &state) {
    Clone(state);
    return *this;
}


void GCanvasState::Clone(const GCanvasState& state) {
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

    mTextAlign = state.mTextAlign;
    mTextBaseline = state.mTextBaseline;
    mTransform = state.mTransform;

    mTextureId = state.mTextureId;

    mShadowColor = state.mShadowColor;
    mShadowBlur = state.mShadowBlur;
    mShadowOffsetX = state.mShadowOffsetX;
    mShadowOffsetY = state.mShadowOffsetY;

    ClearStyle();

    if (state.mFillStyle != nullptr) {
        mFillStyle = state.mFillStyle->Clone();
    }

    if (state.mStrokeStyle != nullptr) {
        mStrokeStyle = state.mStrokeStyle->Clone();
    }

    if (state.mFont != nullptr) {
        mFont = new gcanvas::GFontStyle(*state.mFont);
    } else {
        mFont = nullptr;
    }


    ClearClip();
    if (state.clipPaths.size() > 0) {
        for (int i = 0; i < state.clipPaths.size(); i++) {
            GPath* gp = new GPath(*state.clipPaths[i]);
            clipPaths.push_back(gp);
        }
    }
}


void GCanvasState::ClearClip() {
    if (clipPaths.size() > 0) {
        for (int i = 0; i < clipPaths.size(); i++) {
            delete clipPaths[i];
        }
        clipPaths.clear();
    }
}


void GCanvasState::ClearStyle() {
    if (mFont != nullptr) {
        delete mFont;
        mFont = nullptr;
    }

    if (mFillStyle != nullptr) {
        delete mFillStyle;
        mFillStyle = nullptr;
    }
    if (mStrokeStyle != nullptr) {
        delete mStrokeStyle;
        mStrokeStyle = nullptr;
    }
}


GCanvasState::~GCanvasState() {
    ClearClip();
    ClearStyle();
}
