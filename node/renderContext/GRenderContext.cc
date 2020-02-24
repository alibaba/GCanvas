#include "GRenderContext.h"
#include <fstream>
#include <cmath>
namespace NodeBinding
{
GRenderContext::GRenderContext(int width, int height) : mWidth(width), mHeight(height)
{
    GCanvasConfig config = {true, true};
    this->mCanvas = std::make_shared<gcanvas::GCanvas>("benchmark", config, nullptr);
}

void GRenderContext::initRenderEnviroment()
{
#ifdef CONTEXT_ES20
    EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
#endif

    // Step 1 - Get the default display.
    mEglDisplay = eglGetDisplay((EGLNativeDisplayType)0);

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

#ifdef CONTEXT_ES20
    mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, ai32ContextAttribs);
#else
    mEglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
#endif

    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
    // end of standard gl context setup

    // Step 9 - create framebuffer object

    glGenFramebuffers(1, &mFboId);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboId);

    glGenRenderbuffers(1, &mRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderBuffer);

    glGenRenderbuffers(1, &mDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);

    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Problem with OpenGL framebuffer after specifying color render buffer: n%xn", status);
    }
    else
    {
        // printf("FBO creation succeddedn \n ");
    }

    GLint format = 0, type = 0;
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
    this->initCanvas();
}
void GRenderContext::initCanvas()
{
    mCanvas->CreateContext();
    mCanvas->GetGCanvasContext()->SetClearColor(gcanvas::StrValueToColorRGBA("white"));
    mCanvas->GetGCanvasContext()->ClearScreen();
    mCanvas->OnSurfaceChanged(0, 0, mWidth, mHeight);
}
void GRenderContext::drawFrame()
{
    mCanvas->drawFrame();
    this->drawCount++;
}

void GRenderContext::render2file(std::string fileName, PIC_FORMAT format)
{
    int size = 4 * this->mWidth * this->mHeight;
    unsigned char *data = nullptr;
    data = new unsigned char[size];
    glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
    if (data)
    {
        if (format == PNG_FORAMT)
        {
            encodePixelsToFile(fileName + ".png", data, mWidth, mHeight);
        }
        else if (format == JPEG_FORMAT)
        {
            encodePixelsToJPEGFile(fileName + ".jpg", data, mWidth, mHeight);
        }
    }
    if (data)
    {
        delete data;
        data = nullptr;
    }
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
    if (mEglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mEglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(mEglDisplay, mEglContext);
        }
        if (mEglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(mEglDisplay, mEglSurface);
        }
        eglTerminate(mEglDisplay);
    }
    mEglDisplay = EGL_NO_DISPLAY;
    mEglContext = EGL_NO_CONTEXT;
    mEglSurface = EGL_NO_SURFACE;
}

void GRenderContext::recordTextures(int textureId)
{
    this->textures.push_back(textureId);
}

GRenderContext::~GRenderContext()
{
    this->destoryRenderEnviroment();
}
} // namespace NodeBinding
