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

#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"

GFrameBufferObject::GFrameBufferObject()
{

}


GFrameBufferObject::~GFrameBufferObject()
{
    delete mFboBuffer;
    mFboBuffer = nullptr;
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
    glGenFramebuffers(1, &mFboFrame);
    BindFBO();

    GLuint tid = 0;
    glGenTextures(1, &tid);
    mFboTexture.SetTextureID(tid);
    mFboTexture.SetWidth(width);
    mFboTexture.SetHeight(height);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.Bind();
    mFboBuffer = new unsigned char[width*height*4];

    glTexImage2D(GL_TEXTURE_2D, 0, mFboTexture.GetFormat(), mFboTexture.GetWidth(),
                 mFboTexture.GetHeight(), 0, mFboTexture.GetFormat(),
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mFboTexture.GetTextureID(), 0);

    const char *extString = (const char *)glGetString(GL_EXTENSIONS);
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
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);


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
}

void GFrameBufferObject::UnbindFBO()
{
    if (!mIsFboSupported)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mSaveFboFrame);
}
