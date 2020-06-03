/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GLUTIL_H
#define GCANVAS_GLUTIL_H

#include "GGL.h"
#include "../../support/Log.h"
#include <vector>

namespace gcanvas{
    GLuint PixelsBindTexture(const unsigned char *rgbaData, GLint format, unsigned int width,
                             unsigned int height, std::vector<GCanvasLog> *errVec = nullptr);
    void PixelsSampler(int inWidth, int inHeight, int *inPixels, int outWidth, int outHeight, int *outPixels);
}

#endif /* GCANVAS_GLUTIL_H */
