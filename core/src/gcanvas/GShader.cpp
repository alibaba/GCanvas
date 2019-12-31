/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GShader.h"

#ifdef ANDROID

#include "GPreCompiledShaders.h"

#define CHECK_GL_ERROR_DEBUG()                                                 \
    do                                                                         \
    {                                                                          \
        GLenum __error = glGetError();                                         \
        if (__error)                                                           \
        {                                                                      \
            LOG_E("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__,      \
                  __FUNCTION__, __LINE__);                                     \
        }                                                                      \
    } while (false)


bool GShader::initWithPreCompiledProgramByteArray(
        const char *shaderName, const GLchar *vShaderByteArray,
        const GLchar *fShaderByteArray)
{
    mHandle = glCreateProgram();
    bool haveProgram = GPreCompiledShaders::getInstance()->LoadProgram(mHandle, shaderName);

    CHECK_GL_ERROR_DEBUG();

    return haveProgram;
}

extern bool g_use_pre_compile;

#endif


#include <string.h>

GShader::GShader(const char *name, const char *vertexShaderSrc,
                 const char *fragmentShaderSrc)
        : mHandle(0), mName(name)
{
#ifdef ANDROID
    std::string shaderName = SHADER_NAME_PREFIX + mName;

    if (g_use_pre_compile)
    {
        if (GPreCompiledShaders::getInstance()
                    ->GetSupportPreCompiledShaders() &&
            initWithPreCompiledProgramByteArray(
                    shaderName.c_str(), vertexShaderSrc, fragmentShaderSrc))
        {
            return;
        }
    }
#endif
    
    GLuint vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
    if (vertexShader == 0)
    {
        GCanvasLog log;
        fillLogInfo(log, "shader_compile_fail", "type: vertex, name:%s, glGetError:%x", name, glGetError());
        mErrVec.push_back(log);
        return;
    }

    GLuint fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0)
    {
        GCanvasLog log;
        fillLogInfo(log, "shader_compile_fail", "type: fragment, name:%s, glGetError:%x", name, glGetError());
        mErrVec.push_back(log);
        return;
    }

    mHandle = glCreateProgram();
    glAttachShader(mHandle, vertexShader);
    glAttachShader(mHandle, fragmentShader);
    glLinkProgram(mHandle);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linkSuccess;
    glGetProgramiv(mHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(mHandle, sizeof(message), 0, &message[0]);
        
        GCanvasLog log;
        fillLogInfo(log, "program_link_fail", "name:%s, error:%x", name, message);
        mErrVec.push_back(log);
        
        glDeleteProgram(mHandle);
        mHandle = 0;
        return;
    }
#ifdef ANDROID
    else
    {
        if (g_use_pre_compile &&
            GPreCompiledShaders::getInstance()
                    ->GetSupportPreCompiledShaders())
        {
            GPreCompiledShaders::getInstance()->AddProgram(mHandle, mName);
        }
    }
#endif
}

GShader::~GShader()
{
    if (mHandle != 0)
    {
        glDeleteProgram(mHandle);
        mHandle = 0;
    }
}

GLuint GShader::compileShader(const char *shader, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    if (shaderHandle == 0)
    {
        return 0;
    }

    GLint shaderLength = (GLint) strlen(shader);
    glShaderSource(shaderHandle, 1, &shader, &shaderLength);
    glCompileShader(shaderHandle);

    GLint compileResult;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE)
    {
        GLchar message[2048];
        int len = 0;
        glGetShaderInfoLog(shaderHandle, sizeof(message), &len, &message[0]);
        
        GCanvasLog log;
        fillLogInfo(log, "shader_compile_fail", "error:%s", message);
        mErrVec.push_back(log);
        
        glDeleteShader(shaderHandle);
        return 0;
    }
    return shaderHandle;
}

void GShader::Bind()
{
    glUseProgram(mHandle);
    RestoreShaderState();
}

void GShader::SetTransform(const GTransform &trans)
{

    GLfloat m[16] = {0.0f};
    m[0] = trans.a;
    m[1] = trans.c;
    m[4] = trans.b;
    m[5] = trans.d;
    m[12] = trans.tx;
    m[13] = trans.ty;
    m[15] = 1;


    glUniformMatrix4fv(GetTransformSlot(), 1, GL_FALSE, &(m[0]));

}

void GShader::calculateAttributesLocations() {}

DefaultShader::DefaultShader(const char *name, const char *vertexShaderSrc,
                             const char *fragmentShaderSrc)
        : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

DefaultShader::~DefaultShader() {}

void DefaultShader::calculateAttributesLocations()
{
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");

    mHasTextureSlot = glGetUniformLocation(mHandle, "b_hasTexture");

    mOverrideTextureColorSlot =
            glGetUniformLocation(mHandle, "b_overrideTextureColor");

    glUseProgram(mHandle);

    glUniform1i(mHasTextureSlot, 0);

    glUniform1i(mOverrideTextureColorSlot, 0);

    mHasTextureFlag = false;
    mOverrideTextureColorFlag = false;
}

TextureShader::TextureShader(const char *name, const char *vertexShaderSrc,
                             const char *fragmentShaderSrc)
        : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}


