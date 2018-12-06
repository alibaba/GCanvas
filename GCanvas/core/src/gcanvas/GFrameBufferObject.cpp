/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GFrameBufferObject.h"
#include "../support/Log.h"
#include "GConvert.h"

#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"

GFrameBufferObject::GFrameBufferObject()
{

}


GFrameBufferObject::~GFrameBufferObject()
{
    if (mFboFrame)
    {
        glDeleteFramebuffers(1, &mFboFrame);
        mFboFrame = 0;
    }

    if (mFboStencil)
    {
        glDeleteRenderbuffers(1, &mFboStencil);
        mFboStencil = 0;
    }

    GLuint textureId = mFboTexture.GetTextureID();
    glDeleteTextures(1, &textureId);
    mFboTexture.Unbind();
}

bool GFrameBufferObject::InitFBO(int width, int height, GColorRGBA color)
{
    mWidth = width;
    mHeight = height;

    glGenFramebuffers(1, &mFboFrame);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);

    mFboTexture.SetWidth(width);
    mFboTexture.SetHeight(height);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.CreateTexture(nullptr);
    mFboTexture.Bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mFboTexture.GetTextureID(), 0);

    const char *extString = (const char *) glGetString(GL_EXTENSIONS);
    bool OES_packed_depth_stencil = (strstr(extString, OES_PACKED_DEPTH_STENCIL) != 0);

    if (OES_packed_depth_stencil)
    {
        glGenRenderbuffers(1, &mFboStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, mFboStencil);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH24_STENCIL8_OES /*GL_DEPTH_COMPONENT16*/,
                              mFboTexture.GetWidth(), mFboTexture.GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
    }
    else
    {
        glGenRenderbuffers(1, &mFboStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, mFboStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8,
                              mFboTexture.GetWidth(), mFboTexture.GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
    }
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        mIsFboSupported = false;

        LOG_D("failed to make complete framebuffer object %x", status);
        LOG_D("FBO fail ! fboFrame = %d, fboTexture = %d, fboStencial = "
                      "%d, w = %d, h = %d",
              mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
              mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }
    else
    {
        mIsFboSupported = true;

        LOG_D("FBO complete! fboFrame = %d, fboTexture = %d, fboStencial = "
                      "%d, w = %d, h = %d",
              mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
              mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }


    glClearColor(color.rgba.r, color.rgba.g, color.rgba.b, color.rgba.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    UnbindFBO();

    return mIsFboSupported;
}

void GFrameBufferObject::BindFBO()
{
    if (!mIsFboSupported)
    {
        return;
    }

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);

    if (!mFboTexture.IsValidate())
    {
        mFboTexture.CreateTexture(nullptr);
        mFboTexture.Bind();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               mFboTexture.GetTextureID(), 0);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

}

void GFrameBufferObject::UnbindFBO()
{
    if (!mIsFboSupported)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mSaveFboFrame);
}

int GFrameBufferObject::DetachTexture()
{
    int id = mFboTexture.GetTextureID();
    mFboTexture.Detach();
    return id;
}

inline void GFrameBufferObjectDeleter(GFrameBufferObjectPool::Map *pool, GFrameBufferObject *fbo)
{
    pool->insert(GFrameBufferObjectPool::Map::value_type(
            GFrameBufferObjectPool::Key(fbo->Width(), fbo->Height()), fbo));
}

inline int GetNoSmall2PowNum(int num)
{
    if (num <= 1) return num;
    if (num & (num - 1))
    {
        int r = 1;
        while (num)
        {
            num >>= 1;
            r <<= 1;
        }
        return r;
    }
    else
    {
        return num;
    }
}


GFrameBufferObjectPtr GFrameBufferObjectPool::GetFrameBuffer(int width, int height)
{
    using namespace std::placeholders;  // 对于 _1, _2, _3...

    auto deleter = std::bind(GFrameBufferObjectDeleter, &mPool, _1);

    int twoPowerWidth = GetNoSmall2PowNum(width);
    int twoPowerHeight = GetNoSmall2PowNum(height);

    auto i = mPool.find(Key(twoPowerWidth, twoPowerHeight));
    if (i == mPool.end())
    {

        GFrameBufferObjectPtr fbo(new GFrameBufferObject(), deleter);
        fbo->InitFBO(twoPowerWidth, twoPowerHeight,
                     StrValueToColorRGBA("transparent"));
        fbo->SetSize(width, height);
        return fbo;
    }

    GFrameBufferObjectPtr fbo(i->second, deleter);
    fbo->SetSize(width, height);
    mPool.erase(i);
    return fbo;
}

