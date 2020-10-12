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

GShader::GShader(const char *name, const char *vertexShaderSrc,
                 const char *fragmentShaderSrc)
        : mHandle(0), mName(name), mColorSlot(-1), mTexcoordSlot(-1), mPositionSlot(-1), mTransfromSlot(-1), mShaderTransform(GTransformIdentity), mIsFirstCommit(true)
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
    
    isCompleted = true;
    GLuint vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
    if (vertexShader == 0)
    {
        isCompleted = false;
        return;
    }

    GLuint fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0)
    {
        isCompleted = false;
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
        FillLogInfo(log, "program_link_fail", "name:%s, error:%x", name, message);
        mErrVec.push_back(log);
        
        glDeleteProgram(mHandle);
        mHandle = 0;
        
        isCompleted = false;
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


void GShader::TraceErrorIfHas(GShader* shader, GCanvasHooks* hook, const std::string& contextId) {
    if (shader == nullptr) {
        return;
    }
    std::vector<GCanvasLog> log = shader->GetErrorVector();
    LOG_EXCEPTION_VECTOR(hook, contextId, log);
    shader->ClearErrorVector();
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
        FillLogInfo(log, "shader_compile_fail", "type:%s,name:%s,error:%s",
                shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                mName.data(), message);
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
    /**
        a   b   0   0
        c   d   0   0
        0   0   0   0
        tx  ty  0   1
     */
    
    if( !mIsFirstCommit && GTransformEqualToTransform(trans, mShaderTransform) ){
        return;
    }
    
    mShaderTransform = trans;
    
    GLfloat m[16] = {0.0f};
    m[0] = trans.a;
    m[1] = trans.b;
    m[4] = trans.c;
    m[5] = trans.d;
    m[12] = trans.tx;
    m[13] = trans.ty;
    m[15] = 1;


    glUniformMatrix4fv(GetTransformSlot(), 1, GL_FALSE, &(m[0]));
    mIsFirstCommit = false;

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
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mTextureSamplerSlot = glGetUniformLocation(mHandle, "u_texture");
    mShadowColorSlot = glGetUniformLocation(mHandle, "u_shadowColor");
}

BlurShader::BlurShader(const char *name, const char *vertexShaderSrc,
                             const char *fragmentShaderSrc)
        : GShader(name, vertexShaderSrc, fragmentShaderSrc)
{
    calculateAttributesLocations();
}


void BlurShader::calculateAttributesLocations()
{
    mTexcoordSlot = glGetAttribLocation(mHandle, "a_texCoord");
    mPositionSlot = glGetAttribLocation(mHandle, "a_position");
    mColorSlot = glGetAttribLocation(mHandle, "a_srcColor");
    mTransfromSlot = glGetUniformLocation(mHandle, "u_modelView");
    mXDeltaSlot = glGetUniformLocation(mHandle, "u_xDelta");
    mYDeltaSlot = glGetUniformLocation(mHandle, "u_yDelta");

    mWeightSlot = glGetUniformLocation(mHandle, "u_weight");

    mRadiusSlot = glGetUniformLocation(mHandle, "blur_radius");


    mOverrideTextureColorSlot = glGetUniformLocation(mHandle, "b_overrideTextureColor");
    
    glUseProgram(mHandle);
    
    //default is true, override textureColor, use shadowColor
    glUniform1i(mOverrideTextureColorSlot, 1);
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
    mInvertTransformSlot = glGetUniformLocation(mHandle, "u_invertTransform");
    GradientShader::calculateAttributesLocations();
}

void RadialGradientShader::SetInvertTransform(const GTransform &trans)
{
    GLfloat m[16] = {0.0f};
    m[0] = trans.a;
    m[1] = trans.b;
    m[4] = trans.c;
    m[5] = trans.d;
    m[12] = trans.tx;
    m[13] = trans.ty;
    m[15] = 1;
    
    glUniformMatrix4fv(mInvertTransformSlot, 1, GL_FALSE, &(m[0]));
}
