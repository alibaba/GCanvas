#include "GRenderContext.h"
#include <fstream>
#include <cmath>
#include "GFrameBufferObject.h"
#include "GLUtil.h"
#include "Util.h"
#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
// #include <GL/glut.h>
#include <vector>

namespace NodeBinding
{

    static std::vector<GRenderContext *> g_RenderContextVC;
    static EGLContext g_eglContext = EGL_NO_CONTEXT;
    static EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;

    GRenderContext::GRenderContext(int width, int height)
        : mWidth(width), mHeight(height), mRatio(2.0), mEglDisplay(EGL_NO_DISPLAY)
    {
        GCanvasConfig config = {true, false};
        this->mCanvas = std::make_shared<gcanvas::GCanvas>("node-gcanvas", config, nullptr);
        mCanvasWidth = width * mRatio;
        mCanvasHeight = height * mRatio;
    }

    GRenderContext::GRenderContext(int width, int height, int ratio)
        : mWidth(width), mHeight(height), mRatio(ratio), mEglDisplay(EGL_NO_DISPLAY)
    {
        GCanvasConfig config = {true, true};
        this->mCanvas = std::make_shared<gcanvas::GCanvas>("node-gcanvas", config, nullptr);
        mCanvasWidth = width * mRatio;
        mCanvasHeight = height * mRatio;
    }

    void GRenderContext::initRenderEnviroment()
    {
        InitSharedContextIfNot();
#ifdef CONTEXT_ES20
        EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
#endif
        // Step 1 - Get the default display.
        if (mEglDisplay == EGL_NO_DISPLAY)
        {
            mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            // Step 2 - Initialize EGL.
            eglInitialize(mEglDisplay, 0, 0);
        }

#ifdef CONTEXT_ES20
        // Step 3 - Make OpenGL ES the current API.
        eglBindAPI(EGL_OPENGL_ES_API);

        // Step 4 - Specify the required configuration attributes.
        EGLint pi32ConfigAttribs[5];
        pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
        pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
        pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
        pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
        pi32ConfigAttribs[4] = EGL_NONE;
#else
        EGLint pi32ConfigAttribs[3];
        pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
        pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
        pi32ConfigAttribs[2] = EGL_NONE;
#endif

        // Step 5 - Find a config that matches all requirements.
        int iConfigs;
        EGLConfig eglConfig;
        eglChooseConfig(mEglDisplay, pi32ConfigAttribs, &eglConfig, 1,
                        &iConfigs);
        if (iConfigs != 1)
        {
            printf("Error: eglChooseConfig(): config not found \n");
            exit(-1);
        }

        // Step 6 - Create a surface to draw to.

        mEglSurface = eglCreatePbufferSurface(mEglDisplay, eglConfig, NULL);
        // Step 7 - Create a context.

        if (g_eglContext == EGL_NO_CONTEXT)
        {
#ifdef CONTEXT_ES20
            mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, ai32ContextAttribs);
#else
            mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, NULL);
#endif
            // g_eglContext = mEglContext;
        }
        else
        {
#ifdef CONTEXT_ES20
            mEglContext = eglCreateContext(mEglDisplay, eglConfig, g_eglContext, ai32ContextAttribs);
#else
            mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, NULL);
