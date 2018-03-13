//
// Created by 胡帅 on 8/9/17.
//

#include "gcanvas_proxy.h"
#include "grenderer.h"

using namespace gcanvas;

gcanvas_proxy::gcanvas_proxy(std::string componentid,void *context) :m_context(context),GCanvas(componentid){

}

gcanvas_proxy::~gcanvas_proxy(){
    m_context = nullptr;
}

void gcanvas_proxy::signalUpGLthread() {
    if(m_context != nullptr) {
        reinterpret_cast<GRenderer *>(m_context)->signalUpGLthread();
    }
}

void gcanvas_proxy::setRefreshFlag(bool refresh) {
//    LOG_D("start to set refresh flag in gcanvas_proxy.");
    if(m_context != nullptr) {
//        LOG_D("start to set refresh flag in grenderer.");
        reinterpret_cast<GRenderer *>(m_context)->setRefreshFlag(refresh);
    }
}

void gcanvas_proxy::waitResponse(){
    if(m_context != nullptr) {
        reinterpret_cast<GRenderer *>(m_context)->waitResponse();
    }
}