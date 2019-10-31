/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GFrameBufferObject.h"
#include "GCanvas2dContext.h"

#include "support/Log.h"
#include "gcanvas/GConvert.h"
#include "gcanvas/GFrameBufferObject.h"

#include <EGL/egl.h>

/* OpenGL ES extension functions. */
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT = NULL;


#define OES_PACKED_DEPTH_STENCIL "GL_OES_packed_depth_stencil"


bool extension_available(const char* extName)
{
    std::string extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    return extensions.find(extName) != std::string::npos;
}




bool GFrameBufferObject::InitFBO(int width, int height, GColorRGBA color, bool enableMsaa, std::vector<GCanvasLog> *errVec)
{
    mWidth = width;
    mHeight = height;

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
            AppendErrorLogInfo(errVec, "gl_render_to_texture_msaa_not_support", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
        }
    }

    useMsaa = enableMsaa && support_render_texture_msaa;
    LOG_W("initFBO> useMsaa=%d, support_render_texture_msaa=%d", useMsaa, support_render_texture_msaa);

    glGenFramebuffers(1, &mFboFrame);
    if (mFboFrame <= 0)
    {
        AppendErrorLogInfo(errVec, "gen_framebuffer_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboFrame);

    mFboTexture.SetWidth(width);
    mFboTexture.SetHeight(height);
    mFboTexture.SetFormat(GL_RGBA);
    mFboTexture.CreateTexture(nullptr,errVec);
    mFboTexture.Bind();

    if (useMsaa)
    {
        glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFboTexture.GetTextureID(), 0, samples);
        GLenum  err = glGetError();
        if (err)
        {
            // 埋点记录开启msaa失败
            AppendErrorLogInfo(errVec, "fbo_enable_msaa_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
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
    bool OES_packed_depth_stencil;
    const char *extString = (const char *) glGetString(GL_EXTENSIONS);
    if (extString == nullptr) { // 扩展获取失败，默认采用
        OES_packed_depth_stencil = false;
        LOG_E("initFBO get extension packed depth null, use=GL_STENCIL_INDEX8");
    } else { // ios平台只支持GL_DEPTH24_STENCIL8_OES (部分Android机型也支持此格式)
        OES_packed_depth_stencil = (strstr(extString, OES_PACKED_DEPTH_STENCIL) != 0);
    }

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

        AppendErrorLogInfo(errVec, "fbo_status_check_fail", "<function:%s, status:%d, glGetError:%x>", __FUNCTION__,  status, glGetError());
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
}