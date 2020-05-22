#include "GRenderContext.h"
#include <fstream>
#include <cmath>
#include "GFrameBufferObject.h"
#include "GLUtil.h"
#include "Util.h"
#include <EGL/egl.h>
#include <vector>

namespace NodeBinding
{

    static std::vector<GRenderContext *> g_RenderContextVC;
    // static EGLDisplay g_eglDisplay = nullptr;
    // static EGLContext g_eglContext = nullptr;

    GRenderContext::GRenderContext(int width, int height)
        : mWidth(width), mHeight(height), mRatio(2.0)
    {
        GCanvasConfig config = {true, false};
        this->mCanvas = std::make_shared<gcanvas::GCanvas>("node-gcanvas", config, nullptr);
        mCanvasWidth = width * mRatio;
        mCanvasHeight = height * mRatio;
    }

    GRenderContext::GRenderContext(int width, int height, int ratio)
        : mWidth(width), mHeight(height), mRatio(ratio)
    {
        GCanvasConfig config = {true, true};
        this->mCanvas = std::make_shared<gcanvas::GCanvas>("node-gcanvas", config, nullptr);
        mCanvasWidth = width * mRatio;
        mCanvasHeight = height * mRatio;
    }

    void GRenderContext::initRenderEnviroment()
    {
#ifdef CONTEXT_ES20
        EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
#endif

        // Step 1 - Get the default display.
        if (!mEglDisplay)
        {
            mEglDisplay = eglGetDisplay((EGLNativeDisplayType)0);
        }

        // Step 2 - Initialize EGL.
        eglInitialize(mEglDisplay, 0, 0);

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

        if (!mEglContext)
        {
#ifdef CONTEXT_ES20
            mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, ai32ContextAttribs);
#else
            g_eglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, NULL);
#endif
        }

        // Step 8 - Bind the context to the current thread
        if (!eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext))
        {
            printf("eglMakeCurrent fail \n");
            exit(-1);
        }
        // end of standard gl context setup

        // Step 9 - create framebuffer object
        glGenFramebuffers(1, &mFboId);
        glBindFramebuffer(GL_FRAMEBUFFER, mFboId);

        glGenRenderbuffers(1, &mRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, mCanvasWidth, mCanvasHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderBuffer);
        glGenRenderbuffers(1, &mDepthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, mCanvasWidth, mCanvasHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);

        // check FBO status
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Problem with OpenGL framebuffer after specifying color render buffer: n%x \n", status);
            exit(-1);
        }
        else
        {
            printf("FBO create success %p, fboId=%d, renderbufferId=%d depthbufferId=%d\n", this, mFboId, mRenderBuffer, mDepthRenderbuffer);
        }

        GLint format = 0, type = 0;
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
        this->initCanvas();

        // g_RenderContextVC.push_back(this);
    }

    void GRenderContext::makeCurrent()
    {
        if (mEglContext != EGL_NO_CONTEXT && mEglContext != EGL_NO_DISPLAY)
        {
            if (!eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext))
            {
                printf("eglMakeCurrent fail \n");
                exit(-1);
            }
        }
    }

    void GRenderContext::initCanvas()
    {
        mCanvas->CreateContext();
        mCanvas->GetGCanvasContext()->SetClearColor(gcanvas::StrValueToColorRGBA("red"));
        mCanvas->GetGCanvasContext()->ClearScreen();
        mCanvas->GetGCanvasContext()->SetDevicePixelRatio(mRatio);
        mCanvas->OnSurfaceChanged(0, 0, mCanvasWidth, mCanvasHeight);
        mCanvas->GetGCanvasContext()->SetFillStyle("#ff00ff");
        mCanvas->GetGCanvasContext()->FillRect(0, 0, mCanvasWidth, mCanvasHeight);
    }
    void GRenderContext::drawFrame()
    {
        printf("drawframe() \n");
        mCanvas->drawFrame();
        this->drawCount++;
    }

    void GRenderContext::render2file(std::string fileName, PIC_FORMAT format)
    {
        int size = 4 * mCanvasWidth * mCanvasHeight;
        unsigned char *inputData = new unsigned char[size];
        if (!inputData)
        {
            printf("Error: allocate inputData memeroy faied! \n");
            return;
        }

        glReadPixels(0, 0, mCanvasWidth, mCanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, inputData);
        for (int i = 0; i < 4; i++)
        {
            printf("the pxiels is %d \n", inputData[i]);
        }
        unsigned char *data = new unsigned char[4 * mWidth * mHeight];
        if (!data)
        {
            printf("Error: allocate data memeroy faied! \n");
            delete inputData;
            inputData = nullptr;
            return;
        }

        gcanvas::PixelsSampler(mCanvasWidth, mCanvasHeight, (int *)inputData, mWidth, mHeight, (int *)data);
        gcanvas::FlipPixel(data, mWidth, mHeight);

        delete inputData;
        inputData = nullptr;

        if (format == PNG_FORAMT)
        {
            encodePixelsToPNGFile(fileName + ".png", data, mWidth, mHeight);
        }
        else if (format == JPEG_FORMAT)
        {
            encodePixelsToJPEGFile(fileName + ".jpg", data, mWidth, mHeight);
        }
        delete data;
        data = nullptr;
    }

    void GRenderContext::destoryRenderEnviroment()
    {
        if (this->mFboId != 0)
        {
            glDeleteFramebuffers(1, &this->mFboId);
        }
        if (this->mRenderBuffer != 0)
        {
            glDeleteRenderbuffers(1, &this->mRenderBuffer);
        }
        if (this->mDepthRenderbuffer != 0)
        {
            glDeleteRenderbuffers(1, &this->mDepthRenderbuffer);
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

        if (g_RenderContextVC.size() <= 0)
        {
            if (mEglDisplay != EGL_NO_DISPLAY)
            {
                eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                if (mEglContext != EGL_NO_CONTEXT)
                {
                    eglDestroyContext(mEglDisplay, mEglContext);
                }
                eglTerminate(mEglDisplay);
            }

            mEglDisplay = EGL_NO_DISPLAY;
            mEglContext = EGL_NO_CONTEXT;
        }
    }

    void GRenderContext::recordTextures(int textureId)
    {
        this->textures.push_back(textureId);
    }

    void GRenderContext::BindFBO()
    {
        GLint curFBOId = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFBOId);
        if (curFBOId != mFboId)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
        }
    }

    GRenderContext::~GRenderContext()
    {
        this->destoryRenderEnviroment();
    }
} // namespace NodeBinding
