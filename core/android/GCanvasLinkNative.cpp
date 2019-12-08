/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GCanvasLinkNative.h"
#include "GCanvas.hpp"
#include "GCanvasManager.h"

using namespace std;
using namespace gcanvas;


__attribute__ ((visibility ("default"))) const char *
GCanvasLinkNative::CallNative(const char *ContextId, int type, const char *args) {
    if (ContextId == nullptr || args == nullptr || !strcmp(args, "")) {
        LOG_D("parameter error.\n");
//        if (ContextId == nullptr) {
//            LOG_D("context Id is null");
//        }
//        if (args == nullptr || !strcmp(args, "")) {
//            LOG_D("the args is null");
//        }
        return nullptr;
    }

    GCanvasManager *theManager = GCanvasManager::GetManager();
    string sContextId = ContextId;
    GCanvasWeex *theCanvas = static_cast<GCanvasWeex *>(theManager->GetCanvas(sContextId));
    string sArgs = args;
    if (theCanvas != nullptr) {
        return theCanvas->CallNative(type, sArgs);
    } else {
        LOG_E(""
              "callnative CAN NOT FIND Canvas, id = %s, cmd=%s", sContextId.c_str(),
              sArgs.c_str());
    }
    return nullptr;
}
