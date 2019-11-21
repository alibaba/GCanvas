/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvas2DContextAndroid.h"

#include "GFontCache.h"
#include "GPoint.h"
#include "GFontManagerAndroid.h"
#include "gcanvas/GFrameBufferObject.h"
#include "gcanvas/GShaderManager.h"
#include "support/Log.h"



GCanvas2DContextAndroid::GCanvas2DContextAndroid(uint32_t w, uint32_t h, GCanvasConfig &config) :
        GCanvasContext(w, h, config) {
    // init shader manager
    mShaderManager = nullptr;

    // init font cache
    mFontCache = new GFontCache(*mFontManager);
    GFontManagerAndroid *ptr = static_cast<GFontManagerAndroid *>(mFontManager);
    if (ptr != nullptr) {
        ptr->SetFontCache(mFontCache);
    } else {
        LOG_D("mFontManager init fail");
    }
}


GCanvas2DContextAndroid::~GCanvas2DContextAndroid() {
    // TODO 验证父类 delete 顺序
    if (mFontCache != nullptr) {
        delete mFontCache;
        mFontCache = nullptr;
    }

    if (mShaderManager != nullptr) {
        delete mShaderManager;
        mShaderManager = nullptr;
    }
}


bool GCanvas2DContextAndroid::InitializeGLShader() {
    if (mShaderManager == nullptr) {
        mShaderManager = new GShaderManager();
    }

    return GCanvasContext::InitializeGLShader();
}


GShader *GCanvas2DContextAndroid::FindShader(const char *name) {
    return mShaderManager->programForKey(name);
}


void GCanvas2DContextAndroid::InitFBO() {
    if (0 != mContextType) return;

    if (!mConfig.useFbo) {
        return;
    }

    if (!mIsFboSupported) {
        return;
    }

    if (mFboMap.find(DefaultFboName) == mFboMap.end()) {
        std::vector<GCanvasLog> logVec;
        mIsFboSupported = mFboMap[DefaultFboName].InitFBO(mWidth, mHeight,
                                                          GColorTransparent, mEnableFboMsaa,
                                                          &logVec);
        LOG_EXCEPTION_VECTOR(mHooks, mContextId.c_str(), logVec);
    }
}


void GCanvas2DContextAndroid::ClearColor() {
    GColorRGBA c = GColorTransparentWhite;
    glClearColor(c.rgba.r, c.rgba.g, c.rgba.b, c.rgba.a);
    glClear(GL_COLOR_BUFFER_BIT);
}


void GCanvas2DContextAndroid::GetRawImageData(int width, int height, uint8_t *pixels) {
    SendVertexBufferToGPU();
    glReadPixels(0, 0, width, height, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);
}


//void GCanvas2DContextAndroid::GLBlend(GCompositeOperation op, GCompositeOperation alphaOp)
//{
//    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);
//    GBlendOperationFuncs alphaFuncs = GCompositeOperationFuncs(alphaOp);
//
//    glBlendFuncSeparate(funcs.source, funcs.destination,
//                        alphaFuncs.source, alphaFuncs.destination);
//}


void GCanvas2DContextAndroid::BeginDraw(bool is_first_draw) {
    if (mConfig.useFbo) {
        // 深度test 一直开启
        glEnable(GL_DEPTH_TEST);
        BindFBO();
    } else {
        if (is_first_draw) {
            // 非fbo模式下，首次渲染前执行一次清屏
            ClearScreen();
        }
    }
}


void GCanvas2DContextAndroid::EndDraw() {
    if (!mConfig.useFbo) {
        return;
    }

    // FBO模式
    UnbindFBO();
    // 拷贝fbo前清空默认frameBuffer内容 (因为所有显示内容是叠加绘制在fbo上的)
    ClearScreen();
    // 拷贝FBO
    DrawFBO(DefaultFboName);
}


GTexture *GCanvas2DContextAndroid::GetFBOTextureData() {
    return &(mFboMap[DefaultFboName].mFboTexture);
}

/**
 * 在改变canvas view大小时进行内容复制
 * 新建fbo, 并进行fbo复制
 */
void GCanvas2DContextAndroid::ResizeCopyUseFbo(int width, int height) {
    bool sizeChanged = mWidth != width || height != mHeight;
    mWidth = width;
    mHeight = height;
    if (!sizeChanged) {
        // LOG_D("execResizeWithGLStatus: not really changed, return");
        return;
    }

    bool shouldChangeDimension = (mCanvasWidth <= 0 && mCanvasHeight <= 0);
    if (0 == mContextType && mIsFboSupported) {
        // 1.切换回主fbo
        UnbindFBO();
        // 2.新创建fbo
        GFrameBufferObject *newFbo = new GFrameBufferObject();
        mIsFboSupported = newFbo->InitFBO(mWidth, mHeight, GColorTransparent, mEnableFboMsaa,nullptr);
        // 是否存在旧的fbo, 如果有则拷贝旧fbo并删除旧的
        if (mFboMap.find(DefaultFboName) != mFboMap.end()) {
            // 3.拷贝fbo
            CopyFBO(mFboMap[DefaultFboName], *newFbo);
            // 4.删除旧的fbo，并回收fbo资源
            mFboMap.erase(DefaultFboName);
        }
        // 5.复制新FBO到map内(复制对象内存, 方法内的对象依然存在)
        mFboMap[DefaultFboName] = *newFbo;

        // 6.切换到新FBO上(可以绘制)
        BindFBO();
        // TODO newFbo对象也要想办法回收
        // (直接释会导致FBO被删除，会导致功能异常)
    }

    // 如果画布尺寸为0，表示要跟随surface变换，继续以(0,0)更新dimension，计算新的变换矩阵
    if (shouldChangeDimension) {
        SetCanvasDimension(0, 0);
    }

    if (mContextType == 0) {
        // 更新viewport变换
        glViewport(0, 0, mWidth, mHeight);
    }
}


