/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GGLProgram__
#define __GCanvas__GGLProgram__

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

#include "GPoint.h"

class GShader
{
public:
    GShader(const char *name, const char *vertexShaderSrc,
               const char *fragmentShaderSrc);
    virtual ~GShader();

#ifdef ANDROID
    bool initWithPreCompiledProgramByteArray(const char *shaderName,
                                             const GLchar *vShaderByteArray,
                                             const GLchar *fShaderByteArray);
#endif

    void Bind();

    std::string &GetName() { return mName; }

    virtual void RestoreShaderState() {}

    virtual void SetTextSampler(int value) {}

    virtual void SetHasTexture(int value) {}

    virtual void SetOverideTextureColor(int value) {}

    virtual GLint GetTexcoordSlot() { return -1; }

    virtual GLint GetPositionSlot() { return -1; }

    virtual GLint GetColorSlot() { return -1; }

    virtual GLint GetTransformSlot() { return -1; }

    virtual void SetRepeatMode(const std::string &pattern) {}

    virtual void SetRange(const GPoint &start, const GPoint &end) {}

    virtual void SetRange(const float *start, const float *end) {}

    virtual void SetColorStopCount(int count) {}

    virtual void SetColorStop(float *pack_color_stop, int id) {}

    virtual void SortColorStop() {}

    virtual void SetTextureSize(float width, float height) {}

protected:
    virtual void calculateAttributesLocations();
    GLuint compileShader(const char *shader, GLenum shaderType);

    GLuint mHandle;
    std::string mName;
};

class DefaultShader : public GShader
{
public:
    DefaultShader(const char *name, const char *vertexShaderSrc,
                  const char *fragmentShaderSrc);
    ~DefaultShader();

    void SetTextSampler(int value)
    {
        glUniform1i(mTextureSamplerSlot, value);
    }

    void SetHasTexture(int value)
    {
        mHasTextureFlag = (value != 0);
        glUniform1i(mHasTextureSlot, value);
    }

    void SetOverideTextureColor(int value)
    {
        mOverrideTextureColorFlag = value;
        glUniform1i(mOverrideTextureColorSlot, value);
    }

    GLint GetTexcoordSlot() { return mTexcoordSlot; }

    GLint GetPositionSlot() { return mPositionSlot; }

    GLint GetColorSlot() { return mColorSlot; }

    GLint GetTransformSlot() { return mTransfromSlot; }

    void RestoreShaderState()
    {
        SetHasTexture(mHasTextureFlag);
        SetOverideTextureColor(mOverrideTextureColorFlag);
    }

protected:
    void calculateAttributesLocations();

private:
    GLuint mTexcoordSlot;
    GLuint mPositionSlot;
    GLuint mColorSlot;
    GLuint mTransfromSlot;
    GLuint mTextureSamplerSlot;
    GLuint mHasTextureSlot;
    GLuint mOverrideTextureColorSlot;

    bool mHasTextureFlag;
    bool mOverrideTextureColorFlag;
};

class PatternShader : public GShader
{
public:
    PatternShader(const char *name, const char *vertexShaderSrc,
                  const char *fragmentShaderSrc);

    void SetTextSampler(int value)
    {
        glUniform1i(mTextureSamplerSlot, value);
    }

    void SetRepeatMode(const std::string &pattern)
    {
        glUniform1i(mRepeatXSlot,
                    (pattern == "repeat" || pattern == "repeat-x"));
        glUniform1i(mRepeatYslot,
                    (pattern == "repeat" || pattern == "repeat-y"));
    }

    void SetTextureSize(float width, float height)
    {
        glUniform2f(mTextureSizeSlot, width, height);
    }

    GLint GetTexcoordSlot() { return mTexcoordSlot; }

    GLint GetPositionSlot() { return mPositionSlot; }

    GLint GetColorSlot() { return mColorSlot; }

    GLint GetTransformSlot() { return mTransfromSlot; }

    GLint GetTextureSamplerSlot() { return mTextureSamplerSlot; }

protected:
    void calculateAttributesLocations();

    GLuint mTexcoordSlot;
    GLuint mPositionSlot;
    GLuint mColorSlot;
    GLuint mTransfromSlot;
    GLuint mTextureSamplerSlot;
    GLuint mRepeatXSlot;
    GLuint mRepeatYslot;
    GLuint mTextureSizeSlot;
};

