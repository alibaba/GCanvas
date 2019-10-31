/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifdef GCANVAS_WEEX

#include "gcanvas/GShaderManager.h"
#include "GCanvasManager.h"
#include "GCanvasWeex.hpp"
#include "GCanvas2dContext.h"
#include "support/Encode.h"
#include "support/Util.h"

#include <stdio.h>

#ifdef ANDROID
#include "platform/Android/GCanvas2DContextAndroid.h"
#else
#include <sys/time.h>
#endif

namespace gcanvas {

GCanvasWeex::GCanvasWeex(std::string contextId, const GCanvasConfig &config) :
        GCanvas(contextId, config) {
    mCurrentTransform = GTransformIdentity,
            mFrames = 0;
    mFps = 0.0f;
    mContextLost = false;
    mResult = "";
#ifdef ANDROID
    sem_init(&mSyncSem, 0, 0);
#endif
}

GCanvasWeex::~GCanvasWeex() {

}

void GCanvasWeex::CreateContext() {
#ifdef ANDROID
    mCanvasContext = new GCanvas2DContextAndroid(0, 0, mConfig);
#else
    mCanvasContext = new GCanvasContext(0, 0, mConfig);
#endif
    mCanvasContext->mContextId = this->mContextId;
}

void GCanvasWeex::Clear() {
    LOG_D("GCanvasWeex::DoContextLost start.");
    // No need to clean up GL memory with glDeleteBuffers or glDeleteTextures.
    // It all gets blown away automatically when the context is lost.
    mContextLost = true;
    mTextureMgr.Clear();
#ifdef ANDROID
    clearCmdQueue();
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    LOG_D("GCanvasWeex::DoContextLost end.");
}


void GCanvasWeex::SetClearColor(const GColorRGBA &c) {
    mCanvasContext->SetClearColor(c);
}

void GCanvasWeex::SetDevicePixelRatio(const float ratio) {
    mCanvasContext->SetDevicePixelRatio(ratio);
}

int GCanvasWeex::GetContextType() {
    return mCanvasContext->mContextType;
}

void GCanvasWeex::SetContextType(int contextType) {
    mCanvasContext->mContextType = contextType;
}

GTexture *GCanvasWeex::GetFboTexture() {
    return mCanvasContext->GetFboTexture();
}


//GCanvas Weex
void GCanvasWeex::SetOrtho(int width, int height) {
}

void GCanvasWeex::SetTyOffsetFlag(bool flag) {
}

void GCanvasWeex::AddTexture(int textureGroupId, int glID, int width, int height) {
    LOG_D("AddTexture, Group ID = %d, GL ID = %d, width = %d, height = %d, context lost = %d",
          textureGroupId, glID, width, height, mContextLost);
    if (mContextLost) return;
    mTextureMgr.Append(textureGroupId, glID, width, height);
}

#ifdef IOS
void GCanvasWeex::AddOfflineTexture(int textureGroupId, int glID){
    if (mContextLost) return;
    
    std::map< int, int >::iterator iter = mOfflineTextures.find(glID);
    if (iter == mOfflineTextures.end())
    {
        mOfflineTextures.insert( std::pair< int, int >(textureGroupId, glID) );
    }
}
#endif

bool GCanvasWeex::AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                                unsigned int *pWidth, unsigned int *pHeight) {
    if (mContextLost) return false;
    return mTextureMgr.AppendPng(buffer, size, textureGroupId, pWidth, pHeight);
}

void GCanvasWeex::RemoveTexture(int textureId) {
    if (mContextLost) return;
    mTextureMgr.Remove(textureId);
}