void TextureShader::calculateAttributesLocations()
{
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mPremultipliedAlphaSlot = glGetUniformLocation(mHandle, "b_premultipliedAlpha");
}

ShadowShader::ShadowShader(const char *name, const char *vertexShaderSrc,
                             const char *fragmentShaderSrc)
        : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}


void ShadowShader::calculateAttributesLocations()
{
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mXDeltaSlot = glGetUniformLocation(mHandle, "u_xDelta");
    mYDeltaSlot = glGetUniformLocation(mHandle, "u_yDelta");

    mWeightSlot = glGetUniformLocation(mHandle, "u_weight");

}

PatternShader::PatternShader(const char *name, const char *vertexShaderSrc,
                             const char *fragmentShaderSrc)
        : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

void PatternShader::calculateAttributesLocations()
{
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mRepeatXSlot = glGetUniformLocation(mHandle, "repeatX");
    mRepeatYslot = glGetUniformLocation(mHandle, "repeatY");
    mTextureSizeSlot = glGetUniformLocation(mHandle, "textureSize");
    mPatternAlphaSlot = glGetUniformLocation(mHandle, "u_patternAlpha");

    glUseProgram(mHandle);
    SetRepeatMode("no-repeat");
}


GradientShader::GradientShader(const char *name,
                               const char *vertexShaderSrc,
                               const char *fragmentShaderSrc):GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}


void GradientShader::calculateAttributesLocations()
{
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mHasTextureSlot = glGetUniformLocation(mHandle, "b_hasTexture");

    mStopCountSlot = glGetUniformLocation(mHandle, "u_stopCount");

    mStop0ColorSlot = glGetUniformLocation(mHandle, "u_stopColor0");
    mStop1ColorSlot = glGetUniformLocation(mHandle, "u_stopColor1");
    mStop2ColorSlot = glGetUniformLocation(mHandle, "u_stopColor2");
    mStop3ColorSlot = glGetUniformLocation(mHandle, "u_stopColor3");
    mStop4ColorSlot = glGetUniformLocation(mHandle, "u_stopColor4");

    mStop0Slot = glGetUniformLocation(mHandle, "u_stop0");
    mStop1Slot = glGetUniformLocation(mHandle, "u_stop1");
    mStop2Slot = glGetUniformLocation(mHandle, "u_stop2");
    mStop3Slot = glGetUniformLocation(mHandle, "u_stop3");
    mStop4Slot = glGetUniformLocation(mHandle, "u_stop4");

    glUseProgram(mHandle);
    glUniform1i(mHasTextureSlot, 0);

    mHasTextureFlag = false;
}


LinearGradientShader::LinearGradientShader(const char *name,
                                           const char *vertexShaderSrc,
                                           const char *fragmentShaderSrc)
        : GradientShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

void LinearGradientShader::calculateAttributesLocations()
{
    mRangeStartSlot = glGetUniformLocation(mHandle, "u_startPos");
    mRangeEndSlot = glGetUniformLocation(mHandle, "u_endPos");
    GradientShader::calculateAttributesLocations();
}


RadialGradientShader::RadialGradientShader(const char *name,
                                           const char *vertexShaderSrc,
                                           const char *fragmentShaderSrc)
        : GradientShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

void RadialGradientShader::calculateAttributesLocations()
{
    mStartSlot = glGetUniformLocation(mHandle, "u_startPos");
    mEndSlot = glGetUniformLocation(mHandle, "u_endPos");
    GradientShader::calculateAttributesLocations();
}