class LinearGradientShader : public GShader
{
public:
    LinearGradientShader(const char *name, const char *vertexShaderSrc,
                         const char *fragmentShaderSrc);

    GLint GetTexcoordSlot() { return mTexcoordSlot; }

    GLint GetPositionSlot() { return mPositionSlot; }

    GLint GetColorSlot() { return mColorSlot; }

    GLint GetTransformSlot() { return mTransfromSlot; }

    void SetHasTexture(int value)
    {
        mHasTextureFlag = (value != 0);
        glUniform1i(mHasTextureSlot, value);
    }

    void RestoreShaderState() { SetHasTexture(mHasTextureFlag); }

    void SetRange(const GPoint &start, const GPoint &end)
    {
        glUniform2f(mRangeStartSlot, start.x, start.y);
        glUniform2f(mRangeEndSlot, end.x, end.y);
    }

    void SetColorStopCount(int count) { glUniform1i(mStopCountSlot, count); }

    void SetColorStop(float *stop, int id)
    {
        // stop[0-2] must be the RGB of the stop
        // stop[3] must be the position of the stop
        GLuint loc;
        if (id == 0)
        {
            loc = mStop0Slot;
        }
        else if (id == 1)
        {
            loc = mStop1slot;
        }
        else if (id == 2)
        {
            loc = mStop2slot;
        }
        else if (id == 3)
        {
            loc = mStop3Slot;
        }
        else if (id == 4)
        {
            loc = mStop4Slot;
        }
        else
        {
            return;
        }

        glUniform4f(loc, stop[0], stop[1], stop[2], stop[3]);
    }

protected:
    void calculateAttributesLocations();

    GLuint mPositionSlot;
    GLuint mTexcoordSlot;
    GLuint mColorSlot;
    GLuint mTransfromSlot;
    GLuint mTextureSamplerSlot;

    GLuint mRangeStartSlot;
    GLuint mRangeEndSlot;
    GLuint mStopCountSlot;
    GLuint mStop0Slot;
    GLuint mStop1slot;
    GLuint mStop2slot;
    GLuint mStop3Slot;
    GLuint mStop4Slot;

    GLuint mHasTextureSlot;
    bool mHasTextureFlag;
};

class RadialGradientShader : public GShader
{
public:
    RadialGradientShader(const char *name, const char *vertexShaderSrc,
                         const char *fragmentShaderSrc);

    GLint GetTexcoordSlot() { return mTexcoordSlot; }

    GLint GetPositionSlot() { return mPositionSlot; }

    GLint GetColorSlot() { return mColorSlot; }

    GLint GetTransformSlot() { return mTransfromSlot; }

    void SetHasTexture(int value)
    {
        mHasTextureFlag = (value != 0);
        glUniform1i(mHasTextureSlot, value);
    }

    void RestoreShaderState() { SetHasTexture(mHasTextureFlag); }

    void SetRange(const float *start, const float *end)
    {
        glUniform3f(mStartSlot, start[0], start[1], start[2]);
        glUniform3f(mEndSlot, end[0], end[1], end[2]);
    }

    void SetColorStopCount(int count) { glUniform1i(mStopCountSlot, count); }

    void SetColorStop(float *stop, int id)
    {
        // stop[0-2] must be the RGB of the stop
        // stop[3] must be the position of the stop
        GLuint loc;
        if (id == 0)
        {
            loc = mStop0Slot;
        }
        else if (id == 1)
        {
            loc = mStop1Slot;
        }
        else if (id == 2)
        {
            loc = mStop2Slot;
        }
        else if (id == 3)
        {
            loc = mStop3Slot;
        }
        else if (id == 4)
        {
            loc = mStop4Slot;
        }
        else
        {
            return;
        }

        glUniform4f(loc, stop[0], stop[1], stop[2], stop[3]);
    }

protected:
    void calculateAttributesLocations();

    GLuint mPositionSlot;
    GLuint mTexcoordSlot;
    GLuint mColorSlot;
    GLuint mTransfromSlot;
    GLuint mTextureSamplerSlot;
    GLuint mStartSlot;
    GLuint mEndSlot;
    GLuint mStopCountSlot;
    GLuint mStop0Slot;
    GLuint mStop1Slot;
    GLuint mStop2Slot;
    GLuint mStop3Slot;
    GLuint mStop4Slot;

    GLuint mHasTextureSlot;
    bool mHasTextureFlag;
};

#endif
