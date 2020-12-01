/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#define CONTEXT_ES20
#ifndef GRENDERCONTEXT_H
#define GRENDERCONTEXT_H
#include <iostream>
#include <GCanvas.hpp>
#include "lodepng.h"
#include <functional>
#include <unordered_map>
#include "GConvert.h"
// #include "GWebGLRenderContext.hpp"
#include "NodeBindingUtil.h"
#include "Util.h"
#include "GFrameBufferObject.h"

namespace NodeBinding
{
    extern void encodePixelsToPNGFile(std::string filename, uint8_t *buffer, int width, int height);
    extern void decodeFile2Pixels(std::string filename, std::vector<unsigned char> &image);
    extern void encodePixelsToJPEGFile(std::string filename, uint8_t *buffer, int width, int height);
    extern void encodePNGInBuffer(std::vector<unsigned char> &in, unsigned char *data, int width, int height);
    extern void encodeJPEGInBuffer(unsigned char **in, unsigned long &size, unsigned char *data, int width, int height, int quality);
    class GRenderContext
    {
    public:
        GRenderContext(int width, int height);
        GRenderContext(int width, int height, int ratio);
        virtual ~GRenderContext();
        void initRenderEnviroment();
        void render2file(std::string caseName, PIC_FORMAT format);
        void drawFrame();
        void setContextType(std::string type);
        void resize(int width, int height);
        inline GCanvasContext *getCtx2d() { return mCanvas2d->GetGCanvasContext(); }
        // inline std::shared_ptr<gcanvas::WebGL::GWebGLRenderContext> getCtxWebGL(){ return mCanvasWebGL; }


        inline int  getWdith() { return mWidth; }
        inline int  getHeight() { return mHeight; }
        void destoryRenderEnviroment();
        void recordTextures(int textureId);
        void recordImageTexture(std::string url, int textureId);
        int getTextureIdByUrl(std::string url);
        void BindFBO();
        void makeCurrent();
        int getImagePixelPNG(std::vector<unsigned char> &in);
        int getImagePixelJPG(unsigned char **data, unsigned long &size);
        int readPixelAndSampleFromCurrentCtx(unsigned char *data);
        int getDpi();

    private:
        std::shared_ptr<gcanvas::GCanvas> mCanvas2d;
        // std::shared_ptr<gcanvas::WebGL::GWebGLRenderContext> mCanvasWebGL;
        void initCanvas2d();
        void initCanvasWebGL();

        int mHeight;
        int mWidth;
        int mCanvasHeight;
        int mCanvasWidth;
        int mDpi;
        int drawCount = 0;
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLContext mEglContext;
        GLuint mFboIdSrc = 0;
        GLuint mRenderBufferIdSrc = 0;
        GLuint mDepthRenderbufferIdSrc = 0;
        std::vector<int> textures;
        std::unordered_map<std::string, int> imageTextureMap;
        static void InitSharedContextIfNot();
        GLuint createFBO(int fboWidth, int fboHeigh, GLuint *renderBufferId, GLuint *depthBufferId);
        GLuint mFboIdDes = 0;
        GLuint mRenderBufferIdDes = 0;
        GLuint mDepthRenderbufferIdDes = 0;
    };
} // namespace NodeBinding

#endif