/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifdef COMMANDBUFFER_2D

#include "GContext2DRenderContext.hpp"
#include <algorithm>
#include "../support/Log.h"
#include "../gcanvas/GL/GGL.h"

namespace gcanvas {

GContext2DRenderContext::GContext2DRenderContext(void* data, GCanvasContext* context, std::string contextId)
    : mData(data), mCanvas2D(context), mContextId(contextId), mDrawCall(0),
      mNeed_draw(false)
{

}

GContext2DRenderContext::~GContext2DRenderContext()
{
}

void GContext2DRenderContext::FetchPixels(uint64_t uid, int format, JSBindingPixels* pixels)
{
    if (mFetchPixelsFunc)
    {
        mFetchPixelsFunc(mData, uid, pixels);
    }
}

void GContext2DRenderContext::SetFillStrokeStyle(uint64_t uid, bool isStroke)
{
    if (mSetFillStrokeStyleFunc)
    {
        mSetFillStrokeStyleFunc(mData, uid, isStroke);
    }
}


} //namespace gcanvas

#endif
