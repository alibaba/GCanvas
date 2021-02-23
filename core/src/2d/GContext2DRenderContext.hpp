/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifdef COMMANDBUFFER_2D

#ifndef GCANVAS_GCONTEXT2DRENDERCONTEXT_H
#define GCANVAS_GCONTEXT2DRENDERCONTEXT_H

#include <stdio.h>
#include <string>
#include <unordered_set>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>
#include "../export.h"
#include "../gcanvas/GCanvas2dContext.h"
#include "../gcanvas/GL/GGL.h"
#include "GCommandTypes.hpp"

namespace gcanvas
{

//Set sytle with instance with instanceID
using SetFillStrokeStyleFunc = std::function< void (void*, uint64_t, bool) >;


class GContext2DRenderContext
{
public:
    API_EXPORT GContext2DRenderContext(void* data, GCanvasContext* context, std::string contextId);
    API_EXPORT ~GContext2DRenderContext();

    void FetchPixels(uint64_t uid, int format, JSBindingPixels* pixels);
    void SetFillStrokeStyle(uint64_t uid, bool isStroke);
    
    FetchPixelsFunc mFetchPixelsFunc;
    SetFillStrokeStyleFunc mSetFillStrokeStyleFunc;
    
    long mDrawCall;
    std::atomic_bool mNeed_draw;
    
    GCanvasContext* mCanvas2D;

protected:
    void* mData;  //RTContext2DBatchRender
    std::string mContextId;


};

} //namespace gcanvas

#endif /* GCANVAS_GCONTEXT2DRENDERCONTEXT_H */

#endif
