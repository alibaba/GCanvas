#include <stdio.h>
#include <stdlib.h>
#include <string>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <EGL/egl.h>
#include <iostream>
#include <GCanvas.hpp>
#include "lodepng.h"

void ConvertPixelsToPng(std::string filename, uint8_t* buffer, int width, int height)
{
    unsigned error = lodepng::encode(filename,  buffer,  width,  height);
    if(error) {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error)  << filename << std::endl;
    }
}

int main(int argc, char *argv[])
{
    #define CONTEXT_ES20

    #ifdef CONTEXT_ES20
        EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
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

    if (iConfigs != 1) {
        printf("Error: eglChooseConfig(): config not found \n");
        exit(-1);
    }

    // Step 6 - Create a surface to draw to.
    EGLSurface eglSurface;
    // eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,  (EGLNativeWindowType)NULL, NULL);
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig,   NULL);
    // Step 7 - Create a context.
    EGLContext eglContext;
    #ifdef CONTEXT_ES20
        eglContext = eglCreateContext(eglDisplay, eglConfig, NULL,ai32ContextAttribs);
    #else
        eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
    #endif

    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    // end of standard gl context setup

    // Step 9 - create framebuffer object
    GLuint fboId = 0;
    GLuint renderBufferWidth = 300;
    GLuint renderBufferHeight = 300;

    // create a framebuffer objec g++  testopengloffscreen.cc lodepng.cc  -lGLESv2 -lglfw -lEGLt
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

     GLuint renderBuffer;
     glGenRenderbuffers(1, &renderBuffer);
     glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
     glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB565, renderBufferWidth,renderBufferHeight);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,renderBuffer);
      GLuint depthRenderbuffer;
      glGenRenderbuffers(1, &depthRenderbuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,     renderBufferWidth, renderBufferHeight);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

      // check FBO status
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE) {
          printf("Problem with OpenGL framebuffer after specifying color render buffer: n%xn", status);
      } else {
          printf("FBO creation succeddedn \n ");
     }

     GLint format = 0, type = 0;
     glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
     glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);


 gcanvas:: GCanvas c("id",{false,true},nullptr);
 c.CreateContext();
 c.OnSurfaceChanged(0,0,renderBufferWidth ,renderBufferHeight);   
 c.mCanvasContext->SetFillStyle("#ff0000");
 c.mCanvasContext->FillRect(0,0,renderBufferWidth,renderBufferHeight);
 c.drawFrame();

  // commit the clear to the offscreen surface
//   eglSwapBuffers(eglDisplay, eglSurface);

  // You should put your own calculation code here based on format/type
  // you discovered above
  int size = 4 * renderBufferHeight * renderBufferWidth;
  unsigned char *data = new unsigned char[size];

  printf("size %d \n", size);

  // in my case, I got back a buffer that was RGB565
  glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA,GL_UNSIGNED_BYTE, data);


  ConvertPixelsToPng("a.png",data,renderBufferWidth,renderBufferHeight);

  delete data;
}