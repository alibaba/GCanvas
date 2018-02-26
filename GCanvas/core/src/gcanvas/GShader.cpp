/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GShader.h"
#include "../support/Log.h"

#ifdef ANDROID
#include "GPreCompiledShaders.h"
#endif

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

#ifdef ANDROID
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
        LOG_W("Failed to compile vertext shader!");
        return;
    }

    GLuint fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0)
    {
        LOG_W("Failed to compile fragment shader!");
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
        LOG_E("<%s link error>: %s", name, message);
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
    if(mHandle != 0) {
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

    GLint shaderLength = (GLint)strlen(shader);
    glShaderSource(shaderHandle, 1, &shader, &shaderLength);
    glCompileShader(shaderHandle);

    GLint compileResult;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE)
    {
        GLchar message[2048];
        int len = 0;
        glGetShaderInfoLog(shaderHandle, sizeof(message), &len, &message[0]);
        LOG_W("<%s compile error>: %s", shader, message);
        glDeleteShader(shaderHandle);
        return 0;
    }
    return shaderHandle;
}

void GShader::Bind()
{
//    LOG_D("webgl::exec  in onSurfaceChange mHandle: %d",mHandle);
    glUseProgram(mHandle);
    RestoreShaderState();
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

    glUniform1i(mTextureSamplerSlot, 0);
    glUniform1i(mHasTextureSlot, 0);
    glUniform1i(mOverrideTextureColorSlot, 0);

    mHasTextureFlag = false;
    mOverrideTextureColorFlag = false;
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

    glUniform1i(mTextureSamplerSlot, 0);
    SetRepeatMode("no-repeat");
}


LinearGradientShader::LinearGradientShader(const char *name,
                                           const char *vertexShaderSrc,
                                           const char *fragmentShaderSrc)
    : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

void LinearGradientShader::calculateAttributesLocations()
{
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mHasTextureSlot = glGetUniformLocation(mHandle, "b_hasTexture");

    mRangeStartSlot = glGetUniformLocation(mHandle, "u_startPos");
    mRangeEndSlot = glGetUniformLocation(mHandle, "u_endPos");
    mStopCountSlot = glGetUniformLocation(mHandle, "u_stopCount");
    mStop0Slot = glGetUniformLocation(mHandle, "u_stop0");
    mStop1slot = glGetUniformLocation(mHandle, "u_stop1");
    mStop2slot = glGetUniformLocation(mHandle, "u_stop2");
    mStop3Slot = glGetUniformLocation(mHandle, "u_stop3");
    mStop4Slot = glGetUniformLocation(mHandle, "u_stop4");

    glUniform1i(mTextureSamplerSlot, 0);
    glUniform1i(mHasTextureSlot, 0);

    mHasTextureFlag = false;
}


RadialGradientShader::RadialGradientShader(const char *name,
                                           const char *vertexShaderSrc,
                                           const char *fragmentShaderSrc)
    : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}

void RadialGradientShader::calculateAttributesLocations()
{
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mHasTextureSlot = glGetUniformLocation(mHandle, "b_hasTexture");

    mStartSlot = glGetUniformLocation(mHandle, "u_startPos");
    mEndSlot = glGetUniformLocation(mHandle, "u_endPos");
    mStopCountSlot = glGetUniformLocation(mHandle, "u_stopCount");
    mStop0Slot = glGetUniformLocation(mHandle, "u_stop0");
    mStop1Slot = glGetUniformLocation(mHandle, "u_stop1");
    mStop2Slot = glGetUniformLocation(mHandle, "u_stop2");
    mStop3Slot = glGetUniformLocation(mHandle, "u_stop3");
    mStop4Slot = glGetUniformLocation(mHandle, "u_stop4");

    glUniform1i(mTextureSamplerSlot, 0);
    glUniform1i(mHasTextureSlot, 0);

    mHasTextureFlag = false;
}
