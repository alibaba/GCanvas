#include "GBenchMark.h"
GBenchMark::GBenchMark(int width, int height, std::shared_ptr<gcanvas::GCanvas> canvas) : mWidth(width), mHeight(height), mCanvas(canvas)
{
}

void GBenchMark::intilGLOffScreenEnviroment()
{
#ifdef CONTEXT_ES20
    EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
#endif

    // Step 1 - Get the default display.
    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);

    // Step 2 - Initialize EGL.
    eglInitialize(eglDisplay, 0, 0);

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
    eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1,
                    &iConfigs);

    if (iConfigs != 1)
    {
        printf("Error: eglChooseConfig(): config not found \n");
        exit(-1);
    }

    // Step 6 - Create a surface to draw to.
    EGLSurface eglSurface;
    // eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,  (EGLNativeWindowType)NULL, NULL);
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, NULL);
    // Step 7 - Create a context.
    EGLContext eglContext;
#ifdef CONTEXT_ES20
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);
#else
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
#endif

    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    // end of standard gl context setup

    // Step 9 - create framebuffer object
    GLuint fboId = 0;

    // create a framebuffer objec g++  testopengloffscreen.cc lodepng.cc  -lGLESv2 -lglfw -lEGLt
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
    GLuint depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Problem with OpenGL framebuffer after specifying color render buffer: n%xn", status);
    }
    else
    {
        printf("FBO creation succeddedn \n ");
    }

    GLint format = 0, type = 0;
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
    this->initGcanvas();
}

void GBenchMark::initGcanvas()
{
    if (mCanvas)
    {
        mCanvas->CreateContext();
        mCanvas->OnSurfaceChanged(0, 0, mWidth, mHeight);
    }
}
void GBenchMark::render2file(std::string caseName)
{
    int size = 4 * mWidth * mHeight;
    unsigned char *data = new unsigned char[size];
    glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
    encodePixelsToFile(caseName+".png", data, mWidth, mHeight);
    delete data;
}
float GBenchMark::computeRatioWithW3C(std::string caseName)
{
    std::vector<unsigned char> w3cImage;
    std::vector<unsigned char> gcanvasImage;

    decodeFile2Pixels(this->w3cPrefix+caseName+".png", w3cImage);
    decodeFile2Pixels(caseName+".png", gcanvasImage);
    int N = std::min(w3cImage.size(), gcanvasImage.size());
    int errorCount = 0;
    int rightCount = 0;
    if(N==0)
       return 0.0;
    for (int i = 0; i < N; i++)
    {
        if (w3cImage[i] != gcanvasImage[i])
            errorCount++;
        else
            rightCount++;
    }
    return 1.0f * rightCount / N;
}


void GBenchMark::run(std::string caseName, std::shared_ptr<GBenchMarkCase> oneCase)
{
    oneCase->draw(this->mCanvas,mWidth,mHeight);
    mCanvas->drawFrame();
    this->render2file(oneCase->getCaseName());

   float ratio = this->computeRatioWithW3C(caseName);
   std::cout << "------------------"<< std::endl;
   std::cout << "the case name is "<<caseName << std::endl;
   std::cout << "------------------" << std::endl;
   std::cout << "the correct ratio is " << ratio << std::endl;
   oneCase->ratio=ratio;
}


