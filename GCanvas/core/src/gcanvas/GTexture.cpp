/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GTexture.h"
#include "../support/Log.h"
#include "../support/Util.h"


GLubyte *(*GTexture::loadPixelCallback)(const char *filePath, unsigned int *w,
                                         unsigned int *h) = nullptr;

GTexture::GTexture(unsigned int w, unsigned int h, GLenum format,
                     GLubyte *pixels, std::vector<GCanvasLog> *errVec)
    : mWidth(w), mHeight(h), mFormat(format), mTextureID(0)
{
    CreateTexture(pixels, errVec);
}

GTexture::GTexture() : mWidth(0), mHeight(0), mFormat(0), mTextureID(0) {}

GTexture::GTexture(const char *path)
    : mWidth(0), mHeight(0), mFormat(GL_RGBA), mTextureID(0)
{
    GLubyte *pixels = nullptr;
    if (loadPixelCallback == nullptr)
    {
        pixels = loadPixelsFromPNG(path, &mWidth, &mHeight);
        CreateTexture(pixels);
        free(pixels);
    }
    else
    {
        pixels = loadPixelCallback(path, &mWidth, &mHeight);
        CreateTexture(pixels);
        delete[] pixels;
    }

    pixels = 0;
}

GTexture::~GTexture()
{
    glDeleteTextures(1, &mTextureID);
    mFormat = mTextureID = mWidth = mHeight = 0;
}

void GTexture::Bind() const { glBindTexture(GL_TEXTURE_2D, mTextureID); }

void GTexture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void GTexture::SetLoadPixelCallback(GLubyte *(*callback)(const char *filePath,
                                                          unsigned int *w,
                                                          unsigned int *h))
{
    loadPixelCallback = callback;
}

GLubyte *GTexture::loadPixelsFromPNG(const char *path, unsigned int *pw,
                                      unsigned int *ph)
{
    unsigned char *buffer = nullptr;
//    int res = PngLoader::Instance().DecodePng(path, &buffer, pw, ph);
//    if (res != 0) return nullptr;
    return buffer;
}

