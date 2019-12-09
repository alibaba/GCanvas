/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCANVASMANAGER_H
#define GCANVAS_GCANVASMANAGER_H

#include "GCanvas.hpp"
#ifdef GCANVAS_WEEX
#include "GCanvasWeex.hpp"
#endif

#include <map>
#include <queue>

namespace gcanvas
{
class GCanvasManager
{
public:
    GCanvasManager();
    virtual ~GCanvasManager();

    GCanvas* NewCanvas(const std::string canvasId, bool onScreen=true, bool useFbo = true, GCanvasHooks *hooks = nullptr);
#ifdef GCANVAS_WEEX
    GCanvasWeex* NewCanvasWeex(const std::string canvasId, bool onScreen=true, bool useFbo = true);
#endif
    void RemoveCanvas(const std::string canvasId);
    GCanvas *GetCanvas(const std::string canvasId);
    void AddCanvas(GCanvas *p);
    int CanvasCount();
    void Clear();
#ifdef ANDROID
    void AddtoQueue(const std::string contextId,struct GCanvasCmd *);
    std::queue<struct GCanvasCmd *> * getQueueByContextId(const std::string contextId);
    void clearQueueByContextId(const std::string contextId);
    void clearQueue(std::queue<struct GCanvasCmd *> *queue);
#endif
    static GCanvasManager *GetManager();
    static void Release();

protected:
    std::map< std::string, GCanvas * > mCanvases;
#ifdef ANDROID
    std::map<std::string,std::queue<struct GCanvasCmd *> *> mCmdQueue;
#endif
    
};
}

#endif /* GCANVAS_GCANVASMANAGER_H */
