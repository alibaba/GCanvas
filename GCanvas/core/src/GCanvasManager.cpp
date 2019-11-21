/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GCanvasManager.h"
#include "GCanvasWeex.hpp"
#ifdef IOS
#include "GShaderManager.h"
#endif

namespace gcanvas
{
typedef struct {
    std::shared_ptr<GCanvasManager> sCanvasMgr;
} GCanvasManagerWrapStruct;


GCanvasManagerWrapStruct StaticManager()
{
    static GCanvasManagerWrapStruct s;
    if( s.sCanvasMgr.get() == nullptr )
    {
        s.sCanvasMgr = std::shared_ptr<GCanvasManager>(new GCanvasManager());
    }
    return s;
}

/*static*/
GCanvasManager *GCanvasManager::GetManager()
{
    GCanvasManagerWrapStruct s = StaticManager();
    return s.sCanvasMgr.get();
}

/*static*/
void GCanvasManager::Release()
{
    GCanvasManagerWrapStruct s = StaticManager();

    if ( s.sCanvasMgr.get() != nullptr)
    {
        s.sCanvasMgr.reset();
    }
}

GCanvasManager::GCanvasManager()
{
}

GCanvasManager::~GCanvasManager()
{
    Clear();
}

GCanvas* GCanvasManager::NewCanvas(const std::string canvasId, bool onScreen, bool useFbo, GCanvasHooks *hooks)
{
    GCanvas *c = GetCanvas(canvasId);
    if (!c)
    {
        GCanvasConfig config = { !onScreen, useFbo};
        c = new GCanvas(canvasId, config, hooks);
        c->CreateContext();
        mCanvases[canvasId] = c;
    }
    
    return c;
}

#ifdef GCANVAS_WEEX
GCanvasWeex* GCanvasManager::NewCanvasWeex(const std::string canvasId, bool onScreen, bool useFbo)
{
    GCanvas *c = GetCanvas(canvasId);
    if (!c)
    {
        GCanvasConfig config = { !onScreen, useFbo };
        c = new GCanvasWeex(canvasId, config);
        c->CreateContext();
        mCanvases[canvasId] = c;
    }
    return static_cast<GCanvasWeex*>(c);
}
#endif

void GCanvasManager::RemoveCanvas(const std::string canvasId)
{
    std::map< std::string, GCanvas * >::iterator it = mCanvases.find(canvasId);
    if (it != mCanvases.end())
    {
        delete it->second;
        it->second = 0;
        mCanvases.erase(it);
    }
    
#ifdef IOS
    //release manager while remove last canvas
    if( mCanvases.size() == 0 )
    {
        GShaderManager::release();
        Release();
    }
#endif

}
GCanvas *GCanvasManager::GetCanvas(const std::string canvasId)
{
    std::map< std::string, GCanvas * >::iterator it = mCanvases.find(canvasId);
    if (it != mCanvases.end())
    {
        return it->second;
    }
    return 0;
}

void GCanvasManager::Clear()
{
    std::map< std::string, GCanvas * >::iterator it = mCanvases.begin();
    for (; it != mCanvases.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
            it->second = 0;
        }
    }
    mCanvases.clear();
#ifdef GCANVAS_WEEX
#ifdef ANDROID
    std::map<std::string, std::queue<struct GCanvasCmd *> *>::iterator it1 = mCmdQueue.begin();
    for (; it1 != mCmdQueue.end(); ++it1)
    {
        if (it1->second)
        {
            clearQueue(it1->second);
            delete it1->second;
            it1->second = 0;
        }
    }
    mCmdQueue.clear();
#endif
#endif
}


void GCanvasManager::AddCanvas(GCanvas *p)
{
    mCanvases[p->mContextId] = p;
}

int GCanvasManager::CanvasCount()
{
    return (int)mCanvases.size();
}

}
