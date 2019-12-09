//
// Created by 胡帅 on 8/9/17.
//

#include "GcanvasWeexAndroid.h"
#include "grenderer.h"

using namespace gcanvas;

GcanvasWeexAndroid::GcanvasWeexAndroid(std::string componentid, GRenderer *context) :
        GCanvasWeex(componentid, GCanvasConfig({false, true})), mRender(context) {

}

GcanvasWeexAndroid::~GcanvasWeexAndroid() {
    mRender = nullptr;
}

/**
 * GCanvasWeexAndroid和GRenderer形成环状依赖,
 */
void GcanvasWeexAndroid::signalUpGLthread() {
    if (mRender != nullptr) {
        mRender->signalUpGLthread();
    }
}

void GcanvasWeexAndroid::setRefreshFlag(bool refresh) {
    if (mRender != nullptr) {
        mRender->setRefreshFlag(refresh);
    }
}
