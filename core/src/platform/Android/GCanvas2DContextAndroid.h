/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCANVAS2DCONTEXTANDROID_H
#define GCANVAS_GCANVAS2DCONTEXTANDROID_H


#include "GCanvas2dContext.h"
#include "GFont.h"
#include "GSystemFontInformation.h"
#include "GShaderManager.h"

#include <cstdint>

class GFontCache;


class GCanvas2DContextAndroid : public GCanvasContext {


public:


    GCanvas2DContextAndroid(uint32_t width, uint32_t h, GCanvasConfig &config);


    virtual ~GCanvas2DContextAndroid();


    void InitFBO() override;


    API_EXPORT GShader *FindShader(const char *) override;


    API_EXPORT void ClearColor();


    API_EXPORT void GetRawImageData(int width, int height, uint8_t *pixels);


    API_EXPORT void BeginDraw(bool is_first_draw = false);


    API_EXPORT void EndDraw();


    API_EXPORT GTexture *GetFBOTextureData();


    API_EXPORT void CopyFBO(GFrameBufferObject &srcFbo, GFrameBufferObject &destFbo);


    API_EXPORT void
    CopyImageToCanvas(int width, int height, const unsigned char *rgbaData, int imgWidth,
                      int imgHeight);


    API_EXPORT int BindImage(const unsigned char *rgbaData, GLint format, unsigned int width,
                             unsigned int height);


    API_EXPORT void DrawFrame(bool clear);


    API_EXPORT void ResizeCopyUseFbo(int width, int height);


    API_EXPORT  void ResizeCopyUseImage(int width, int height,
                                        const unsigned char *rgbaData, int imgWidth, int imgHeight);

    void SetEnableFboMsaa(bool v) { mEnableFboMsaa = v; }


    GShaderManager* GetShaderManager();
protected:


    void ResetGLBeforeCopyFrame(int dest_fbo_w, int dest_fbo_h);


    void RestoreGLAfterCopyFrame();


    bool InitializeGLShader() override;


    // void GLBlend(GCompositeOperation op, GCompositeOperation alphaOp);


    void DrawFBO(std::string fboName, GCompositeOperation compositeOp = COMPOSITE_OP_SOURCE_OVER,
                 float sx = 0, float sy = 0, float sw = 1, float sh = 1, float dx = 0, float dy = 0,
                 float dw = 1, float dh = 1);


private:

    bool mEnableFboMsaa = false;
    GShaderManager *mShaderManager = nullptr;

    GFontCache *mFontCache = nullptr;


};


#endif //GCANVAS_GCANVAS2DCONTEXTANDROID_H