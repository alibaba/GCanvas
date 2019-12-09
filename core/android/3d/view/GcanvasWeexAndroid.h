//
// Created by 胡帅 on 8/9/17.
//

#ifndef ANDROID_GCANVAS_PROXY_H
#define ANDROID_GCANVAS_PROXY_H

#include <GCanvasWeex.hpp>

#include "grenderer.h"

using namespace gcanvas;

class GRenderer;

class GcanvasWeexAndroid : public GCanvasWeex {
public:
    GcanvasWeexAndroid(std::string componentid, GRenderer *context);

    ~GcanvasWeexAndroid();

    void signalUpGLthread();

    void setRefreshFlag(bool refresh);


public:
    GRenderer *mRender;
};


#endif //ANDROID_GCANVAS_PROXY_H