void GTexture::CreateTexture(GLubyte *pixels, std::vector<GCanvasLog> *errVec)
{
    GLenum glerror = 0;
    while ((glerror = glGetError()) != GL_NO_ERROR && errVec)
    {
        GCanvasLog log;
        fillLogInfo(log, "glerror_before", "<function:%s, glGetError:%x>", __FUNCTION__, glerror);
        errVec->push_back(log);
    }
    
    // Release previous texture if we had one
    if (mTextureID)
    {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    if ((int)mWidth > maxTextureSize || (int)mHeight > maxTextureSize)
    {
        if (errVec) {
            GCanvasLog log;
            fillLogInfo(log, "texture_size_exceed", "<function:%s, width:%d, height:%d, maxSize:%d>", __FUNCTION__, mWidth, mHeight, maxTextureSize);
            errVec->push_back(log);
        }
        return;
    }
    
    int boundTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);

    glGenTextures(1, &mTextureID);
    if (mTextureID <= 0 && errVec) {
        GCanvasLog log;
        fillLogInfo(log, "gen_texture_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glGetError());
        errVec->push_back(log);
    }
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glerror = glGetError();
    if (glerror && errVec && mWidth > 0 && mHeight > 0) {
        GCanvasLog log;
        fillLogInfo(log, "bind_texture_fail", "<function:%s, glGetError:%x, width:%d, height:%d>", __FUNCTION__, glerror, mWidth, mHeight);
        errVec->push_back(log);
    }


    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, (GLsizei)mWidth,
                 (GLsizei)mHeight, 0, mFormat, GL_UNSIGNED_BYTE, pixels);

    glerror = glGetError();
    if (glerror && errVec) {
        GCanvasLog log;
        fillLogInfo(log, "glTexImage2D_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glerror);
        errVec->push_back(log);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLint filter = GL_LINEAR;
#ifdef ANDROID
    if (mFormat == GL_ALPHA) {
        filter = GL_NEAREST;
    }
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    glBindTexture(GL_TEXTURE_2D, (GLuint)boundTexture);
    glerror = glGetError();
    if (glerror && errVec) {
        GCanvasLog log;
        fillLogInfo(log, "glBindTexture_fail", "<function:%s, glGetError:%x>", __FUNCTION__, glerror);
        errVec->push_back(log);
    }
    glFlush();
}

void GTexture::UpdateTexture(GLubyte *pixels, int x, int y, int w, int h)
{
    if (mTextureID == 0)
    {
        return;
    }

    int boundTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
    if (boundTexture != (int)mTextureID)
    {
        glBindTexture(GL_TEXTURE_2D, mTextureID);
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, mFormat, GL_UNSIGNED_BYTE,
                    pixels);
    if (boundTexture != (int)mTextureID)
    {
        glBindTexture(GL_TEXTURE_2D, (GLuint)boundTexture);
    }
    glFlush();
}

unsigned int GTexture::size()
{
    unsigned int factor;
    switch (mFormat)
    {
        case GL_RGB:
            factor = 3;
            break;
        case GL_RGBA:
            factor = 4;
            break;
        case GL_LUMINANCE:
            factor = 3;
            break;
        case GL_LUMINANCE_ALPHA:
            factor = 4;
            break;
        default:
            factor = 1;
    }

    return sizeof(GTexture) + mWidth * mHeight * factor;
}

TextureGroup::~TextureGroup() { Clear(); };

bool TextureGroup::IsSplit() const { return (1 != mVecTexture.size()); }
void TextureGroup::Clear()
{
    for (unsigned int i = 0; i < mVecTexture.size(); ++i)
    {
        GLuint glID = (GLuint)mVecTexture[i]->GetGlID();
        glDeleteTextures(1, &glID);
        delete mVecTexture[i];
    }
    mVecTexture.clear();
}
void TextureGroup::Append(int glID, int width, int height)
{
    Texture *texture = new Texture(glID, width, height);
    mVecTexture.push_back(texture);
}

const Texture *TextureGroup::Get(int id) const
{
    if (0 <= id && id < (int)mVecTexture.size())
    {
        return mVecTexture[id];
    }
    return nullptr;
}

TextureMgr::TextureMgr(){};

void TextureMgr::Append(int id, int glID, int width, int height)
{
    LOG_D("TextureMgr::Append");
    TextureGroup &textureGroup = mTextureGroupPool[id];
    textureGroup.Clear();
    textureGroup.Append(glID, width, height);
}
void TextureMgr::Remove(int id) { mTextureGroupPool.erase(id); }
void TextureMgr::Clear() { mTextureGroupPool.clear(); }

const TextureGroup *TextureMgr::Get(int id) const
{
    std::map< int, TextureGroup >::const_iterator itr;
    itr = mTextureGroupPool.find(id);
    if (mTextureGroupPool.end() != itr)
    {
        return &(itr->second);
    }
    return nullptr;
}

bool TextureMgr::AppendPng(const unsigned char *buffer, unsigned int size, int textureGroupId,
                           unsigned int *widthPtr, unsigned int *heightPtr)
{
    return false;
//    unsigned int srcWidth = 0;
//    unsigned int srcHeight = 0;
//    unsigned int destWidth = 0;
//    unsigned int destHeight = 0;
//
//    bool success = false;
//    unsigned char *srcPixels = nullptr;
//    int error = PngLoader::Instance().DecodePng(
//        buffer, size, &srcPixels, &srcWidth, &srcHeight);
//    if (error)
//    {
//        LOG_E("[TextureMgr::AppendPng] error %d", error);
//    }
//    else
//    {
//        TextureGroup &textureGroup = mTextureGroupPool[textureGroupId];
//        textureGroup.Clear();
//
//        GLint glMax;
//        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMax);
//        unsigned int maxTextureSize = (unsigned int)glMax;
//
//        *widthPtr = srcWidth;
//        *heightPtr = srcHeight;
//        destWidth = std::min(srcWidth, maxTextureSize);
//        destHeight = std::min(srcHeight, maxTextureSize);
//
//        textureGroup.mSrcWidth = srcWidth;
//        textureGroup.mSrcHeight = srcHeight;
//        textureGroup.mTileWidth = destWidth;
//        textureGroup.mTileHeight = destHeight;
//
//        if (srcWidth <= maxTextureSize)
//        {
//            for (unsigned int y = 0; y < srcHeight; y += destHeight)
//            {
//                int h = std::min(srcHeight - y, destHeight);
//
//                GLuint glID = CreateTexture(srcPixels + (y * 4), srcWidth, h);
//                Texture *texture = new Texture(glID, srcWidth, h);
//                textureGroup.mVecTexture.push_back(texture);
//            }
//        }
//        else
//        {
//            GLubyte *destPixels = new GLubyte[4 * destWidth * destHeight];
//
//            for (unsigned int y = 0; y < srcHeight; y += destHeight)
//            {
//                int h = std::min(srcHeight - y, destHeight);
//                for (unsigned int x = 0; x < srcWidth; x += destWidth)
//                {
//                    int w = std::min(srcWidth - x, destWidth);
//
//                    gcanvas::GetSegmentPixel(srcPixels, srcWidth, x, y, w, h,
//                                             destPixels);
//                    GLuint glID = CreateTexture(destPixels, w, h);
//                    Texture *texture = new Texture(glID, w, h);
//                    textureGroup.mVecTexture.push_back(texture);
//                }
//            }
//            delete[] destPixels;
//        }
//
//        success = true;
//    }
//
//    if (srcPixels)
//    {
//        free(srcPixels);
//    }
//
//    return success;
}
