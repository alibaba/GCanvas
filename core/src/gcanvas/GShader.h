/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GSHADER_H
#define GCANVAS_GSHADER_H

#include "GGL.h"
#include "GPoint.h"
#include "GTransform.h"
#include "../support/Log.h"

#include <vector>

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

    const std::string &GetName() { return mName; }

    virtual void RestoreShaderState() {}

    virtual void SetTextSampler(int value) {}

    virtual void SetHasTexture(bool value) {}

    virtual void SetOverideTextureColor(int value) {}
    
    GLint GetTexcoordSlot() { return mTexcoordSlot; }
    
    GLint GetPositionSlot() { return mPositionSlot; }
    
    GLint GetColorSlot() { return mColorSlot; }
    
    GLint GetTransformSlot() { return mTransfromSlot; }

    virtual void SetRepeatMode(const std::string &pattern) {}

    virtual void SetRange(const GPoint &start, const GPoint &end) {}

    virtual void SetRange(const float *start, const float *end) {}

    virtual void SetColorStopCount(int count) {}

    virtual void SetColorStop(float *pack_color, float stop, int id) {}

    virtual void SortColorStop() {}

    virtual void SetTextureSize(float width, float height) {}

    void SetTransform(const GTransform &trans);

    virtual void SetDelta(float x, float y) {}

    virtual void SetWeight(float w[], int count) {}

    std::vector<GCanvasLog>& GetErrorVector(){ return mErrVec;}
    
    void ClearErrorVector() { mErrVec.clear(); }

protected:
    virtual void calculateAttributesLocations();

    GLuint compileShader(const char *shader, GLenum shaderType);

    GLuint mHandle;
    std::string mName;
    
    GLint mTexcoordSlot;
    GLint mPositionSlot;
    GLint mColorSlot;
    GLint mTransfromSlot;
    
    std::vector<GCanvasLog> mErrVec;
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

    void SetHasTexture(bool value)
    {

        if (mHasTextureFlag != value)
        {
            mHasTextureFlag = value;
            glUniform1i(mHasTextureSlot, value);

        }
    }

    void SetOverideTextureColor(int value)
    {
        mOverrideTextureColorFlag = value;
        glUniform1i(mOverrideTextureColorSlot, value);
    }

    void RestoreShaderState()
    {
        SetHasTexture(mHasTextureFlag);
        SetOverideTextureColor(mOverrideTextureColorFlag);
    }

protected:
    void calculateAttributesLocations();

private:
    GLuint mTextureSamplerSlot;
    GLuint mHasTextureSlot;
    GLuint mOverrideTextureColorSlot;

    bool mHasTextureFlag;
    bool mOverrideTextureColorFlag;
};

class TextureShader : public GShader
{
public:
    TextureShader(const char *name, const char *vertexShaderSrc,
                  const char *fragmentShaderSrc);

    ~TextureShader() = default;

    void SetTextSampler(int value)
    {
        glUniform1i(mTextureSamplerSlot, value);
    }
    
    void SetPremultipliedAlpha(bool value)
    {
        glUniform1i(mPremultipliedAlphaSlot, value);
    }

protected:
    void calculateAttributesLocations();

private:
    GLuint mTextureSamplerSlot;
    GLuint mPremultipliedAlphaSlot;
};

class ShadowShader : public GShader
{
public:
    ShadowShader(const char *name, const char *vertexShaderSrc,
                 const char *fragmentShaderSrc);

    ~ShadowShader() = default;


    void SetDelta(float x, float y)
    {
        glUniform1f(mXDeltaSlot, x);
        glUniform1f(mYDeltaSlot, y);

    }

    void SetWeight(float w[], int count)
    {
        glUniform1fv(mWeightSlot, count, w);

    }

protected:
    void calculateAttributesLocations();

private:
    GLuint mXDeltaSlot;
    GLuint mYDeltaSlot;
    GLuint mWeightSlot;
    GLuint mSamplerSlot;

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
    
    void SetPatternAlpha(float patternAlpha)
    {
        glUniform1f(mPatternAlphaSlot, patternAlpha);
    }

    GLint GetTextureSamplerSlot() { return mTextureSamplerSlot; }

protected:
    void calculateAttributesLocations();
    
    GLuint mTextureSamplerSlot;
    GLuint mRepeatXSlot;
    GLuint mRepeatYslot;
    GLuint mTextureSizeSlot;
    GLuint mPatternAlphaSlot;
};


class GradientShader : public GShader
{
public:
    GradientShader(const char *name, const char *vertexShaderSrc, const char *fragmentShaderSrc);
    
    void SetHasTexture(bool value)
    {
        if (mHasTextureFlag != value)
        {
            mHasTextureFlag = value;
            glUniform1i(mHasTextureSlot, value);
            
        }
    }
    
    void RestoreShaderState() { SetHasTexture(mHasTextureFlag); }
    
    void SetColorStopCount(int count) { glUniform1i(mStopCountSlot, count); }
    
    void SetColorStop(float *color, float stop, int id)
    {
        GLuint stopLoc;
        GLuint colorLoc;
        if (id == 0)
        {
            stopLoc = mStop0Slot;
            colorLoc = mStop0ColorSlot;
        }
        else if (id == 1)
        {
            stopLoc = mStop1Slot;
            colorLoc = mStop1ColorSlot;
        }
        else if (id == 2)
        {
            stopLoc = mStop2Slot;
            colorLoc = mStop2ColorSlot;
        }
        else if (id == 3)
        {
            stopLoc = mStop3Slot;
            colorLoc = mStop3ColorSlot;
        }
        else if (id == 4)
        {
            stopLoc = mStop4Slot;
            colorLoc = mStop4ColorSlot;
        }
        else
        {
            return;
        }
        
        glUniform1f(stopLoc, stop);
        glUniform4f(colorLoc, color[0], color[1], color[2], color[3]);
    }
    
protected:
    void calculateAttributesLocations();
    
    GLuint mTextureSamplerSlot;
    
    GLuint mStopCountSlot;
    
    GLuint mStop0ColorSlot;
    GLuint mStop1ColorSlot;
    GLuint mStop2ColorSlot;
    GLuint mStop3ColorSlot;
    GLuint mStop4ColorSlot;
    
    GLuint mStop0Slot;
    GLuint mStop1Slot;
    GLuint mStop2Slot;
    GLuint mStop3Slot;
    GLuint mStop4Slot;
    
    GLuint mHasTextureSlot;
    bool mHasTextureFlag;
};

class LinearGradientShader : public GradientShader
{
public:
    LinearGradientShader(const char *name, const char *vertexShaderSrc,
                         const char *fragmentShaderSrc);

    void SetRange(const GPoint &start, const GPoint &end)
    {
        glUniform2f(mRangeStartSlot, start.x, start.y);
        glUniform2f(mRangeEndSlot, end.x, end.y);
    }

protected:
    void calculateAttributesLocations();
    GLuint mRangeStartSlot;
    GLuint mRangeEndSlot;
};

class RadialGradientShader : public GradientShader
{
public:
    RadialGradientShader(const char *name, const char *vertexShaderSrc,
                         const char *fragmentShaderSrc);

    void SetRange(const float *start, const float *end)
    {
        glUniform3f(mStartSlot, start[0], start[1], start[2]);
        glUniform3f(mEndSlot, end[0], end[1], end[2]);
    }
protected:
    void calculateAttributesLocations();
    GLuint mStartSlot;
    GLuint mEndSlot;

};

#endif /* GCANVAS_GSHADER_H */
