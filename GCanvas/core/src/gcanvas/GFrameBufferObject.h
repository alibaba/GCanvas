/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GFrameBufferObject__
#define __GCanvas__GFrameBufferObject__

#include <map>
#include <string>
#include <vector>
#include "GTexture.h"
#include "GPoint.h"
#include "GTransform.h"

class GFrameBufferObject
{
public:
    GFrameBufferObject();
    ~GFrameBufferObject();

    bool InitFBO(int width, int height, GColorRGBA color);
    void BindFBO();
    void UnbindFBO();

    bool mIsFboSupported = true;
    GTexture mFboTexture;
    unsigned char* mFboBuffer = nullptr;
    GLuint mFboFrame = 0;
    GLuint mFboStencil = 0;
    GLint mSaveFboFrame = 0;
    GTransform mSavedTransform;
};

#endif