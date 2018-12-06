/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef __GCANVAS__MANAGER_H_
#define __GCANVAS__MANAGER_H_

#include <map>
#include "GCanvas.hpp"

namespace gcanvas
{
class GCanvasManager
{
public:
    GCanvasManager();
    virtual ~GCanvasManager();

    void NewCanvas(std::string canvasId, bool onScreen=true);
    void RemoveCanvas(std::string canvasId);
    GCanvas *GetCanvas(std::string canvasId);
    void addCanvas(GCanvas *p);
    int canvasCount();
    void Clear();
#ifdef ANDROID
    void AddtoQueue(std::string contextId,struct GCanvasCmd *);
    std::queue<struct GCanvasCmd *> * getQueueByContextId(std::string contextId);
    void clearQueueByContextId(std::string contextId);
    void clearQueue(std::queue<struct GCanvasCmd *> *queue);
#endif
    static GCanvasManager *GetManager();
    static void Release();

protected:
    std::map< std::string, GCanvas * > mCanvases;
#ifdef ANDROID
    std::map<std::string,std::queue<struct GCanvasCmd *> *> mCmdQueue;
#endif
    
private:
    static std::auto_ptr<GCanvasManager> sCanvasMgr;
};
}

#endif /* GCANVASMANAGER_H_ */