void GCanvasWeex::drawFBO(std::string fboName, GCompositeOperation compositeOp,
                          float sx, float sy, float sw, float sh,
                          float dx, float dy, float dw, float dh) {
    if (!mCanvasContext->mIsFboSupported) {
        return;
    }

    if (nullptr == mCanvasContext->mCurrentState ||
        nullptr == mCanvasContext->mCurrentState->mShader) {
        return;
    }


    mCanvasContext->Save();
    glViewport(mCanvasContext->mX, mCanvasContext->mY, mCanvasContext->mWidth,
               mCanvasContext->mHeight);

    GFrameBufferObject &fbo = mCanvasContext->mFboMap[fboName];

    mCanvasContext->UseDefaultRenderPipeline();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    mCanvasContext->DoSetGlobalCompositeOperation(compositeOp, compositeOp);

    GColorRGBA color = GColorWhite;
    mCanvasContext->mCurrentState->mShader->SetOverideTextureColor(0);
    mCanvasContext->mCurrentState->mShader->SetHasTexture(1);
    fbo.mFboTexture.Bind();

    mCanvasContext->PushRectangle(-1, -1, 2, 2, 0, 0, 1, 1, color);
    mCanvasContext->mCurrentState->mShader->SetTransform(GTransformIdentity);
    glDrawArrays(GL_TRIANGLES, 0, mCanvasContext->mVertexBufferIndex);
    mCanvasContext->mVertexBufferIndex = 0;

    if (mCanvasContext->HasClipRegion()) {
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    glViewport(0, 0, mCanvasContext->mWidth, mCanvasContext->mHeight);

    mCanvasContext->Restore();
}


void GCanvasWeex::calculateFPS() {
    ++mFrames;
    if (mFrames >= mFps || mFrames >= 60) {
#ifdef ANDROID
        static clock_t tvLast;
        clock_t now = clock();
        clock_t delta = now - tvLast;
        if (delta >= CLOCKS_PER_SEC) {
            double dSeconds =
                    static_cast<double>(delta) / static_cast<double>(CLOCKS_PER_SEC);
            mFps = static_cast<float>(static_cast<double>(mFrames) / dSeconds);
            LOG_D("[calculateFPS] fps=%.1f", mFps);
        } else if (delta >= 0) {
            return; // skip if the interval is less than a second.
        }

        mFrames = 0;
        tvLast = now;
#endif

#ifdef IOS
        static timeval tvLast = {0, 0};
        struct timeval tvNow;
        gettimeofday(&tvNow, NULL);
        
        if (tvNow.tv_sec > tvLast.tv_sec)
        {
            double delta = (tvNow.tv_sec - tvLast.tv_sec) +
            (tvNow.tv_usec - tvLast.tv_usec) / 1000000;
            mFps = static_cast<float>(static_cast<double>(mFrames) / delta);
            
            mFrames = 0;
            tvLast = tvNow;
        }
#endif
    }
}

//for string cmd
void GCanvasWeex::Render(const char *renderCommands, int length) {
    if (mContextLost) {
        return;
    }
    LOG_D("GCanvasWeex::Render: mContextType: %d", GetContextType());
    if (0 != mCanvasContext->mContextType) {
        if (length > 0) {
            calculateFPS();
            LOG_E("GCanvasWeex::Render:[WebGL] renderCommands:%s", renderCommands);
            executeWebGLCommands(renderCommands, length);
        }
    } else {
        if (nullptr == mCanvasContext->mCurrentState ||
            nullptr == mCanvasContext->mCurrentState->mShader) {
            return;
        }

        mCanvasContext->BindFBO();
        if (length > 0) {
            calculateFPS();
            LOG_E("GCanvasWeex::Render:[2D] renderCommands:%s", renderCommands);
            execute2dCommands(renderCommands, length);
        }
        mCanvasContext->UnbindFBO();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        drawFBO(GCanvasContext::DefaultFboName);
    }
}

//for string cmd
void GCanvasWeex::DrawImage(int textureId, float sx, float sy, float sw, float sh,
                            float dx, float dy, float dw, float dh) {
    LOG_D("GCanvasWeex::DrawImage texture %d", textureId);
    const TextureGroup *ptr_group = mTextureMgr.Get(textureId);
    if (NULL == ptr_group) {
        LOG_E("GCanvasWeex::DrawImage texture %d is NULL", textureId);
        return;
    }
    const TextureGroup &group = *ptr_group;

    if (!group.IsSplit()) {
#ifdef IOS
        std::map< int, int >::iterator iter = mOfflineTextures.find(textureId);
        if (iter != mOfflineTextures.end())
        {
            mCanvasContext->DoScale(1, -1);
            mOfflineTextures.erase(iter);
        }
#endif
        mCanvasContext->DoDrawImage(group.mVecTexture[0]->GetWidth(),
                                   group.mVecTexture[0]->GetHeight(),
                                   group.mVecTexture[0]->GetGlID(), sx, sy, sw, sh, dx, dy, dw,
                                   dh);
    } else {
        int start_col = sx / group.mTileWidth;
        int stop_col =
                (sx + sw - 1) / group.mTileWidth; // length is start with 1,so need
        // to minus 1 before check
        int start_row = sy / group.mTileHeight;
        int stop_row = (sy + sh - 1) / group.mTileHeight;

        int block_count = (group.mSrcWidth - 1) / group.mTileWidth +
                          1; // spilt block per row

        LOG_D("[GCanvasWeex::drawImage] row:[%d~%d], col:[%d~%d]", start_row,
              stop_row, start_col, stop_col);

        float scale_h = dh / sh;
        float scale_w = dw / sw;
        for (int r = start_row; r <= stop_row; ++r) {
            int y1 = std::max(r * group.mTileHeight, static_cast<int>(sy));
            int y2 = std::min((r + 1) * group.mTileHeight, static_cast<int>(sy + sh));
            float new_h = (y2 - y1) * scale_h;

            for (int c = start_col; c <= stop_col; ++c) {
                int index = r * block_count + c;
                int x1 = std::max(c * group.mTileWidth, static_cast<int>(sx));
                int x2 = std::min((c + 1) * group.mTileWidth, static_cast<int>(sx + sw));
                LOG_D("[GCanvasWeex::drawImage] rc:(%d, %d), x:(%d, %d), y:(%d, %d)",
                      r, c, x1, x2, y1, y2);

                mCanvasContext->DoDrawImage(group.mVecTexture[index]->GetWidth(),
                                           group.mVecTexture[index]->GetHeight(),
                                           group.mVecTexture[index]->GetGlID(),
                                           x1 % group.mTileWidth, y1 % group.mTileHeight,
                                           x2 - x1, y2 - y1, dx + (x1 - sx) * scale_w,
                                           dy + (y1 - sy) * scale_h, (x2 - x1) * scale_w,
                                           new_h);
            }
        }
    }
}

//for string cmd
void GCanvasWeex::UsePatternRenderPipeline(int textureListId, int textureWidth, int textureHeight,
                                           const std::string &pattern, bool isStroke) {
    mCanvasContext->SendVertexBufferToGPU();

#ifdef IOS
    mCanvasContext->mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif

#ifdef ANDROID
    mCanvasContext->mCurrentState->mShader = ((GCanvas2DContextAndroid *) mCanvasContext)->GetShaderManager()->programForKey(
            "PATTERN");
#endif

    if (nullptr == mCanvasContext->mCurrentState->mShader) {
        return;
    }
    mCanvasContext->mCurrentState->mShader->Bind();

    GFillStyle *style = isStroke ? mCanvasContext->mCurrentState->mStrokeStyle
                                 : mCanvasContext->mCurrentState->mFillStyle;
    if (style != nullptr) {
        delete style;
        style = nullptr;
    }

    if (textureListId >= 0 && pattern != "") {
        style = new FillStylePattern(textureListId, pattern);
        if (isStroke) {
            mCanvasContext->mCurrentState->mStrokeStyle = style;
        } else {
            mCanvasContext->mCurrentState->mFillStyle = style;
        }
        mCanvasContext->mCurrentState->mShader->SetRepeatMode(
                ((FillStylePattern *) style)->GetPattern());

        mCanvasContext->mCurrentState->mFillColor = StrValueToColorRGBA("white");
        mCanvasContext->mCurrentState->mTextureId = textureListId;
        mCanvasContext->mCurrentState->mShader->SetTextureSize(textureWidth, textureHeight);
    }
}

//for string cmd
void GCanvasWeex::UsePatternRenderPipeline(int textureListId,
                                           const std::string &pattern, bool isStroke) {
    mCanvasContext->SendVertexBufferToGPU();

#ifdef IOS
    mCanvasContext->mCurrentState->mShader = GShaderManager::getSingleton()->programForKey("PATTERN");
#endif

#ifdef ANDROID
    mCanvasContext->mCurrentState->mShader = ((GCanvas2DContextAndroid *) mCanvasContext)->GetShaderManager()->programForKey(
            "PATTERN");
#endif

    mCanvasContext->mCurrentState->mShader->Bind();
    if (mCanvasContext->mCurrentState->mFillStyle != nullptr) {
        delete mCanvasContext->mCurrentState->mFillStyle;
        mCanvasContext->mCurrentState->mFillStyle = nullptr;
    }
    if (textureListId >= 0 && pattern != "") {
        mCanvasContext->mCurrentState->mFillStyle =
                new FillStylePattern(textureListId, pattern);
        mCanvasContext->mCurrentState->mShader->SetRepeatMode(
                ((FillStylePattern *) (mCanvasContext->mCurrentState->mFillStyle))->GetPattern());
        const Texture *texture = getTextureWithOneImage(
                mCanvasContext->mCurrentState->mFillStyle->GetTextureListID());
        if (texture) {
            texture->Bind();
            mCanvasContext->mCurrentState->mShader->SetTextureSize(texture->GetWidth(),
                                                                   texture->GetHeight());
        }
    }
    if (mCanvasContext->mCurrentState != nullptr) {
        mCanvasContext->SetTransformOfShader(mCanvasContext->mCurrentState->mTransform);
    }
}

void GCanvasWeex::parseSetTransForTextform(
        float v1, float v2,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    GTransform t;
    switch (parseMode) {
        case SCALE:
            t.a = v1;
            t.d = v2;
            break;
        case TRANSLATE:
            t.tx = v1;
            t.ty = v2;
            break;
        default: ASSERT(0);
            break;
    }

    if (concat) {
        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        *transOut = t;
    }
}


void
GCanvasWeex::PutImageData(
        const char *imageData,
        int dataLength,
        float
        dx,
        float dy,
        float sw,
        float sh,
        float dw,
        float dh) {
    LOG_D("[PutImageData] xy=(%f, %f), src_wh=(%f, %f), dest_wh=(%f, %f)",

          dx, dy, sw, sh, dw, dh);

    std::string &pixels_data = mTempStr;
    if ((unsigned int) dataLength > pixels_data.length())
        pixels_data.resize(dataLength);

    GLubyte *pixels = (GLubyte *) (pixels_data.c_str());

    gcanvas::Base64DecodeBuf(reinterpret_cast<char *>(pixels), imageData, dataLength);

    GLuint glID;
    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw, sh, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);

    mCanvasContext->DoDrawImage(sw, sh, glID, 0, 0, sw, sh, dx, dy, dw, dh);
    if (!mContextLost) glDeleteTextures(1, (const GLuint *) (&glID));
}

