/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/
#include "GWebGLRenderContext.hpp"
#include "GWebGLRenderContextInner.hpp"
#include <algorithm>
#include "../support/Log.h"
#include "GGL.h"



namespace gcanvas {

namespace WebGL {

JSBindingPixels::JSBindingPixels():
    textureId(0), width(0), height(0), pixels(nullptr)
{

}

JSBindingPixels::~JSBindingPixels()
{

}


GWebGLRenderContext::GWebGLRenderContext(std::string contextId)
    : mContextId(contextId), mDrawCall(0),mNeed_draw(false),
    mUnpackFlipY_WebGL(false), mPremultiplyAlpha_WebGL(false)
{

}

GWebGLRenderContext::GWebGLRenderContext(void* data, std::string contextId)
    : mData(data), mContextId(contextId), mDrawCall(0),
      mNeed_draw(false), mUnpackFlipY_WebGL(false), mPremultiplyAlpha_WebGL(false)
{

}

GWebGLRenderContext::~GWebGLRenderContext()
{
    ReleaseResource();
}

void GWebGLRenderContext::FetchPixels(uint64_t instanceId, int format, JSBindingPixels* pixels)
{
    if (mFetchPixelsFunc)
    {
        mFetchPixelsFunc(mData, instanceId, pixels);
    }
}

void GWebGLRenderContext::BindFramebufferNull()
{
    if( mBindFramebufferNullFunc )
    {
        mBindFramebufferNullFunc(mData);
    }
}

void GWebGLRenderContext::AddGLResource(GLResourceType resType, GLuint v)
{
    if( mAddGLResourceFunc )
    {
        mAddGLResourceFunc( mData, resType, v );
    }
    
    switch (resType)
    {
        case GLResourceType::Buffer:
        {
            mBufferSet.insert(v);
            break;
        }
        case GLResourceType::FrameBuffer:
        {
            mFrameBuferSet.insert(v);
            break;
        }
        case GLResourceType::Program:
        {
            mProgramSet.insert(v);
            break;
        }
        case GLResourceType::RenderBuffer:
        {
            mRenderBufferSet.insert(v);
            break;
        }
        case GLResourceType::Shader:
        {
            mShaderSet.insert(v);
            break;
        }
        case GLResourceType::VertexArray:
        {
            mVertexArraySet.insert(v);
            break;
        }
        default:
        {
            break;
        }
    }
}

void GWebGLRenderContext::DeleteGLResource(GLResourceType resType, GLuint v)
{
    switch (resType)
    {
        case GLResourceType::Buffer:
        {
            mBufferSet.erase(v);
            break;
        }
        case GLResourceType::FrameBuffer:
        {
            mFrameBuferSet.erase(v);
            break;
        }
        case GLResourceType::Program:
        {
            mProgramSet.erase(v);
            break;
        }
        case GLResourceType::RenderBuffer:
        {
            mRenderBufferSet.erase(v);
            break;
        }
        case GLResourceType::Shader:
        {
            mShaderSet.erase(v);
            break;
        }
        case GLResourceType::VertexArray:
        {
            mVertexArraySet.erase(v);
            break;
        }
        default:
        {
            break;
        }
    }
}

void GWebGLRenderContext::ReleaseResource() {
    std::for_each(mBufferSet.begin(), mBufferSet.end(), [](GLuint v) {
        deleteBuffer(v);
    });
    mBufferSet.clear();

    std::for_each(mFrameBuferSet.begin(), mFrameBuferSet.end(), [](GLuint v) {
        deleteFramebuffer(v);
    });
    mFrameBuferSet.clear();

    std::for_each(mProgramSet.begin(), mProgramSet.end(), [](GLuint v) {
        deleteProgram(v);
    });
    mProgramSet.clear();

    std::for_each(mRenderBufferSet.begin(), mRenderBufferSet.end(), [](GLuint v) {
        deleteRenderbuffer(v);
    });
    mRenderBufferSet.clear();

    std::for_each(mShaderSet.begin(), mShaderSet.end(), [](GLuint v) {
        deleteShader(v);
    });
    mShaderSet.clear();

    std::for_each(mTextureSet.begin(), mTextureSet.end(), [](GLuint v) {
        deleteTexture(v);
    });
    mTextureSet.clear();
}




} //namespace WebGL
} //namespace gcanvas
