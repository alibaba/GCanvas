/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCANVASANDROID_H
#define GCANVAS_GCANVASANDROID_H


#include "GCanvas.hpp"
#include "GCanvas2DContextAndroid.h"


class GCanvasAndroid : public gcanvas::GCanvas {


public:


    API_EXPORT GCanvasAndroid(std::string contextId, const GCanvasConfig& config, GCanvasHooks *hooks = nullptr);


    void CreateContext() override;


    API_EXPORT GCanvas2DContextAndroid* GetCanvasContextAndroid();


};


#endif //GCANVAS_GCANVASANDROID_H
