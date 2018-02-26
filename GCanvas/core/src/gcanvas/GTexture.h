/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCanvas_GTexture_h
#define GCanvas_GTexture_h

#ifndef _WIN32

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#else
#include <GLES2/gl2.h>
#endif

#include <map>
#include <vector>

// -----------------------------------------------------------
// --    Texture utility class
// --    Used by loadTexture
// -----------------------------------------------------------
class Texture
{
public:
    Texture(int glID, int w, int h)
    {
        mGlId = glID;
        mWidth = w;
        mHeight = h;
    }

    int GetGlID() const { return mGlId; }
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }

    void Bind() const { glBindTexture(GL_TEXTURE_2D, (GLuint)mGlId); }

private:
    int mGlId;
    int mWidth;
    int mHeight;
};

class TextureGroup
{
public:
    TextureGroup(){};
    ~TextureGroup();

    bool IsSplit() const;
    void Clear();
    void Append(int glID, int width, int height);
    const Texture *Get(int id) const;

public:
    std::vector< Texture * > mVecTexture;
    int mSrcWidth;   // enable when split
    int mSrcHeight;   // enable when split
    int mTileWidth;  // enable when split
    int mTileHeight; // enable when split
};

class TextureMgr
{
public:
    TextureMgr();
    static GLuint CreateTexture(const unsigned char *rgbaData,
                                unsigned int width, unsigned int height);
    bool AppendPng(const unsigned char *buffer, unsigned int size, int textureGroupId,
                   unsigned int *widthPtr, unsigned int *heightPtr);
    void Append(int id, int glID, int width, int height);
    void Remove(int id);
    void Clear();
    const TextureGroup *Get(int id) const;

private:
    std::map< int, TextureGroup > mTextureGroupPool;
};

class GTexture
{
public:
    GTexture(unsigned int w, unsigned int h, GLenum format = GL_RGBA,
              GLubyte *pixels = nullptr);
    GTexture(const char *filePath);
    GTexture();
    ~GTexture();

    void Bind() const;
    void Unbind() const;

    unsigned int GetWidth() const { return mWidth; }
    unsigned int GetHeight() const { return mHeight; }
    GLenum GetFormat() const { return mFormat; }
    void SetWidth(const unsigned int w) { mWidth = w; }
    void SetHeight(const unsigned int h) { mHeight = h; }
    void SetFormat(const GLenum format) { mFormat = format; }
    GLuint GetTextureID() const { return mTextureID; }
    void SetTextureID(const GLuint id) { mTextureID = id; }
    void UpdateTexture(GLubyte *pixels, int x, int y, int w, int h);

    // Caution: the memeory allocated by callback should be allocated by new []
    static void SetLoadPixelCallback(GLubyte *(*callback)(const char *filePath,
                                                          unsigned int *w,
                                                          unsigned int *h));

    unsigned int size();

private:
    void createTexture(GLubyte *pixels);
    static GLubyte *(*loadPixelCallback)(const char *filePath, unsigned int *w,
                                         unsigned int *h);
    static GLubyte *loadPixelsFromPNG(const char *filePath, unsigned int *w,
                                      unsigned int *h);

private:
    unsigned int mWidth;
    unsigned int mHeight;
    GLenum mFormat;
    GLuint mTextureID;
};

#endif
