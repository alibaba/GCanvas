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

#ifdef ANDROID
#include <EGL/egl.h>

/* OpenGL ES extension functions. */
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT = NULL;

#endif

#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"


bool extension_available(const char* extName)
{
    std::string extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    return extensions.find(extName) != std::string::npos;
}



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


/**
 * 删除FBO资源
 */
void GFrameBufferObject::DeleteFBO()
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



bool GFrameBufferObject::InitFBO(int width, int height, GColorRGBA color, std::string appInfo)
{
    return InitFBO(width, height, color, false, appInfo);
}


bool GFrameBufferObject::InitFBO(int width, int height, GColorRGBA color, bool enableMsaa, std::string appInfo)
{
    mWidth = width;
    mHeight = height;

#ifdef ANDROID

    LOG_W("InitFBO> start ---");

    // 是否使用msaa
    bool useMsaa;
    // 在使用msaa条件下，开启msaa是否成功
    bool openMsaaOk = false;
    GLint samples = 0;

    bool support_render_texture_msaa = true;
    if (enableMsaa) {
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
        // samples逻辑，保证不超过GL_MAX_SAMPLES_EXT
        if (samples > 4 || samples <= 0)
        {
            samples = 4;
        }
        // 默认samples为4，是否需要处理？
        support_render_texture_msaa = extension_available("GL_EXT_multisampled_render_to_texture");
        glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC) eglGetProcAddress(
                "glFramebufferTexture2DMultisampleEXT");
        if (!glFramebufferTexture2DMultisampleEXT) {
            LOG_E("Couldn't get function pointer to glFramebufferTexture2DMultisampleEXT()");
            support_render_texture_msaa = false;
        }

        glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC) eglGetProcAddress(
                "glRenderbufferStorageMultisampleEXT");
        if (!glRenderbufferStorageMultisampleEXT) {
            LOG_E("Couldn't get function pointer to glRenderbufferStorageMultisampleEXT()");
            support_render_texture_msaa = false;
        }

        if (!support_render_texture_msaa)
        {
            // 埋点记录不支持支持msaa的数据(开启msaa后)
            LOG_EXCEPTION(appInfo.c_str(), "gl_render_to_texture_msaa_not_support", "");
        }
    }

    useMsaa = enableMsaa && support_render_texture_msaa;
    LOG_W("initFBO> useMsaa=%d, support_render_texture_msaa=%d", useMsaa, support_render_texture_msaa);

    glGenFramebuffers(1, &mFboFrame);
    if (mFboFrame <= 0)
    {
        LOG_EXCEPTION(appInfo.c_str(), "gen_framebuffer_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);

    mFboTexture.SetWidth(width);
    mFboTexture.SetHeight(height);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.CreateTexture(nullptr, appInfo.c_str());
    mFboTexture.Bind();

    if (useMsaa)
    {
        glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFboTexture.GetTextureID(), 0, samples);
        GLenum  err = glGetError();
        if (err)
        {
            // 埋点记录开启msaa失败
            LOG_EXCEPTION(appInfo.c_str(), "fbo_enable_msaa_fail", "<function:%s, glGetError:%x>",
                          __FUNCTION__, err);
            openMsaaOk = false;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFboTexture.GetTextureID(), 0);
        }
        else
        {
            openMsaaOk = true;
        }
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFboTexture.GetTextureID(), 0);
    }

    // Renderbuffer depth & stencil 配置
    const char *extString = (const char *) glGetString(GL_EXTENSIONS);
    // ios平台只支持GL_DEPTH24_STENCIL8_OES (部分Android机型也支持此格式)
    bool OES_packed_depth_stencil = (strstr(extString, OES_PACKED_DEPTH_STENCIL) != 0);
    GLenum  depthFormat;
    if (OES_packed_depth_stencil) {
        depthFormat = GL_DEPTH24_STENCIL8_OES;
    }
    else
    {
        depthFormat = GL_STENCIL_INDEX8;
    }

    glGenRenderbuffers(1, &mFboStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, mFboStencil);
    if (useMsaa && openMsaaOk)
    {
        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, samples, depthFormat, mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, depthFormat /*GL_DEPTH_COMPONENT16*/,
                              mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }

    if (depthFormat == GL_DEPTH24_STENCIL8_OES)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, mFboStencil);
        LOG_E("initFBO depthFormat=GL_DEPTH24_STENCIL8_OES");
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, mFboStencil);
 

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        mIsFboSupported = false;
        LOG_D("FBO fail ! fboFrame = %d, fboTexture = %d, fboStencial = "
                      "%d, w = %d, h = %d",
              mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
              mFboTexture.GetWidth(), mFboTexture.GetHeight());
        
        LOG_EXCEPTION(appInfo.c_str(), "fbo_status_check_fail", "<function:%s, status:%d, glGetError:%x>", __FUNCTION__,  status, glGetError());
    }
    else
    {
        mIsFboSupported = true;
        LOG_D("FBO complete! fboFrame = %d, fboTexture = %d, fboStencial = "
                      "%d, w = %d, h = %d",
              mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
              mFboTexture.GetWidth(), mFboTexture.GetHeight());
    }


    GLClearScreen(color);

    UnbindFBO();

    return mIsFboSupported;
#endif


#ifdef IOS

    glGenFramebuffers(1, &mFboFrame);
    if (mFboFrame <= 0) {
        LOG_EXCEPTION(appInfo.c_str(), "gen_framebuffer_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);

    mFboTexture.SetWidth(width);
    mFboTexture.SetHeight(height);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.CreateTexture(nullptr, appInfo.c_str());
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

        LOG_D("FBO fail ! fboFrame = %d, fboTexture = %d, fboStencial = "
                      "%d, w = %d, h = %d",
              mFboFrame, mFboTexture.GetTextureID(), mFboStencil,
              mFboTexture.GetWidth(), mFboTexture.GetHeight());

        LOG_EXCEPTION(appInfo.c_str(), "fbo_status_check_fail", "<function:%s, status:%d, glGetError:%x>", __FUNCTION__,  status, glGetError());
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

    glEnable(GL_DEPTH_TEST);

    UnbindFBO();

    return mIsFboSupported;

#endif

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

        GLClearScreen(GColorTransparent);
    }
}


void GFrameBufferObject::GLClearScreen(GColorRGBA color)
{
    glClearColor(color.rgba.r, color.rgba.g, color.rgba.b, color.rgba.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
        fbo->InitFBO(twoPowerWidth, twoPowerHeight, GColorTransparent);
        fbo->SetSize(width, height);
        return fbo;
    }

    GFrameBufferObjectPtr fbo(i->second, deleter);
    fbo->SetSize(width, height);
    mPool.erase(i);
    return fbo;
}