#endif
        }

        // Step 8 - Bind the context to the current thread
        if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) != EGL_TRUE)
        {
            EGLint error = eglGetError();
            printf("eglMakeCurrent fail the error is %x\n", error);
            exit(-1);
        }
        // end of standard gl context setup

        // Step 9 - create framebuffer object
        this->mFboIdSrc = this->createFBO(mCanvasWidth, mCanvasHeight, &this->mRenderBufferIdSrc, &this->mDepthRenderbufferIdSrc);
        this->mFboIdDes = this->createFBO(mWidth, mHeight, &this->mRenderBufferIdDes, &this->mDepthRenderbufferIdDes);
        glBindFramebuffer(GL_FRAMEBUFFER, this->mFboIdSrc);

        GLint format = 0, type = 0;
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
        this->initCanvas();
        g_RenderContextVC.push_back(this);
    }

    GLuint GRenderContext::createFBO(int fboWidth, int fboHeight, GLuint *renderBufferId, GLuint *depthBufferId)
    {
        GLuint fboId2Ret = 0;
        glGenFramebuffers(1, &fboId2Ret);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId2Ret);

        glGenRenderbuffers(1, renderBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, *renderBufferId);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, fboWidth, fboHeight);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *renderBufferId);

        glGenRenderbuffers(1, depthBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, *depthBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, fboWidth, fboHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depthBufferId);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *depthBufferId);
        // check FBO status
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            EGLint error = eglGetError();
            printf("Problem with OpenGL framebuffer after specifying color render buffer:  %x  the glError is %x \n", status, error);
            exit(-1);
        }
        else
        {
            printf("FBO Create success %p, FboId=%d, RenderbufferId=%d DepthbufferId=%d\n", this, fboId2Ret, *renderBufferId, *depthBufferId);
        }
        return fboId2Ret;
    }


    void GRenderContext::makeCurrent()
    {
        if (mEglContext != EGL_NO_CONTEXT && mEglDisplay != EGL_NO_DISPLAY)
        {
            //判断当前上下文是否是该canvas的上下文,减少makecurrent的切换过程
            EGLContext currentContext = eglGetCurrentContext();
            EGLSurface currentSurface = eglGetCurrentSurface(EGL_DRAW);
            if (mEglContext == currentContext && mEglSurface == currentSurface)
            {
                this->BindFBO();
                return;
            }
            else
            {
                if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) != EGL_TRUE)
                {
                    printf("eglMakeCurrent fail \n");
                    exit(-1);
                }
                this->BindFBO();
                return;
            }
        }
    }

    void GRenderContext::initCanvas()
    {
        mCanvas->CreateContext();
        mCanvas->GetGCanvasContext()->SetClearColor(gcanvas::StrValueToColorRGBA("transparent"));
        mCanvas->GetGCanvasContext()->ClearScreen();
        mCanvas->GetGCanvasContext()->SetDevicePixelRatio(mRatio);
        mCanvas->OnSurfaceChanged(0, 0, mCanvasWidth, mCanvasHeight);
    }
    void GRenderContext::drawFrame()
    {
        mCanvas->drawFrame();
        this->drawCount++;
    }

    int GRenderContext::getImagePixelPNG(std::vector<unsigned char> &in)
    {
        unsigned char *data = new unsigned char[4 * mWidth * mHeight];
        int ret = this->readPixelAndSampleFromCurrentCtx(data);
        if (ret == 0)
        {
            encodePNGInBuffer(in, data, mWidth, mHeight);
        }
        else
        {
            delete data;
            data = nullptr;
            return -1;
        }
        delete data;
        data = nullptr;
        return 0;
    }

    int GRenderContext::getImagePixelJPG(unsigned char **in, unsigned long &size)
    {
        unsigned char *data = new unsigned char[4 * mWidth * mHeight];
        int ret = this->readPixelAndSampleFromCurrentCtx(data);
        if (ret == 0)
        {
            encodeJPEGInBuffer(in, size, data, mWidth, mHeight);
        }
        else
        {
            delete data;
            data = nullptr;
            return -1;
        }
        delete data;
        data = nullptr;
        return 0;
    }

    int GRenderContext::readPixelAndSampleFromCurrentCtx(unsigned char *data)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->mFboIdSrc); // src FBO (multi-sample)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->mFboIdDes);
        glBlitFramebuffer(0, 0, mCanvasWidth, mCanvasHeight, // src rect
                          0, 0, mWidth, mHeight,             // dst rect
                          GL_COLOR_BUFFER_BIT,               // buffer mask
                          GL_LINEAR);
        glBlitFramebuffer(0, 0, mCanvasWidth, mCanvasHeight, // src rect
                          0, 0, mWidth, mHeight,             // dst rect
                          GL_DEPTH_BUFFER_BIT,               // buffer mask
                          GL_LINEAR);
        glBlitFramebuffer(0, 0, mCanvasWidth, mCanvasHeight, // src rect
                          0, 0, mWidth, mHeight,             // dst rect
                          GL_STENCIL_BUFFER_BIT,             // buffer mask
                          GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, this->mFboIdDes);
        glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
        return 0;
    }
    void GRenderContext::render2file(std::string fileName, PIC_FORMAT format)
    {
        unsigned char *data = new unsigned char[4 * mWidth * mHeight];
        int ret = this->readPixelAndSampleFromCurrentCtx(data);
        if (ret == 0)
        {
            if (format == PNG_FORAMT)
            {
                int width = mWidth;
                int height = mHeight;
                encodePixelsToPNGFile(fileName + ".png", data, width, height);
            }
            else if (format == JPEG_FORMAT)
            {
                encodePixelsToJPEGFile(fileName + ".jpg", data, mWidth, mHeight);
            }
        }
        else
        //todo
        {
        }
        delete data;
        data = nullptr;
    }

    void GRenderContext::destoryRenderEnviroment()
    {
        if (this->mFboIdSrc != 0)
        {
            glDeleteFramebuffers(1, &this->mFboIdSrc);
        }
        if (this->mFboIdDes != 0)
        {
            glDeleteFramebuffers(1, &this->mFboIdDes);
        }
        if (this->mRenderBufferIdSrc != 0)
        {
            glDeleteRenderbuffers(1, &this->mRenderBufferIdSrc);
        }
        if (this->mRenderBufferIdDes != 0)
        {
            glDeleteRenderbuffers(1, &this->mRenderBufferIdDes);
        }
        if (this->mDepthRenderbufferIdSrc != 0)
        {
            glDeleteRenderbuffers(1, &this->mDepthRenderbufferIdSrc);
        }
        if (this->mDepthRenderbufferIdDes != 0)
        {
            glDeleteRenderbuffers(1, &this->mDepthRenderbufferIdDes);
        }
        if (this->textures.size() > 0)
        {
            glDeleteTextures(this->textures.size(), (GLuint *)&textures[0]);
        }

        if (mEglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(mEglDisplay, mEglSurface);
            mEglSurface = EGL_NO_SURFACE;
        }
        std::vector<GRenderContext *>::iterator iter = find(g_RenderContextVC.begin(), g_RenderContextVC.end(), this);
        if (iter != g_RenderContextVC.end())
        {
            g_RenderContextVC.erase(iter);
        }
        if (mEglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (mEglContext != EGL_NO_CONTEXT)
            {
                eglDestroyContext(mEglDisplay, mEglContext);
            }
        }
        mEglDisplay = EGL_NO_DISPLAY;
        mEglContext = EGL_NO_CONTEXT;
        //todo 考虑何时释放sharedContext?不用释放？等待进程销毁
        // if (g_RenderContextVC.size() == 0)
        // {
        //     g_eglContext = EGL_NO_CONTEXT;
        // }
    }

    void GRenderContext::recordTextures(int textureId)
    {
        this->textures.push_back(textureId);
    }

    void GRenderContext::BindFBO()
    {
        GLint curFBOId = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFBOId);
        if (curFBOId != mFboIdSrc)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mFboIdSrc);
            // printf("checkout  fbo value is %d\n", mFboIdSrc);
        }
        else
        {
        }
    }

    void GRenderContext::recordImageTexture(std::string url, int textureId)
    {
        this->imageTextureMap[url] = textureId;
    }

    void GRenderContext::InitSharedContextIfNot()
    {
        if (g_eglDisplay == EGL_NO_DISPLAY && g_eglContext == EGL_NO_CONTEXT)
        {
#ifdef CONTEXT_ES20
            EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
#endif
            g_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            // Step 2 - Initialize EGL.
            eglInitialize(g_eglDisplay, 0, 0);

            // Step 3 - Make OpenGL ES the current API.
            eglBindAPI(EGL_OPENGL_ES_API);

            // Step 4 - Specify the required configuration attributes.
            EGLint pi32ConfigAttribs[5];
            pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
            pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
            pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
            pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
            pi32ConfigAttribs[4] = EGL_NONE;

            // Step 5 - Find a config that matches all requirements.
            int iConfigs;
            EGLConfig eglConfig;
            eglChooseConfig(g_eglDisplay, pi32ConfigAttribs, &eglConfig, 1,
                            &iConfigs);
            if (iConfigs != 1)
            {
                printf("Error: eglChooseConfig(): config not found \n");
                exit(-1);
            }

            // Step 7 - Create a context.

            if (g_eglContext == EGL_NO_CONTEXT)
            {
#ifdef CONTEXT_ES20
                g_eglContext = eglCreateContext(g_eglDisplay, eglConfig, NULL, ai32ContextAttribs);
#else
                g_eglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, NULL);
#endif
            }
        }
    }
    int GRenderContext::getTextureIdByUrl(std::string url)
    {
        if (this->imageTextureMap.find(url) == imageTextureMap.end())
        {
            return -1;
        }
        else
        {
            return this->imageTextureMap[url];
        }
    }

    GRenderContext::~GRenderContext()
    {
        this->destoryRenderEnviroment();
    }
} // namespace NodeBinding