void
GCanvasWeex::GetImageData(int
                          x, int
                          y, int
                          w, int
                          h, bool
                          base64Encode,
                          std::string &pixelsData) {
    LOG_D("GCanvas.cpp::getImageData begin... xy=(%d, %d), wh=(%d, %d)", x,

          y, w, h);

    // bounds check the parameters
    // get the dimensions of the current viewport
    int results[4];
    glGetIntegerv(GL_VIEWPORT, results);

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (-1 == w) w = results[2];
    if (-1 == h) h = results[3];
    if ((x + w) > results[2]) {
        x = 0;
        w = results[2];
    }
    if ((y + h) > results[3]) {
        y = 0;
        h = results[3];
    }

    // flip y axis to be in openGL lower left origin
    y = results[3] - y - h;
    LOG_D("GCanvas.cpp::getImageData bounds... xy=(%d, %d), wh=(%d, %d)", x,
          y, w, h);

    // use glGetPixels to get the bits from the current frame buffer
    // Make the BYTE array, factor of 3 because it's RBG.
    int buf_size = 4 * w * h;
    std::string *ptr_data = NULL;
    if (base64Encode) {
        ptr_data = &mTempStr;
        if ((unsigned int) buf_size > ptr_data->length())
            ptr_data->resize(buf_size);
    } else {
        ptr_data = &pixelsData;
        ptr_data->resize(buf_size);
    }

    unsigned char *pixels = (unsigned char *) ptr_data->c_str();

    glFinish();
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    gcanvas::FlipPixel(pixels, w, h);

    if (base64Encode) {
        pixelsData.resize(gcanvas::Base64EncodeLen(buf_size));
        gcanvas::Base64EncodeBuf((char *) (pixelsData.c_str()), ptr_data->c_str(),
                                 buf_size);
    }
    LOG_D("GCanvas.cpp::getImageData end...[%d]=%s", buf_size,
          pixelsData.c_str());
}


const char *GCanvasWeex::parseTokens(const char *p, float *tokens, int iMaxCount) {
    int iToken = 0;

    while (*p && *p != ';' && iToken < iMaxCount) {
        tokens[iToken++] = fastFloat(p);
        while (*p && (*p != ',' && *p != ';')) {
            ++p;
        }
        if (*p == ',') ++p;
    }
    if (*p == ';') ++p;
    return p;
}

void GCanvasWeex::parseTokesOpt(float *tokens, const char **pp) {
    int iToken = 0;
    const char *position[10] = {nullptr};
    const char *p = *pp;
    position[iToken++] = p;
    for (; *p != ';'; p++) {
        if (',' == *p) {
            p++;
            position[iToken++] = p;
        }
    }
    for (int i = 0; i < iToken; i++) {
        tokens[i] = atof(position[i]);
    }
    *pp = p;
    return;
}

// From the current position, past semicolon or to end
// set the GL matrix if we have all the right data

const char *GCanvasWeex::parseSetTransformT(
        const char *p,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    if (concat) {
        GTransform t;
        parseTokesOpt(reinterpret_cast<float *>(&t), &p);

        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        parseTokesOpt(reinterpret_cast<float *>(transOut), &p);
    }

    if (*p == ';') ++p;

    return p;
}

