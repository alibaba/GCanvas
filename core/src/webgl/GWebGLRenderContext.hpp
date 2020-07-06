/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GWEBGLRENDERCONTEXT_H
#define GCANVAS_GWEBGLRENDERCONTEXT_H

#include <stdio.h>
#include <string>
#include <unordered_set>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>
#include "../export.h"
#include "GGL.h"


namespace gcanvas
{

namespace WebGL
{

class JSBindingPixels
{
public:
    API_EXPORT  JSBindingPixels();
    API_EXPORT ~JSBindingPixels();

    JSBindingPixels(JSBindingPixels &&v)
    {
        textureId = v.textureId;
        pixels = v.pixels;
        width = v.width;
        height = v.height;
        
        v.textureId = 0;
        v.pixels = nullptr;
        v.width = 0;
        v.height = 0;
    }
    
    
    uint32_t    textureId;
    uint8_t*    pixels;
    short       width;
    short       height;
};


//Fectch pixels form JSBinding instance with instanceID
using FetchPixelsFunc = std::function< void (void*obj, uint64_t, JSBindingPixels*) >;
//bindframebuffer null
using BindFramebufferNullFunc = std::function< void(void*obj) >;
//glResource
using GLResourceFunc = std::function< void(void*obj, int, GLuint) >;


enum GLResourceType
{
    Buffer,
    FrameBuffer,
    Program,
    RenderBuffer,
    Shader,
    Texture,
    VertexArray
};

class GWebGLRenderContext
{
public:
    API_EXPORT GWebGLRenderContext(std::string contextId);
    API_EXPORT GWebGLRenderContext(void* data, std::string contextId);
    API_EXPORT ~GWebGLRenderContext();

    void FetchPixels(uint64_t instanceId, int format, JSBindingPixels* pixels);
    void BindFramebufferNull();

    API_EXPORT void ReleaseResource();

    FetchPixelsFunc mFetchPixelsFunc;
    BindFramebufferNullFunc mBindFramebufferNullFunc;
    GLResourceFunc mAddGLResourceFunc;

    API_EXPORT void AddGLResource(GLResourceType resType, GLuint v);
    void DeleteGLResource(GLResourceType resType, GLuint v);

    long mDrawCall;
    bool mUnpackFlipY_WebGL;
    bool mPremultiplyAlpha_WebGL;
    std::atomic_bool mNeed_draw;

protected:
    void* mData;
    std::string mContextId;

    std::unordered_set<GLuint>  mBufferSet;
    std::unordered_set<GLuint>  mFrameBuferSet;
    std::unordered_set<GLuint>  mProgramSet;
    std::unordered_set<GLuint>  mRenderBufferSet;
    std::unordered_set<GLuint>  mShaderSet;
    std::unordered_set<GLuint>  mTextureSet;
    std::unordered_set<GLuint>  mVertexArraySet;

};

} //namespace WebGL

} //namespace gcanvas

#endif /* GCANVAS_GWEBGLRENDERCONTEXT_H */