void GCanvas2DContextAndroid::CopyFBO(GFrameBufferObject &srcFbo, GFrameBufferObject &destFbo) {
    if (!mIsFboSupported) {
        return;
    }

    if (nullptr == mCurrentState || nullptr == mCurrentState->mShader) {
        return;
    }

    destFbo.BindFBO();

    ResetGLBeforeCopyFrame(destFbo.mWidth, destFbo.mHeight);

    GColorRGBA color = GColorWhite;
    glBindTexture(GL_TEXTURE_2D, srcFbo.mFboTexture.GetTextureID());
    PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;

    RestoreGLAfterCopyFrame();

    // 拷贝完成，切回主fbo
    UnbindFBO();
}


void
GCanvas2DContextAndroid::ResizeCopyUseImage(int width, int height, const unsigned char *rgbaData,
                                            int imgWidth,
                                            int imgHeight) {
    bool sizeChanged = (mWidth != width) || (height != mHeight);
    if (!sizeChanged) {
        // LOGE("sizeChanged not changed");
        return;
    }

    // 更新尺寸
    mWidth = width;
    mHeight = height;

    bool shouldChangeDimension = (mCanvasWidth <= 0 && mCanvasHeight <= 0);
    // 开始复制图像
    if (rgbaData != nullptr) {
        CopyImageToCanvas(width, height, rgbaData, imgWidth, imgHeight);
    }

    // 如果画布尺寸为0，表示要跟随surface变换，继续以(0,0)更新dimension，计算新的变换矩阵
    if (shouldChangeDimension) {
        SetCanvasDimension(0, 0);
    }
    // 更新viewport变换
    glViewport(0, 0, width, height);
}


void GCanvas2DContextAndroid::CopyImageToCanvas(int width, int height,
                                                const unsigned char *rgbaData, int imgWidth,
                                                int imgHeight) {
    ResetGLBeforeCopyFrame(width, height);
    // 绑定图像纹理
    GLuint glID = BindImage(rgbaData, GL_RGBA, (GLuint) imgWidth, (GLuint) imgHeight);
    GColorRGBA color = GColorWhite;
    PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;

    glDeleteTextures(1, &glID);

    RestoreGLAfterCopyFrame();
}


int
GCanvas2DContextAndroid::BindImage(const unsigned char *rgbaData, GLint format, unsigned int width,
                                   unsigned int height) {
    if (nullptr == rgbaData)
        return (GLuint) -1;

    GLenum glerror = 0;
    GLuint glID;
    glGenTextures(1, &glID);
    glerror = glGetError();
    if (glerror) {
//        LOG_EXCEPTION("", "gen_texture_fail", "<function:%s, glGetError:%x>", __FUNCTION__,
//                      glerror);
    }
    glBindTexture(GL_TEXTURE_2D, glID);
    glerror = glGetError();
    if (glerror) {
//        LOG_EXCEPTION("", "bind_texture_fail", "<function:%s, glGetError:%x>", __FUNCTION__,
//                      glerror);
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, rgbaData);
    glerror = glGetError();
    if (glerror) {
//        LOG_EXCEPTION("", "glTexImage2D_failglTexImage2D_fail", "<function:%s, glGetError:%x>",
//                      __FUNCTION__, glerror);
    }

    return glID;
}


void
GCanvas2DContextAndroid::
DrawFBO(std::string fboName, GCompositeOperation compositeOp, float sx, float sy, float sw,
        float sh, float dx, float dy, float dw, float dh) {
    if (!mIsFboSupported) {
        return;
    }

    if (nullptr == mCurrentState || nullptr == mCurrentState->mShader) {
        return;
    }


    Save();
    glViewport(mX, mY, mWidth, mHeight);

    GFrameBufferObject &fbo = mFboMap[fboName];

    UseDefaultRenderPipeline();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    DoSetGlobalCompositeOperation(compositeOp, compositeOp);

    GColorRGBA color = GColorWhite;
    mCurrentState->mShader->SetOverideTextureColor(0);
    mCurrentState->mShader->SetHasTexture(1);
    fbo.mFboTexture.Bind();

    PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
    glDrawArrays(GL_TRIANGLES, 0, mVertexBufferIndex);
    mVertexBufferIndex = 0;

    if (HasClipRegion()) {
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    glViewport(0, 0, mWidth, mHeight);

    Restore();
}


void GCanvas2DContextAndroid::ResetGLBeforeCopyFrame(int width, int height) {
    Save();
    GColorRGBA c = mClearColor;
    SetClearColor(GColorTransparent);
    ClearScreen();
    SetClearColor(c);
    glViewport(0, 0, width, height);

    UseDefaultRenderPipeline();
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    GCompositeOperation compositeOp = COMPOSITE_OP_SOURCE_OVER;
    DoSetGlobalCompositeOperation(compositeOp, compositeOp);

    mCurrentState->mShader->SetOverideTextureColor(0);
    mCurrentState->mShader->SetHasTexture(1);
    mCurrentState->mShader->SetTransform(GTransformIdentity);
}


void GCanvas2DContextAndroid::RestoreGLAfterCopyFrame() {
    if (HasClipRegion()) {
        glEnable(GL_STENCIL_TEST);
    }
    glEnable(GL_DEPTH_TEST);

    Restore();
}


void GCanvas2DContextAndroid::DrawFrame(bool clear) {
    SendVertexBufferToGPU();
    if (clear) {
        ClearGeometryDataBuffers();
    }
}

GShaderManager *GCanvas2DContextAndroid::GetShaderManager() {
    return this->mShaderManager;
}