const char *GCanvasWeex::parseSetTransform(
        const char *p,
        int parseMode,       // what to read: IDENTITY, FULL_XFORM, etc.
        bool concat,         // if true, concatenate, else replace.
        GTransform transIn, // the current xform
        GTransform *transOut) {
    float tokens[6] = {0, 0, 0, 0, 0, 0};

    // #define debug_Parsetokes 1
#ifdef debug_Parsetokes
    const char *p0 = p;
#endif

#if origin_code_realization
    int iToken = 0;
    while (*p && *p != ';' && iToken < 6)
    {
        tokens[iToken++] = atof(p);
        
        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }
    
#ifdef debug_Parsetokes
    
    FILE *flog = fopen("/storage/sdcard0/ParseTokesOrigin1.txt", "a+");
    fwrite((p0 - 1), 1, (p - p0 + 1 + 1), flog);
    char buffer[256] = {0};
    int len = snprintf(buffer, "\n %f,%f,%f,%f,%f,%f \n", tokens[0], tokens[1],
                       tokens[2], tokens[3], tokens[4], tokens[5]);
    fwrite(buffer, 1, len, flog);
    fclose(flog);
#endif
#else
    parseTokesOpt(tokens, &p);
#endif

    GTransform t;
    switch (parseMode) {
        case IDENTITY:
            break;
        case SET_XFORM:
            t.a = tokens[0];
            t.b = tokens[1];
            t.c = tokens[2];
            t.d = tokens[3];
            t.tx = tokens[4];
            t.ty = tokens[5];
            break;
        case SCALE:
            t.a = tokens[0];
            t.d = tokens[1];
            break;
        case ROTATE: {
            double angle = tokens[0];

            double ca = cos(angle);
            double sa = sin(angle);

            t.a = static_cast<float>(ca);
            t.b = static_cast<float>(sa);
            t.c = static_cast<float>(-sa);
            t.d = static_cast<float>(ca);
        }
            break;
        case TRANSLATE:
            t.tx = tokens[0];
            t.ty = tokens[1];
            break;
        default: ASSERT(0);
            break;
    }

    if (concat) {
        transOut->a = transIn.a * t.a + transIn.c * t.b;
        transOut->b = transIn.b * t.a + transIn.d * t.b;
        transOut->c = transIn.a * t.c + transIn.c * t.d;
        transOut->d = transIn.b * t.c + transIn.d * t.d;
        transOut->tx = transIn.a * t.tx + transIn.c * t.ty + transIn.tx;
        transOut->ty = transIn.b * t.tx + transIn.d * t.ty + transIn.ty;
    } else {
        *transOut = t;
    }

    if (*p == ';') ++p;
    return p;
}

