/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GCanvasManager.h"
#ifdef IOS
#include "GShaderManager.h"
#endif

using namespace std;
using namespace gcanvas;

typedef struct {
    std::shared_ptr<GCanvasManager> sCanvasMgr;
} GCanvasManagerWrapStruct;


GCanvasManagerWrapStruct StaticManager(){
    static GCanvasManagerWrapStruct s;
    if( s.sCanvasMgr.get() == nullptr ){
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

    if ( s.sCanvasMgr.get() != nullptr) {
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

void GCanvasManager::NewCanvas(const string canvasId, bool onScreen, std::string appInfo)
{
    LOG_D("new canvas");
    GCanvas *c = GetCanvas(canvasId);
    if (!c)
    {
        c = new GCanvas(canvasId, !onScreen, appInfo);
        mCanvases[canvasId] = c;
    }
}

void GCanvasManager::RemoveCanvas(const string canvasId)
{
    std::map< string, GCanvas * >::iterator it = mCanvases.find(canvasId);
    if (it != mCanvases.end())
    {
        delete it->second;
        it->second = 0;
        mCanvases.erase(it);
    }
    
#ifdef IOS
    if( mCanvases.size() == 0 ) //没有canvas时候释放单例
    {
        GShaderManager::release();
        Release();
    }
#endif

}

GCanvas *GCanvasManager::GetCanvas(const string canvasId)
{
    std::map< string, GCanvas * >::iterator it = mCanvases.find(canvasId);
    if (it != mCanvases.end())
    {
        return it->second;
    }
    return 0;
}

void GCanvasManager::Clear()
{
    std::map< string, GCanvas * >::iterator it = mCanvases.begin();
    for (; it != mCanvases.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
            it->second = 0;
        }
    }
    mCanvases.clear();

#ifdef ANDROID
    std::map<string, std::queue<struct GCanvasCmd *> *>::iterator it1 = mCmdQueue.begin();
    for (; it1 != mCmdQueue.end(); ++it1) {
        if (it1->second) {
            clearQueue(it1->second);
            delete it1->second;
            it1->second = 0;
        }
    }
    mCmdQueue.clear();
#endif
}

#ifdef ANDROID
void GCanvasManager::AddtoQueue(const std::string contextId,struct GCanvasCmd *p){
    std::map< string, std::queue<struct GCanvasCmd *> *>::iterator it = mCmdQueue.find(contextId);
    if (it != mCmdQueue.end()) {
        std::queue<struct GCanvasCmd *> *queue = it->second;
        queue->push(p);
    } else {
        std::queue<struct GCanvasCmd *> *queue = new std::queue<struct GCanvasCmd *>;
        queue->push(p);
        mCmdQueue[contextId] = queue;
    }
}

std::queue<struct GCanvasCmd *> * GCanvasManager::getQueueByContextId(const std::string contextId){
    std::map< string, std::queue<struct GCanvasCmd *> *>::iterator it = mCmdQueue.find(contextId);
    if (it != mCmdQueue.end()) {
        std::queue<struct GCanvasCmd *> *queue =  it->second;
        it->second = 0;
        return queue;
    }

    return nullptr;
}
void GCanvasManager::clearQueue(std::queue<struct GCanvasCmd *> *queue){
    if(queue != nullptr){
        while(!queue->empty()) {
            struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *> (queue->front());
            queue->pop();
            delete p;
        }
    }
}

void GCanvasManager::clearQueueByContextId(const std::string contextId){
    std::queue<struct GCanvasCmd *> *queue = getQueueByContextId(contextId);
    if(queue != nullptr) {
        clearQueue(queue);
    }
}
#endif

void GCanvasManager::addCanvas(GCanvas *p){
    mCanvases[p->mContextId] = p;
}

int GCanvasManager::canvasCount(){
    return (int)mCanvases.size();
}
