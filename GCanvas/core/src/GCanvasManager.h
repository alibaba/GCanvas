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
#include "memory/AutoPtr.h"
#include <map>

namespace gcanvas
{
class GCanvasManager
{
public:
    GCanvasManager();
    virtual ~GCanvasManager();

    void NewCanvas(const std::string canvasId);
    void RemoveCanvas(const std::string canvasId);
    GCanvas *GetCanvas(const std::string canvasId);
    void addCanvas(GCanvas *p);
    int canvasCount();
    void Clear();
    void AddtoQueue(const std::string contextId,struct GCanvasCmd *);
    std::queue<struct GCanvasCmd *> * getQueueByContextId(const std::string contextId);
    void clearQueueByContextId(const std::string contextId);
    void clearQueue(std::queue<struct GCanvasCmd *> *queue);
    static GCanvasManager *GetManager();
    static void Release();

protected:
    std::map< std::string, GCanvas * > mCanvases;
    std::map<std::string,std::queue<struct GCanvasCmd *> *> mCmdQueue;

private:
    static AutoPtr< GCanvasManager > sCanvasMgr;
};
}

#endif /* GCANVASMANAGER_H_ */
