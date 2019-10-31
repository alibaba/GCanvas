/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCANVASLINKNATIVE_H
#define GCANVAS_GCANVASLINKNATIVE_H

//#include "GCanvas.hpp"

namespace gcanvas {
    class GCanvasLinkNative {
    public:
        static const char* CallNative(const char* ContextId,int type,const char* args);
    };
}

#endif //GCANVAS_GCANVASLINKNATIVE_H