const char *GCanvasWeex::parseDrawImage(const char *p, ClipStruct *clipOut) {
#if origin_code_realization
    float tokens[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int iToken = 0;
    
    while (*p && *p != ';' && iToken < 9)
    {
        if (iToken == 0)
        {
            // First token is the texture ID
            clipOut->textureID = FastInt(p);
        }
        else
        {
            tokens[iToken - 1] = FastFloat(p);
        }
        iToken++;
        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }
    
    clipOut->cx = tokens[0]; // cx
    clipOut->cy = tokens[1]; // cy
    clipOut->cw = tokens[2]; // cw
    clipOut->ch = tokens[3]; // ch
    clipOut->px = tokens[4]; // px
    clipOut->py = tokens[5]; // py
    clipOut->pw = tokens[6]; // pw
    clipOut->ph = tokens[7]; // ph
#else
    clipOut->textureID = atoi(p);

    for (; *p != ','; p++); // skip image.id
    p++;

    //    for (; *p != ','; p++); // skip image.src
    //    p++;

    parseTokesOpt(&clipOut->cx, &p);
#endif

    if (*p == ';') ++p;
    return p;
}

const char *GCanvasWeex::parseName(const char *p, std::string &name) {
    const char *old = p;
    while (*p && *p != ';') {
        ++p;
    }
    name.assign(old, p - old);
    if (*p == ';') ++p;
    return p;
}

const char *
GCanvasWeex::parseBindingPara(const char *p, std::string &name, float &width, float &height) {
    const char *old = p;
    while (*p && *p != ',') {
        ++p;
    }
    name.assign(old, p - old);

    if (*p == ',') ++p;


    width = fastFloat(p);
    while (*p && *p != ',') {
        ++p;
    }
    if (*p == ',') ++p;
    height = fastFloat(p);
    while (*p && *p != ';') {
        ++p;
    }
    if (*p == ';') ++p;
    return p;
}

const char *
GCanvasWeex::parseBindingPara(const char *p, std::string &name, float &sx, float &sy, float &sw,
                              float &sh,
                              float &dx, float &dy, float &dw, float &dh) {
    const char *old = p;
    while (*p && *p != ',') {
        ++p;
    }
    name.assign(old, p - old);

    if (*p == ',') ++p;

    float tokens[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    parseTokesOpt(tokens, &p);

    sx = tokens[0];
    sy = tokens[1];
    sw = tokens[2];
    sh = tokens[3];

    dx = tokens[4];
    dy = tokens[5];
    dw = tokens[6];
    dh = tokens[7];

    if (*p == ';') ++p;
    return p;
}

// From the current position, past semicolon or to end
const char *GCanvasWeex::parseUnknown(const char *p) {
    while (*p && *p != ';') {
        ++p;
    }
    if (*p == ';') ++p;
    return p;
}

const Texture *GCanvasWeex::getTextureWithOneImage(int aId) {
    const TextureGroup *texture_group = mTextureMgr.Get(aId);
    if (texture_group != nullptr) {
        return texture_group->Get(0);
    }
    return nullptr;
}

const char *GCanvasWeex::extractOneParameterFromCommand(char *outParameter,
                                                        const char *commands) {
    int i = 0;
    const char *p = commands;
    bool has_left_parentheses = false;
    while (*p != ';' && (has_left_parentheses || *p != ',')) {
        if (*p == '(') {
            has_left_parentheses = true;
        } else if (*p == ')') {
            has_left_parentheses = false;
        }
        outParameter[i++] = *p++;
    }
    outParameter[i] = '\0';
    return p;
}


void GCanvasWeex::execute2dCommands(const char *renderCommands, int length) {
    if (mContextLost) return;


    if (LOG_LEVEL_DEBUG == GetLogLevel()) {
        char log_tmp[128];
        snprintf(log_tmp, sizeof(log_tmp), "%s", renderCommands);
        LOG_D("[executeRenderCommands][%d]%s", strlen(renderCommands), log_tmp);
    }

    char tmpFont[256];
    char tmpText[1024];
    GTransform &action = mCurrentTransform;

    mCanvasContext->ClearGeometryDataBuffers();
//    ApplyTransform(1, 0, 0, 1, 0, mTyOffset);
//    action.a = 1, action.b = 0, action.c = 0, action.d = 1, action.tx = 0,
//    action.ty = mTyOffset;
    mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d, action.tx, action.ty);
    ClipStruct clip;
    const char *p = renderCommands;
    const char *end = renderCommands + length;
    while (p < end) {
        switch (*p) {
            case 't': {
                // setTransform
                p++;
                p = parseSetTransformT(p, SET_XFORM, false, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'd': {
                p++;
                // Load the clip
                p = parseDrawImage(p, &clip);

                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                mCanvasContext->UseDefaultRenderPipeline();
                this->DrawImage(clip.textureID, clip.cx, clip.cy, clip.cw, clip.ch,
                          clip.px, clip.py, clip.pw, clip.ph);
//                SendVertexBufferToGPU();

                break;
            }
            case 'Q': {
                p++;

                // bind FBO
                std::string name;
                float width, height;
                p = parseBindingPara(p, name, width, height);

                if (!mCanvasContext->mIsFboSupported) {
                    break;
                }
                GFrameBufferObject *fbo = nullptr;
                std::map<std::string, GFrameBufferObject>::iterator it = mCanvasContext->mFboMap.find(
                        name);
                if (it == mCanvasContext->mFboMap.end()) {
                    fbo = &(mCanvasContext->mFboMap[name]);
                    fbo->InitFBO(width * mCanvasContext->mDevicePixelRatio,
                                 height * mCanvasContext->mDevicePixelRatio,
                                 StrValueToColorRGBA("transparent_white"));
                } else {
                    fbo = &it->second;
                }
                fbo->BindFBO();
                fbo->mSavedTransform = action;
                glViewport(0, 0, width * mCanvasContext->mDevicePixelRatio,
                           height * mCanvasContext->mDevicePixelRatio);
                //CalculaSetDevicePixelRatioTransformIdentity;
//                action.a = 1, action.b = 0, action.c = 0, action.d = 1, action.tx = 0,
//                action.ty = (mHeight / mDevicePixelRatio - height);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d, action.tx,
                                               action.ty);
                break;
            }
            case 'Y': {
                p++;

                // unbind FBO
                std::string name;
                p = parseName(p, name);

                if (!mCanvasContext->mIsFboSupported) {
                    break;
                }
                mCanvasContext->SendVertexBufferToGPU();
                GFrameBufferObject &fbo = mCanvasContext->mFboMap[name];
                fbo.UnbindFBO();
                glViewport(0, 0, mCanvasContext->mWidth, mCanvasContext->mHeight);
                action = fbo.mSavedTransform;
                //CalculateProjectTransform(mWidth, mHeight);

                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d, action.tx,
                                               action.ty);
                break;
            }
            case 'I': {
                p++;

                // draw FBO
                std::string name;
                float sx, sy, sw, sh, dx, dy, dw, dh;
                p = parseBindingPara(p, name, sx, sy, sw, sh, dx, dy, dw, dh);
                if (!mCanvasContext->mIsFboSupported) {
                    break;
                }
                GTexture &texture = mCanvasContext->mFboMap[name].mFboTexture;
                float r = mCanvasContext->mDevicePixelRatio;
                drawFBO(name, COMPOSITE_OP_SOURCE_OVER,
                        sx * 2 * r / texture.GetWidth(),
                        sy * 2 * r / texture.GetHeight(),
                        sw * r / texture.GetWidth(),
                        sh * r / texture.GetHeight(),
                        dx * 2 * r / mCanvasContext->mWidth,
                        dy * 2 * r / mCanvasContext->mHeight,
                        dw * r / mCanvasContext->mWidth,
                        dh * r / mCanvasContext->mHeight);

                break;
            }
            case 'f': {
                // transform
                p++;
                p = parseSetTransform(p, SET_XFORM, true, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'm': {
                // resetTransform
                p++;
                p = parseSetTransform(p, IDENTITY, false, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'k': {
                // scale
                p++;
                p = parseSetTransform(p, SCALE, true, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'r': {
                // rotate
                p++;
                p = parseSetTransform(p, ROTATE, true, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'l': {
                // translate
                p++;
                p = parseSetTransform(p, TRANSLATE, true, action, &action);
                mCanvasContext->ApplyTransform(action.a, action.b, action.c, action.d,
                                               action.tx, action.ty);
                break;
            }
            case 'v': {
                // save
                p++;
                mActionStack.Append(&action, 1);
                mCanvasContext->Save();
                if (*p == ';') {
                    ++p;
                }
                break;
            }
            case 'e': {
                // restore
                p++;
                if (mActionStack.GetSize() > 0) {
                    mCanvasContext->Restore();
                    action = mActionStack[mActionStack.GetSize() - 1];
                    mActionStack.SetSize(mActionStack.GetSize() - 1);
                }
                if (*p == ';') {
                    ++p;
                }
                break;
            }
            case 'a': {
                // global alpha
                p++;
                float alpha = fastFloat(p);
                mCanvasContext->SetGlobalAlpha(alpha);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'F': {
                // fillStyle
                p++;
                char str[64] = "";
                char *tmp = str;
                while (*p && *p != ';') {
                    *tmp = *p;
                    ++tmp;
                    ++p;
                }
                if (*p == ';') ++p;
                GColorRGBA color = StrValueToColorRGBA(str);
                mCanvasContext->SetFillStyle(color);
                break;
            }
            case 'S': {
                // lineStyle
                p++;
                char str[64] = "";
                char *tmp = str;
                while (*p && *p != ';') {
                    *tmp = *p;
                    ++tmp;
                    ++p;
                }
                if (*p == ';') ++p;
                GColorRGBA color = StrValueToColorRGBA(str);
                mCanvasContext->SetStrokeStyle(color);
                break;
            }
            case 'W': {
                // lineWidth
                p++;
                float w = fastFloat(p);
                mCanvasContext->SetLineWidth(w);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'C': {
                // lineCap

                p++;
                if (strncmp(p, "butt", 4) == 0) {
                    mCanvasContext->SetLineCap(LINE_CAP_BUTT);
                } else if (strncmp(p, "round", 5) == 0) {
                    mCanvasContext->SetLineCap(LINE_CAP_ROUND);
                } else if (strncmp(p, "square", 6) == 0) {
                    mCanvasContext->SetLineCap(LINE_CAP_SQUARE);
                }

                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'J': {
                // lineJoin

                p++;
                if (strncmp(p, "miter", 4) == 0) {
                    mCanvasContext->SetLineJoin(LINE_JOIN_MITER);
                } else if (strncmp(p, "bevel", 5) == 0) {
                    mCanvasContext->SetLineJoin(LINE_JOIN_BEVEL);
                } else if (strncmp(p, "round", 5) == 0) {
                    mCanvasContext->SetLineJoin(LINE_JOIN_ROUND);
                }

                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 'M': {
                // miterLimit
                p++;
                float w = fastFloat(p);
                mCanvasContext->SetMiterLimit(w);
                while (*p && *p != ';') ++p;
                if (*p == ';') ++p;
                break;
            }
            case 's': {
                p++;
                // strokeRect
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->DoStrokeRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'c': {
                p++;
                // clearRect
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->ClearRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'p': {
                p++;
                // clip
                mCanvasContext->GCanvasContext::ClipRegion();
                if (*p == ';') ++p;
                break;
            }
            case 'q': {
                p++;
                // resetClip
                mCanvasContext->ResetClip();
                if (*p == ';') ++p;
                break;
            }
            case 'o': {
                p++;
                // closePath
                mCanvasContext->ClosePath();
                if (*p == ';') ++p;
                break;
            }
            case 'g': {
                p++;
                // moveTo
                float tokens[2] = {0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->MoveTo(tokens[0], tokens[1]);
                break;
            }
            case 'G': {
                p++;
                char id[128];
                p = extractOneParameterFromCommand(id, p);
                const int texture_id = atoi(id);
                ++p;
                p = extractOneParameterFromCommand(id, p);
                const std::string repeat_mode = id;
                ++p;

                UsePatternRenderPipeline(texture_id, repeat_mode);
                break;
            }
            case 'H': {
                // createRadialGradient()
                p++;
                char str[128];
                float start[3], end[3];
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[0] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[1] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start[2] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[0] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[1] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end[2] = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                const int stop_count = atoi(str);

                if (mCanvasContext->mCurrentState->mFillStyle != nullptr) {
                    delete mCanvasContext->mCurrentState->mFillStyle;
                }
                mCanvasContext->mCurrentState->mFillStyle =
                        new FillStyleRadialGradient(start, end);

                for (int i = 0; i < stop_count; ++i) {
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    float pos = atof(str);
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    std::string color = str;
                    (FillStyleRadialGradient *) (mCanvasContext->mCurrentState->mFillStyle)->AddColorStop(
                            pos, color);
                }
                mCanvasContext->UseRadialGradientPipeline();

                break;
            }
            case 'D': {
                // createLinearGradient()
                p++;
                char str[128];
                GPoint start, end;
                p = extractOneParameterFromCommand(str, p);
                p++;
                start.x = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                start.y = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end.x = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                end.y = atoi(str);
                p = extractOneParameterFromCommand(str, p);
                p++;
                const int stop_count = atoi(str);

                if (mCanvasContext->mCurrentState->mFillStyle != nullptr) {
                    delete mCanvasContext->mCurrentState->mFillStyle;
                }
                mCanvasContext->mCurrentState->mFillStyle =
                        new FillStyleLinearGradient(start, end);

                for (int i = 0; i < stop_count; ++i) {
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    float pos = atof(str);
                    p = extractOneParameterFromCommand(str, p);
                    p++;
                    std::string color = str;
                    (FillStyleLinearGradient *) (mCanvasContext->mCurrentState->mFillStyle)->AddColorStop(
                            pos, color);
                }
                mCanvasContext->UseLinearGradientPipeline();

                break;
            }
            case 'i': {
                p++;
                // lineTo
                float tokens[2] = {0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->LineTo(tokens[0], tokens[1]);
                break;
            }
            case 'u': {
                p++;
                // quadraticCurveTo
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->QuadraticCurveTo(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'z': {
                p++;
                // bezierCurveTo
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->BezierCurveTo(tokens[0], tokens[1], tokens[2], tokens[3],
                                              tokens[4], tokens[5]);
                break;
            }
            case 'h': {
                p++;
                // arcTo
                float tokens[6] = {0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->ArcTo(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]);
                break;
            }
            case 'b': {
                p++;
                mCanvasContext->BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'n': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->DoFillRect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'w': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->Rect(tokens[0], tokens[1], tokens[2], tokens[3]);
                break;
            }
            case 'L': {
                p++;
                mCanvasContext->Fill();
//                BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'x': {
                p++;
                mCanvasContext->Stroke();
//                BeginPath();
                if (*p == ';') ++p;
                break;
            }
            case 'y': {
                p++;
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens);
                mCanvasContext->Arc(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                                    tokens[5]);
                break;
            }
            case 'B': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);

                mCanvasContext->DoSetGlobalCompositeOperation(GCompositeOperation(tokens[0]));
                break;
            }
            case 'A': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);
                mCanvasContext->mCurrentState->mTextAlign = GTextAlign(tokens[0]);
                break;
            }
            case 'E': {
                p++;
                float tokens[1] = {0};
                p = parseTokens(p, tokens);
                mCanvasContext->mCurrentState->mTextBaseline = GTextBaseline(tokens[0]);
                break;
            }
            case 'T': { // fillText
                p++;

                int textLen = 0;
                float tokens[3] = {0, 0, 0};
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                p++;
                p = parseTokens(p, tokens);
                if (textLen > 0) {
                    strncpy(tmpText, pStart, textLen);
                    tmpText[textLen] = 0;
                    float maxWidth = tokens[2];
                    if (maxWidth < 0.0001) {
                        maxWidth = SHRT_MAX;
                    }
                    mCanvasContext->DrawText(tmpText, tokens[0], tokens[1], maxWidth);
                }

                break;
            }
            case 'U': { // strokeText
                p++;

                int textLen = 0;
                float tokens[3] = {0, 0, 0};
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                p++;
                p = parseTokens(p, tokens);
                if (textLen > 0) {
                    strncpy(tmpText, pStart, textLen);
                    tmpText[textLen] = 0;
                    float maxWidth = tokens[2];
                    if (maxWidth < 0.0001) {
                        maxWidth = SHRT_MAX;
                    }
                    mCanvasContext->StrokeText(tmpText, tokens[0], tokens[1], maxWidth);
                }

                break;
            }
            case 'j': {
                p++;
                int textLen = 0;
                const char *pStart = p;
                while (*p && (*p != ',' && *p != ';')) {
                    textLen++;
                    ++p;
                }
                if (textLen > 0) {
                    strncpy(tmpFont, pStart, textLen);
                    tmpFont[textLen] = 0;
                    if (mCanvasContext->mCurrentState->mFont != nullptr) {
                        delete mCanvasContext->mCurrentState->mFont;
                    }
                    mCanvasContext->mCurrentState->mFont = new GFontStyle(tmpFont);
                }
                if (*p == ';') ++p;

                break;
            }
            case 'P': // PutImageData
            {
                p++;
                float tokens[6] = {0, 0, 0, 0, 0, 0};
                p = parseTokens(p, tokens, 6);
                const char *begin_pos = p;

                while (*p && *p != ';') {
                    ++p;
                }
                int data_len = static_cast<int>(p - begin_pos);
                LOG_D("[executeRenderCommands] data_len:%d", data_len);

                PutImageData(begin_pos, data_len, tokens[0], tokens[1],
                             tokens[2], tokens[3], tokens[4], tokens[5]);

                if (*p == ';') ++p;
                break;
            }
            case 'R': {
                p++;
                float tokens[4] = {0, 0, 0, 0};
                p = parseTokens(p, tokens, 4);
                std::string imageDataResult;
                GetImageData(tokens[0], tokens[1], tokens[2], tokens[3], true, imageDataResult);
                setSyncResult(imageDataResult);
                break;
            }
            default: {
                LOG_W("unknown error");
                LOG_W("command: %s", p);
                p = parseUnknown(p);
            }
        }
    }
    if (LOG_LEVEL_DEBUG == GetLogLevel()) {
        mCanvasContext->Save();
        mCanvasContext->SetFillStyle(StrValueToColorRGBA("black"));
#ifdef ANDROID
        if (mCanvasContext->mCurrentState->mFont != nullptr) {
            delete mCanvasContext->mCurrentState->mFont;
        }
        mCanvasContext->mCurrentState->mFont = new GFontStyle("20px");
#endif
        char fpsString[20];
        snprintf(fpsString, sizeof(fpsString), "%.0f", mFps);
        mCanvasContext->DrawText(fpsString, 10, 25, 100);
        mCanvasContext->Restore();

    }
    mCanvasContext->SendVertexBufferToGPU();
}

void GCanvasWeex::setSyncResult(std::string result) {
    mResult = result;
}

#ifdef ANDROID

std::string GCanvasWeex::exeSyncCmd(int type, const char *&args) {

    if (mContextLost) {
        return "";
    }

    LOG_D("[GCanvasWeex::exeSyncCmd] type=%d,context type=%d,TODATAURL=%d,width=%d,height=%d,args=%s\n",
          type, GetContextType(), TODATAURL, mCanvasContext->mWidth, mCanvasContext->mHeight, args);


    if (0 == GetContextType()) {
    }


    return "";
}

int GCanvasWeex::getCmdType(int type) {
    return type >> 30;
}

int GCanvasWeex::getSyncAttrib(int type) {
    return (type >> 29) && 0x1;
}

int GCanvasWeex::getOpType(int type) {
    return type & 0x1fff;
}

void GCanvasWeex::signalUpGLthread() {
    return;
}


std::string GCanvasWeex::canvasProc(int op, int sync, std::string args) {

    LOG_E("canvasProc start.");

    if (args.c_str() == nullptr || args.length() == 0 ||
        !strcmp(args.c_str(), "")) {
        return "";
    }

    LOG_E("canvasProc,length = %d,args = %s\n", args.length(), args.c_str());
    this->Render(args.c_str(), (int) args.length());
    LOG_E("canvasProc finished.");

    return "";
}

std::string GCanvasWeex::webglProc(int op, int sync, std::string args) {
    if (args.c_str() == nullptr || args.length() == 0 ||
        !strcmp(args.c_str(), "")) {
        return nullptr;
    }
    const char *commands = args.c_str();
    executeWebGLCommands(commands, (int) args.length());
    return "";
}

std::string GCanvasWeex::vulkanProc(int op, int sync, std::string args) {
    switch (op) {

    }

    return "";
}

std::string GCanvasWeex::metalProc(int op, int sync, std::string args) {
    switch (op) {

    }

    return "";
}

const char *GCanvasWeex::CallNative(int type, const std::string &args) {
    if (mContextLost) {
        LOG_E("context lost, return");
        return "";
    }

    if (mExit) {
        LOG_E("request exit, return");
        return "";
    }

    mResult = "";

    int sync = getSyncAttrib(type);

    struct GCanvasCmd *p = new struct GCanvasCmd();
    p->type = type;
    p->args = args;

    mCmdQueue.push(p);

    signalUpGLthread();


    if (sync == SYNC) {
        LOG_D("call native sync call, start wait. cmd is %s", args.c_str());
        gcanvas::waitUtilTimeout(&mSyncSem, GCANVAS_TIMEOUT);
        LOG_D("call native sync result: %s", mResult.c_str());
        if (mResult.length() > 0) {
            char *resultChar = new char[mResult.length() + 1];
            strcpy(resultChar, mResult.c_str());
            return resultChar;
        } else {
            LOG_D("return empty string");
            return "";
        }
    }

    return nullptr;
}

void GCanvasWeex::clearCmdQueue() {

    GCanvasManager *theManager = GCanvasManager::GetManager();
    theManager->clearQueueByContextId(mContextId);
    while (!mCmdQueue.empty()) {
        struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *> (mCmdQueue.front());
        mCmdQueue.pop();
        delete p;
    }

    while (!mBitmapQueue.empty()) {
        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd *> (mBitmapQueue.front());
        mBitmapQueue.pop();
        delete p;
    }

}

void GCanvasWeex::QueueProc(std::queue<struct GCanvasCmd *> *queue) {
    if (queue == nullptr) {
        return;
    }
    if (!queue->empty()) {
        struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *>(queue->front());
        int type = p->type;
        int cmd = getCmdType(type);
        int op = getOpType(type);
        int sync = getSyncAttrib(type);

        std::string args = p->args;

        LOG_E("start to process queue cmd.");

        switch (cmd) {
            case CANVAS: {
                canvasProc(op, sync, args);
                break;
            }
            case WEBGL: {
                webglProc(op, sync, args);
                break;
            }
            case VULKAN: {
                vulkanProc(op, sync, args);
                break;
            }
            case METAL: {
                metalProc(op, sync, args);
                break;
            }
            default: {
                break;
            }
        }

        if (op == 1) {
            setRefreshFlag(true);
        }
        queue->pop();

        delete p;

        if (sync == SYNC) {
            setSyncFlag();
        }
    }
}

void GCanvasWeex::LinkNativeGLProc() {
    if (mContextLost) {
        LOG_E("in LinkNativeGLProc mContextLost");
        clearCmdQueue();
        return;
    }

    GCanvasManager *theManager = GCanvasManager::GetManager();
    std::queue<struct GCanvasCmd *> *queue = theManager->getQueueByContextId(
            mContextId);
    if (queue != nullptr) {
        LOG_E("in LinkNativeProc QueueProc queue");
        QueueProc(queue);
    }
    //    LOG_D("in LinkNativeProc QueueProc mCmdQueue");
    QueueProc(&mCmdQueue);

    if (queue != nullptr) {
        delete queue;
    }
}

void GCanvasWeex::setSyncFlag() {
    mSync = true;
}

void GCanvasWeex::setThreadExit() {
    mExit = true;
}

void GCanvasWeex::setContextLost(bool lost) {
    mContextLost = lost;
}

void GCanvasWeex::finishProc() {
    if (mSync == SYNC) {
        LOG_D("LinkNativeGLProc,sync proc result=%s\n", mResult.c_str());
        sem_post(&mSyncSem);
        mSync = false;
    }
}

void GCanvasWeex::setRefreshFlag(bool refresh) {

}


void GCanvasWeex::addBitmapQueue(struct BitmapCmd *p) {
    mBitmapQueue.push(p);
}

void GCanvasWeex::bindTexture(struct BitmapCmd cmd) {
    //    while (!mBitmapQueue.empty()) {
    //        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd * >(mBitmapQueue.front());
    GLuint glID;
    LOG_D("DO BIND TEXTURE. context type = %d", GetContextType());

    //step 1:bindtexture
    if (GetContextType() == 0) {
        glGenTextures(1, &glID);
        glBindTexture(GL_TEXTURE_2D, glID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    }

    glTexImage2D(cmd.target, cmd.level, cmd.interformat, cmd.width,
                 cmd.height, 0, cmd.format,
                 cmd.type, cmd.Bitmap);

    //step 2:save textureid
    if (GetContextType() == 0) {
        glGenerateMipmap(GL_TEXTURE_2D);
        AddTexture(cmd.id, glID, cmd.width, cmd.height);
    }
}

void GCanvasWeex::bindTexture(GTexture *texture) {


    if (GetContextType() == 0) {
        texture->Bind();
        //        AddTexture(texture->GetTextureID(), texture->GetTextureID(), texture->GetWidth(), texture->GetHeight());

    }


}

void GCanvasWeex::texSubImage2D(struct BitmapCmd cmd) {
    LOG_D("start to texSubImage2D in grenderer.");
    glTexSubImage2D(cmd.target, cmd.level, cmd.xoffset, cmd.yoffset,
                    cmd.width, cmd.height,
                    cmd.format,
                    cmd.type, cmd.Bitmap);
}


bool GCanvasWeex::continueProcess() {

    if (mContextLost) {
        return false;
    }

    return !mBitmapQueue.empty() || !mCmdQueue.empty();

}

Callback::Callback(const char *id, const char *res,
                   bool error) {
    strncpy(callbackId, id, ALLOCATED - 1);
    callbackId[ALLOCATED - 1] = 0;

    strncpy(result, res, ALLOCATED - 1);
    result[ALLOCATED - 1] = 0;
    isError = error;
}


// Get the front of the callback queue
Callback *GCanvasWeex::GetNextCallback() {
    if (!mCallbacks.IsEmpty())
        return mCallbacks[0];
    else
        return nullptr;
}

// delete the front of the callback queue
void GCanvasWeex::PopCallbacks() {
    if (!mCallbacks.IsEmpty()) mCallbacks.RemoveAt(0);
}


// push to the end of the callback queue
void GCanvasWeex::AddCallback(const char *callbackID, const char *result,
                              bool isError) {
    if (callbackID != nullptr && *callbackID != '\0') {
        Callback *cb = new Callback(callbackID, result, isError);
        mCallbacks.Append(&cb, 1);
        LOG_D("Canvas::AddCallback - Callback created: %s, %s, %d",
              callbackID, result, isError);
    }
}

#endif

} //namespace gcanvas

#endif
