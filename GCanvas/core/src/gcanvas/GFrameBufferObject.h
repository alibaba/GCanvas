/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GFRAMEBUFFEROBJECT_H
#define GCANVAS_GFRAMEBUFFEROBJECT_H

#include "GTexture.h"
#include "GPoint.h"
#include "GTransform.h"

#include <map>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

struct GCanvasLog;

class GFrameBufferObject
{
public:
    GFrameBufferObject();

    ~GFrameBufferObject();


    bool InitFBO(int width, int height, GColorRGBA color, std::vector<GCanvasLog> *errVec = nullptr);

    bool InitFBO(int width, int height, GColorRGBA color, bool enableMsaa, std::vector<GCanvasLog> *errVec = nullptr);

    void BindFBO();

    void UnbindFBO();

    void DeleteFBO();

    int DetachTexture();

    void GLClearScreen(GColorRGBA color);


    int Width()
    {
        return mFboTexture.GetWidth();
    }

    int Height()
    {
        return mFboTexture.GetHeight();

    }

    void SetSize(int w, int h)
    {
        mWidth=w;
        mHeight=h;
    }
    int ExpectedWidth()
    {
        return mWidth;
    }

    int ExpectedHeight()
    {
        return mHeight;

    }

    bool mIsFboSupported = true;

    GTexture mFboTexture;
    GLuint mFboFrame = 0;
    GLuint mFboStencil = 0;
    GLint mSaveFboFrame = 0;
    GTransform mSavedTransform;

    int mWidth;
    int mHeight;

};


typedef std::shared_ptr<GFrameBufferObject> GFrameBufferObjectPtr;

class GFrameBufferObjectPool
{
public:
    typedef std::pair<int, int> Key;
    typedef std::multimap<Key, GFrameBufferObject*> Map;

public:
    GFrameBufferObjectPool() = default;

    ~GFrameBufferObjectPool()
    {
        for(auto& i : mPool)
        {
            delete i.second;
        }
    }

    GFrameBufferObjectPtr GetFrameBuffer(int width, int height);


private:

    Map mPool;
};

#endif /* GCANVAS_GFRAMEBUFFEROBJECT_H */
