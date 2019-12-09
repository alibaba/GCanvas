//
// Created by yuantong on 2017/8/1.
//

#include "gmanager.h"


using namespace std;
using namespace gcanvas;

AutoPtr<GManager> GManager::theManager;

GManager *GManager::getSingleton() {
    if (theManager.IsNULL()) {
        theManager = new GManager();
    }
    return theManager.RawData();
}


void GManager::release() {
    if (!theManager.IsNULL()) {
        theManager = nullptr;
    }
}

GManager::GManager() {
}

GManager::~GManager() {
    map<string, GRenderer*>::iterator it = m_renderMap.begin();
    for (; it != m_renderMap.end(); ++it) {
        if (it->second) {
            delete it->second;
            it->second = 0;
        }
    }
    m_renderMap.clear();
}


GRenderer* GManager::findRenderer(const std::string& key) {
    map<string, GRenderer*>::iterator it = m_renderMap.find(key);
    if(it != m_renderMap.end()){
        return it->second;
    }

    return NULL;
}

void GManager::removeRenderer(const std::string& key) {
    map<string, GRenderer*>::iterator it = m_renderMap.find(key);
    if(it != m_renderMap.end()){
        it->second->stop();
        delete it->second;
        it->second = 0;
        m_renderMap.erase(key);
    }
}


GRenderer* GManager::newRenderer(const std::string& key) {
    GRenderer* renderer = findRenderer(key);
    if(!renderer){
        renderer = new GRenderer(key);
        m_renderMap[key] = renderer;
    }

    return renderer;
}