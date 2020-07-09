/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/


#include "GCommandDecoderWebGL.hpp"
#include "GWebGLRenderContextInner.hpp"
#include "../webgl/GWebGLRenderContext.hpp"
#include "../support/Log.h"
#include "../commandbuffer/GCommandDecoder.hpp"
#include "../support/Util.h"
#include <sstream>
#include <string>
#include <codecvt>
#include <stdlib.h>


namespace gcanvas
{

//#define WEBGL_FUNC_ARRAY_ASSIGN(NAME) \
//mWebGLFuncs[GWEBGL_FUNC_##NAME] = std::bind(&GCommandDecoderWebGL::WebGL_##NAME, this, std::placeholders::_1, std::placeholders::_2);


#define WEBGL_SYNC_FUNC_ID_INSERT(NAME) \
mWebGLSyncFuncSet.insert(GWEBGL_FUNC_##NAME);

//无返回值的，参数0个
#define WEBGL_FUNC_IMP_PARAM_0(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    NAME(); \
    LOG_D("[webgl::exec] %s(), error=%d", #NAME,getError()); \
}

//无返回值的，参数1个int
#define WEBGL_FUNC_IMP_PARAM_1_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t *v = buffer.parseValue<uint32_t>(); \
    if( !v ) return; \
    NAME(*v); \
    LOG_D("[webgl::exec] %s(%d), error=%d", #NAME,*v,getError()); \
}

#define WEBGL_FUNC_IMP_PARAM_1_INT_VALUE(NAME, FUNC) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t *v = buffer.parseValue<uint32_t>(); \
    if( !v ) return; \
    FUNC(*v); \
    LOG_D("[webgl::exec] %s(%d), error=%d", #NAME,*v,getError()); \
}

//无返回值的，参数2个int
#define WEBGL_FUNC_IMP_PARAM_2_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t v[2]; \
    bool ret = buffer.parseArray<uint32_t>(v, 2); \
    if( !ret ) return; \
    NAME(v[0], v[1]); \
    LOG_D("[webgl::exec] %s(%d,%d), error=%d", #NAME,v[0],v[1],getError()); \
}

//无返回值的，参数3个int
#define WEBGL_FUNC_IMP_PARAM_3_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t v[3]; \
    bool ret = buffer.parseArray<uint32_t>(v, 3); \
    if( !ret ) return; \
    NAME(v[0], v[1], v[2]); \
    LOG_D("[webgl::exec] %s(%d,%d,%d), error=%d", #NAME,v[0],v[1],v[2],getError()); \
}

//无返回值的，参数4个int
#define WEBGL_FUNC_IMP_PARAM_4_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t v[4]; \
    bool ret = buffer.parseArray<uint32_t>(v, 4); \
    if( !ret ) return; \
    NAME(v[0], v[1], v[2], v[3]); \
    LOG_D("[webgl::exec] %s(%d,%d,%d,%d), error=%d", #NAME,v[0],v[1],v[2],v[3],getError()); \
}

//无返回值的，参数5个int
#define WEBGL_FUNC_IMP_PARAM_5_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t v[5]; \
    bool ret = buffer.parseArray<uint32_t>(v, 5); \
    if( !ret ) return; \
    NAME(v[0], v[1], v[2], v[3], v[4]); \
    LOG_D("[webgl::exec] %s(%d,%d,%d,%d,%d), error=%d", #NAME,v[0],v[1],v[2],v[3],v[4],getError()); \
}

//无返回值的，参数8个int
#define WEBGL_FUNC_IMP_PARAM_8_INT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    uint32_t v[8]; \
    bool ret = buffer.parseArray<uint32_t>(v, 8); \
    if( !ret ) return; \
    NAME(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]); \
    LOG_D("[webgl::exec] %s(%d,%d,%d,%d,%d,%d,%d,%d), error=%d", #NAME,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7], getError()); \
}


//无返回值的，参数1个float
#define WEBGL_FUNC_IMP_PARAM_1_FLOAT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    float *v = buffer.parseValue<float>(); \
    if( !v ) return; \
    NAME(*v); \
    LOG_D("[webgl::exec] %s(%f), error=%d", #NAME,v[0],getError()); \
}

#define WEBGL_FUNC_IMP_PARAM_1_FLOAT_VALUE(NAME, FUNC) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    float *v = buffer.parseValue<float>(); \
    if( !v ) return; \
    FUNC(*v); \
    LOG_D("[webgl::exec] %s(%f), error=%d", #NAME,v[0],,getError()); \
}

//无返回值的，参数2个float
#define WEBGL_FUNC_IMP_PARAM_2_FLOAT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    float v[2]; \
    bool ret = buffer.parseArray<float>(v, 2); \
    if( !ret ) return; \
    NAME(v[0], v[1]); \
    LOG_D("[webgl::exec] %s(%f,%f), error=%d", #NAME,v[0],v[1],getError()); \
}

#define WEBGL_FUNC_IMP_PARAM_2_FLOAT_VALUE(NAME, FUNC) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    float v[2]; \
    bool ret = buffer.parseArray<float>(v, 2); \
    if( !ret ) return; \
    FUNC(v[0], v[1]); \
    LOG_D("[webgl::exec] %s(%f,%f,%f), error=%d", #NAME,v[0],v[1],v[2],getError()); \
}

//无返回值的，参数4个float
#define WEBGL_FUNC_IMP_PARAM_4_FLOAT(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    float v[4]; \
    bool ret = buffer.parseArray<float>(v, 4); \
    if( !ret ) return; \
    NAME(v[0], v[1], v[2], v[3]); \
    LOG_D("[webgl::exec] %s(%f,%f,%f,%f), error=%d", #NAME,v[0],v[1],v[2],v[3],getError()); \
}

//错误E
#define DECODER_PARSE_ERROR LOG_E("%s parse failed !!", __FUNCTION__);


//static GCommandFuncNew mWebGLFuncs[GWEBGL_FUNC_COUNT] = { nullptr };


//std::unordered_set<unsigned int> GCommandDecoderWebGL::mWebGLSyncFuncSet = {};
static WebGL::GWebGLRenderContext* mRenderContext;

GCommandDecoderWebGL::GCommandDecoderWebGL(WebGL::GWebGLRenderContext* renderContext)
{
    mRenderContext = renderContext;
#ifdef DEBUG
    SetLogLevel(LOG_LEVEL_ERROR);
#else
    SetLogLevel(LOG_LEVEL_ERROR);
#endif
    InitCommands();
}

GCommandDecoderWebGL::~GCommandDecoderWebGL()
{

}

void GCommandDecoderWebGL::InitCommands()
{
    //init WebGL1.0 API
//    WEBGL_FUNC_ARRAY_ASSIGN(activeTexture);
//    WEBGL_FUNC_ARRAY_ASSIGN(attachShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindAttribLocation);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindBuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindFramebuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindRenderbuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindTexture);
//    WEBGL_FUNC_ARRAY_ASSIGN(blendColor);
//    WEBGL_FUNC_ARRAY_ASSIGN(blendEquation);
//    WEBGL_FUNC_ARRAY_ASSIGN(blendEquationSeparate);
//    WEBGL_FUNC_ARRAY_ASSIGN(blendFunc);
//    WEBGL_FUNC_ARRAY_ASSIGN(blendFuncSeparate);
//    WEBGL_FUNC_ARRAY_ASSIGN(bufferData_size);
//    WEBGL_FUNC_ARRAY_ASSIGN(bufferData_buffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(bufferSubData);
//    WEBGL_FUNC_ARRAY_ASSIGN(checkFramebufferStatus);
//    WEBGL_FUNC_ARRAY_ASSIGN(clear);
//    WEBGL_FUNC_ARRAY_ASSIGN(clearColor);
//    WEBGL_FUNC_ARRAY_ASSIGN(clearDepth);
//    WEBGL_FUNC_ARRAY_ASSIGN(clearStencil);
//    WEBGL_FUNC_ARRAY_ASSIGN(colorMask);
//    WEBGL_FUNC_ARRAY_ASSIGN(commit);
//    WEBGL_FUNC_ARRAY_ASSIGN(compileShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(compressedTexImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(compressedTexSubImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(copyTexImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(copyTexSubImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(createBuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(createFramebuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(createProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(createRenderbuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(createShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(createTexture);
//    WEBGL_FUNC_ARRAY_ASSIGN(cullFace);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteBuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteFramebuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteRenderbuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteTexture);
//    WEBGL_FUNC_ARRAY_ASSIGN(depthFunc);
//    WEBGL_FUNC_ARRAY_ASSIGN(depthMask);
//    WEBGL_FUNC_ARRAY_ASSIGN(depthRange);
//    WEBGL_FUNC_ARRAY_ASSIGN(detachShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(disable);
//    WEBGL_FUNC_ARRAY_ASSIGN(disableVertexAttribArray);
//    WEBGL_FUNC_ARRAY_ASSIGN(drawArrays);
//    WEBGL_FUNC_ARRAY_ASSIGN(drawElements);
//    WEBGL_FUNC_ARRAY_ASSIGN(enable);
//    WEBGL_FUNC_ARRAY_ASSIGN(enableVertexAttribArray);
//    WEBGL_FUNC_ARRAY_ASSIGN(finish);
//    WEBGL_FUNC_ARRAY_ASSIGN(flush);
//    WEBGL_FUNC_ARRAY_ASSIGN(framebufferRenderbuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(framebufferTexture2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(frontFace);
//    WEBGL_FUNC_ARRAY_ASSIGN(generateMipmap);
//    WEBGL_FUNC_ARRAY_ASSIGN(getActiveAttrib);
//    WEBGL_FUNC_ARRAY_ASSIGN(getActiveUniform);
//    WEBGL_FUNC_ARRAY_ASSIGN(getAttachedShaders);
//    WEBGL_FUNC_ARRAY_ASSIGN(getAttribLocation);
//    WEBGL_FUNC_ARRAY_ASSIGN(getBufferParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getContextAttributes);
//    WEBGL_FUNC_ARRAY_ASSIGN(getError);
//    WEBGL_FUNC_ARRAY_ASSIGN(getExtension);
//    WEBGL_FUNC_ARRAY_ASSIGN(getFramebufferAttachmentParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getProgramInfoLog);
//    WEBGL_FUNC_ARRAY_ASSIGN(getProgramParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getRenderbufferParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getShaderInfoLog);
//    WEBGL_FUNC_ARRAY_ASSIGN(getShaderParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getShaderPrecisionFormat);
//    WEBGL_FUNC_ARRAY_ASSIGN(getShaderSource);
//    WEBGL_FUNC_ARRAY_ASSIGN(getSupportedExtensions);
//    WEBGL_FUNC_ARRAY_ASSIGN(getTexParameter);
//    WEBGL_FUNC_ARRAY_ASSIGN(getUniform);
//    WEBGL_FUNC_ARRAY_ASSIGN(getUniformLocation);
//    WEBGL_FUNC_ARRAY_ASSIGN(getVertexAttrib);
//    WEBGL_FUNC_ARRAY_ASSIGN(getVertexAttribOffset);
//    WEBGL_FUNC_ARRAY_ASSIGN(hint);
//    WEBGL_FUNC_ARRAY_ASSIGN(isBuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(isContextLost);
//    WEBGL_FUNC_ARRAY_ASSIGN(isEnabled);
//    WEBGL_FUNC_ARRAY_ASSIGN(isFramebuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(isProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(isRenderbuffer);
//    WEBGL_FUNC_ARRAY_ASSIGN(isShader);
//    WEBGL_FUNC_ARRAY_ASSIGN(isTexture);
//    WEBGL_FUNC_ARRAY_ASSIGN(lineWidth);
//    WEBGL_FUNC_ARRAY_ASSIGN(linkProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(pixelStorei);
//    WEBGL_FUNC_ARRAY_ASSIGN(polygonOffset);
//    WEBGL_FUNC_ARRAY_ASSIGN(readPixels);
//    WEBGL_FUNC_ARRAY_ASSIGN(renderbufferStorage);
//    WEBGL_FUNC_ARRAY_ASSIGN(sampleCoverage);
//    WEBGL_FUNC_ARRAY_ASSIGN(scissor);
//    WEBGL_FUNC_ARRAY_ASSIGN(shaderSource);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilFunc);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilFuncSeparate);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilMask);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilMaskSeparate);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilOp);
//    WEBGL_FUNC_ARRAY_ASSIGN(stencilOpSeparate);
//    WEBGL_FUNC_ARRAY_ASSIGN(texImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(texImage2D_null);
//    WEBGL_FUNC_ARRAY_ASSIGN(texImage2D_pixels);
//    WEBGL_FUNC_ARRAY_ASSIGN(texParameterf);
//    WEBGL_FUNC_ARRAY_ASSIGN(texParcreateShaderImage2D);
//    WEBGL_FUNC_ARRAY_ASSIGN(texSubImage2D_null);
//    WEBGL_FUNC_ARRAY_ASSIGN(texSubImage2D_pixels);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform1f);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform2f);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform3f);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform4f);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform1i);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform2i);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform3i);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform4i);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform1fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform2fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform3fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform4fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform1iv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform2iv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform3iv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniform4iv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniformMatrix2fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniformMatrix3fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(uniformMatrix4fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(useProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(validateProgram);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib1f);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib2f);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib3f);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib4f);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib1fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib2fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib3fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttrib4fv);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttribPointer);
//    WEBGL_FUNC_ARRAY_ASSIGN(viewport);
//
//    //init Extensions API
//    WEBGL_FUNC_ARRAY_ASSIGN(drawArraysInstancedANGLE);
//    WEBGL_FUNC_ARRAY_ASSIGN(drawElementsInstancedANGLE);
//    WEBGL_FUNC_ARRAY_ASSIGN(vertexAttribDivisorANGLE);
//    WEBGL_FUNC_ARRAY_ASSIGN(createVertexArrayOES);
//    WEBGL_FUNC_ARRAY_ASSIGN(deleteVertexArrayOES);
//    WEBGL_FUNC_ARRAY_ASSIGN(isVertexArrayOES);
//    WEBGL_FUNC_ARRAY_ASSIGN(bindVertexArrayOES);


    //WebGL1.0 Sync API
    WEBGL_SYNC_FUNC_ID_INSERT(checkFramebufferStatus);
    WEBGL_SYNC_FUNC_ID_INSERT(createBuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(createFramebuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(createProgram);
    WEBGL_SYNC_FUNC_ID_INSERT(createRenderbuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(createShader);
    WEBGL_SYNC_FUNC_ID_INSERT(createTexture);
    WEBGL_SYNC_FUNC_ID_INSERT(getActiveAttrib);
    WEBGL_SYNC_FUNC_ID_INSERT(getActiveUniform);
    WEBGL_SYNC_FUNC_ID_INSERT(getAttachedShaders);
    WEBGL_SYNC_FUNC_ID_INSERT(getAttribLocation);
    WEBGL_SYNC_FUNC_ID_INSERT(getBufferParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getContextAttributes);
    WEBGL_SYNC_FUNC_ID_INSERT(getError);
    WEBGL_SYNC_FUNC_ID_INSERT(getExtension);
    WEBGL_SYNC_FUNC_ID_INSERT(getFramebufferAttachmentParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getProgramInfoLog);
    WEBGL_SYNC_FUNC_ID_INSERT(getProgramParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getRenderbufferParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getShaderInfoLog);
    WEBGL_SYNC_FUNC_ID_INSERT(getShaderParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getShaderPrecisionFormat);
    WEBGL_SYNC_FUNC_ID_INSERT(getShaderSource);
    WEBGL_SYNC_FUNC_ID_INSERT(getSupportedExtensions);
    WEBGL_SYNC_FUNC_ID_INSERT(getTexParameter);
    WEBGL_SYNC_FUNC_ID_INSERT(getUniform);
    WEBGL_SYNC_FUNC_ID_INSERT(getUniformLocation);
    WEBGL_SYNC_FUNC_ID_INSERT(getVertexAttrib);
    WEBGL_SYNC_FUNC_ID_INSERT(getVertexAttribOffset);
    WEBGL_SYNC_FUNC_ID_INSERT(isBuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(isContextLost);
    WEBGL_SYNC_FUNC_ID_INSERT(isEnabled);
    WEBGL_SYNC_FUNC_ID_INSERT(isFramebuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(isProgram);
    WEBGL_SYNC_FUNC_ID_INSERT(isRenderbuffer);
    WEBGL_SYNC_FUNC_ID_INSERT(isShader);
    WEBGL_SYNC_FUNC_ID_INSERT(isTexture);
    WEBGL_SYNC_FUNC_ID_INSERT(readPixels);

    //Extension Sync API
    WEBGL_SYNC_FUNC_ID_INSERT(createVertexArrayOES);
    WEBGL_SYNC_FUNC_ID_INSERT(isVertexArrayOES);
}


bool GCommandDecoderWebGL::Decode(GCommandBuffer& buffer, GDecodeRes& res)
{
    static GCommandFuncPtr mWebGLFuncs[GWEBGL_FUNC_COUNT] = {
        nullptr,
        &GCommandDecoderWebGL::WebGL_activeTexture,
        &GCommandDecoderWebGL::WebGL_attachShader,
        &GCommandDecoderWebGL::WebGL_bindAttribLocation,
        &GCommandDecoderWebGL::WebGL_bindBuffer,
        &GCommandDecoderWebGL::WebGL_bindFramebuffer,
        &GCommandDecoderWebGL::WebGL_bindRenderbuffer,
        &GCommandDecoderWebGL::WebGL_bindTexture,
        &GCommandDecoderWebGL::WebGL_blendColor,
        &GCommandDecoderWebGL::WebGL_blendEquation,
        &GCommandDecoderWebGL::WebGL_blendEquationSeparate,
        &GCommandDecoderWebGL::WebGL_blendFunc,
        &GCommandDecoderWebGL::WebGL_blendFuncSeparate,
        &GCommandDecoderWebGL::WebGL_bufferData_size,
        &GCommandDecoderWebGL::WebGL_bufferData_buffer,
        &GCommandDecoderWebGL::WebGL_bufferSubData,
        &GCommandDecoderWebGL::WebGL_checkFramebufferStatus,
        &GCommandDecoderWebGL::WebGL_clear,
        &GCommandDecoderWebGL::WebGL_clearColor,
        &GCommandDecoderWebGL::WebGL_clearDepth,
        &GCommandDecoderWebGL::WebGL_clearStencil,
        &GCommandDecoderWebGL::WebGL_colorMask,
        &GCommandDecoderWebGL::WebGL_commit,
        &GCommandDecoderWebGL::WebGL_compileShader,
        &GCommandDecoderWebGL::WebGL_compressedTexImage2D,
        &GCommandDecoderWebGL::WebGL_compressedTexSubImage2D,
        &GCommandDecoderWebGL::WebGL_copyTexImage2D,
        &GCommandDecoderWebGL::WebGL_copyTexSubImage2D,
        &GCommandDecoderWebGL::WebGL_createBuffer,
        &GCommandDecoderWebGL::WebGL_createFramebuffer,
        &GCommandDecoderWebGL::WebGL_createProgram,
        &GCommandDecoderWebGL::WebGL_createRenderbuffer,
        &GCommandDecoderWebGL::WebGL_createShader,
        &GCommandDecoderWebGL::WebGL_createTexture,
        &GCommandDecoderWebGL::WebGL_cullFace,
        &GCommandDecoderWebGL::WebGL_deleteBuffer,
        &GCommandDecoderWebGL::WebGL_deleteFramebuffer,
        &GCommandDecoderWebGL::WebGL_deleteProgram,
        &GCommandDecoderWebGL::WebGL_deleteRenderbuffer,
        &GCommandDecoderWebGL::WebGL_deleteShader,
        &GCommandDecoderWebGL::WebGL_deleteTexture,
        &GCommandDecoderWebGL::WebGL_depthFunc,
        &GCommandDecoderWebGL::WebGL_depthMask,
        &GCommandDecoderWebGL::WebGL_depthRange,
        &GCommandDecoderWebGL::WebGL_detachShader,
        &GCommandDecoderWebGL::WebGL_disable,
        &GCommandDecoderWebGL::WebGL_disableVertexAttribArray,
        &GCommandDecoderWebGL::WebGL_drawArrays,
        &GCommandDecoderWebGL::WebGL_drawElements,
        &GCommandDecoderWebGL::WebGL_enable,
        &GCommandDecoderWebGL::WebGL_enableVertexAttribArray,
        &GCommandDecoderWebGL::WebGL_finish,
        &GCommandDecoderWebGL::WebGL_flush,
        &GCommandDecoderWebGL::WebGL_framebufferRenderbuffer,
        &GCommandDecoderWebGL::WebGL_framebufferTexture2D,
        &GCommandDecoderWebGL::WebGL_frontFace,
        &GCommandDecoderWebGL::WebGL_generateMipmap,
        &GCommandDecoderWebGL::WebGL_getActiveAttrib,
        &GCommandDecoderWebGL::WebGL_getActiveUniform,
        &GCommandDecoderWebGL::WebGL_getAttachedShaders,
        &GCommandDecoderWebGL::WebGL_getAttribLocation,
        &GCommandDecoderWebGL::WebGL_getBufferParameter,
        &GCommandDecoderWebGL::WebGL_getContextAttributes,
        &GCommandDecoderWebGL::WebGL_getError,
        &GCommandDecoderWebGL::WebGL_getExtension,
        &GCommandDecoderWebGL::WebGL_getFramebufferAttachmentParameter,
        &GCommandDecoderWebGL::WebGL_getParameter,
        &GCommandDecoderWebGL::WebGL_getProgramInfoLog,
        &GCommandDecoderWebGL::WebGL_getProgramParameter,
        &GCommandDecoderWebGL::WebGL_getRenderbufferParameter,
        &GCommandDecoderWebGL::WebGL_getShaderInfoLog,
        &GCommandDecoderWebGL::WebGL_getShaderParameter,
        &GCommandDecoderWebGL::WebGL_getShaderPrecisionFormat,
        &GCommandDecoderWebGL::WebGL_getShaderSource,
        &GCommandDecoderWebGL::WebGL_getSupportedExtensions,
        &GCommandDecoderWebGL::WebGL_getTexParameter,
        &GCommandDecoderWebGL::WebGL_getUniform,
        &GCommandDecoderWebGL::WebGL_getUniformLocation,
        &GCommandDecoderWebGL::WebGL_getVertexAttrib,
        &GCommandDecoderWebGL::WebGL_getVertexAttribOffset,
        &GCommandDecoderWebGL::WebGL_hint,
        &GCommandDecoderWebGL::WebGL_isBuffer,
        &GCommandDecoderWebGL::WebGL_isContextLost,
        &GCommandDecoderWebGL::WebGL_isEnabled,
        &GCommandDecoderWebGL::WebGL_isFramebuffer,
        &GCommandDecoderWebGL::WebGL_isProgram,
        &GCommandDecoderWebGL::WebGL_isRenderbuffer,
        &GCommandDecoderWebGL::WebGL_isShader,
        &GCommandDecoderWebGL::WebGL_isTexture,
        &GCommandDecoderWebGL::WebGL_lineWidth,
        &GCommandDecoderWebGL::WebGL_linkProgram,
        &GCommandDecoderWebGL::WebGL_pixelStorei,
        &GCommandDecoderWebGL::WebGL_polygonOffset,
        &GCommandDecoderWebGL::WebGL_readPixels,
        &GCommandDecoderWebGL::WebGL_renderbufferStorage,
        &GCommandDecoderWebGL::WebGL_sampleCoverage,
        &GCommandDecoderWebGL::WebGL_scissor,
        &GCommandDecoderWebGL::WebGL_shaderSource,
        &GCommandDecoderWebGL::WebGL_stencilFunc,
        &GCommandDecoderWebGL::WebGL_stencilFuncSeparate,
        &GCommandDecoderWebGL::WebGL_stencilMask,
        &GCommandDecoderWebGL::WebGL_stencilMaskSeparate,
        &GCommandDecoderWebGL::WebGL_stencilOp,
        &GCommandDecoderWebGL::WebGL_stencilOpSeparate,
        &GCommandDecoderWebGL::WebGL_texImage2D,
        &GCommandDecoderWebGL::WebGL_texImage2D_null,
        &GCommandDecoderWebGL::WebGL_texImage2D_pixels,
        &GCommandDecoderWebGL::WebGL_texParameterf,
        &GCommandDecoderWebGL::WebGL_texParameteri,
        &GCommandDecoderWebGL::WebGL_texSubImage2D,
        &GCommandDecoderWebGL::WebGL_texSubImage2D_null,
        &GCommandDecoderWebGL::WebGL_texSubImage2D_pixels,
        &GCommandDecoderWebGL::WebGL_uniform1f,
        &GCommandDecoderWebGL::WebGL_uniform2f,
        &GCommandDecoderWebGL::WebGL_uniform3f,
        &GCommandDecoderWebGL::WebGL_uniform4f,
        &GCommandDecoderWebGL::WebGL_uniform1i,
        &GCommandDecoderWebGL::WebGL_uniform2i,
        &GCommandDecoderWebGL::WebGL_uniform3i,
        &GCommandDecoderWebGL::WebGL_uniform4i,
        &GCommandDecoderWebGL::WebGL_uniform1fv,
        &GCommandDecoderWebGL::WebGL_uniform2fv,
        &GCommandDecoderWebGL::WebGL_uniform3fv,
        &GCommandDecoderWebGL::WebGL_uniform4fv,
        &GCommandDecoderWebGL::WebGL_uniform1iv,
        &GCommandDecoderWebGL::WebGL_uniform2iv,
        &GCommandDecoderWebGL::WebGL_uniform3iv,
        &GCommandDecoderWebGL::WebGL_uniform4iv,
        &GCommandDecoderWebGL::WebGL_uniformMatrix2fv,
        &GCommandDecoderWebGL::WebGL_uniformMatrix3fv,
        &GCommandDecoderWebGL::WebGL_uniformMatrix4fv,
        &GCommandDecoderWebGL::WebGL_useProgram,
        &GCommandDecoderWebGL::WebGL_validateProgram,
        &GCommandDecoderWebGL::WebGL_vertexAttrib1f,
        &GCommandDecoderWebGL::WebGL_vertexAttrib2f,
        &GCommandDecoderWebGL::WebGL_vertexAttrib3f,
        &GCommandDecoderWebGL::WebGL_vertexAttrib4f,
        &GCommandDecoderWebGL::WebGL_vertexAttrib1fv,
        &GCommandDecoderWebGL::WebGL_vertexAttrib2fv,
        &GCommandDecoderWebGL::WebGL_vertexAttrib3fv,
        &GCommandDecoderWebGL::WebGL_vertexAttrib4fv,
        &GCommandDecoderWebGL::WebGL_vertexAttribPointer,
        &GCommandDecoderWebGL::WebGL_viewport,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &GCommandDecoderWebGL::WebGL_drawArraysInstancedANGLE,  //200
        &GCommandDecoderWebGL::WebGL_drawElementsInstancedANGLE,
        &GCommandDecoderWebGL::WebGL_vertexAttribDivisorANGLE,
        &GCommandDecoderWebGL::WebGL_createVertexArrayOES,
        &GCommandDecoderWebGL::WebGL_deleteVertexArrayOES,
        &GCommandDecoderWebGL::WebGL_isVertexArrayOES,
        &GCommandDecoderWebGL::WebGL_bindVertexArrayOES,
    };

    uint32_t *v;

    while ( (v = buffer.parseValue<uint32_t>()) != nullptr )
    {
        uint32_t funcId = *v;

        if( funcId < 0 || funcId >= GWEBGL_FUNC_COUNT )
        {
            LOG_E("[Error] Command buffer with wrong WebGL API ID:%d", funcId);
            return false;
        }

        GCommandFuncPtr func = mWebGLFuncs[funcId];
        if( !func )
        {
            LOG_E("[Error] Can't find WebGL API ID::%d", funcId);
            return false;
        }

        LOG_D("==========API ID:[%d-%s]==========", funcId, WEBGL_API(funcId) );
        func(buffer, res);
        if( res.hasResult && IsSyncCommand(funcId) )
        {
            return true;
        }

    }

    return true;
}


/////////////////////////////////////////
///      WebGL API
/////////////////////////////////////////
void GCommandDecoderWebGL::WebGL_activeTexture(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    activeTexture(*v);

    LOG_D("[webgl::exec] activeTexture(%d), error=%d",
          *v, getError());
}

void GCommandDecoderWebGL::WebGL_attachShader(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;
    attachShader(v[0], v[1]);
    LOG_D("[webgl::exec] attachShader(%d,%d), error=%d",v[0],v[1],getError());
}

void GCommandDecoderWebGL::WebGL_bindAttribLocation(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray<uint32_t>(v, 3);
    if( !ret ) return;
    size_t size = v[2];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( !u16Char ) return;
        std::string u8Str = U16StrToU8Str(u16Char);
        bindAttribLocation(v[0], v[1], u8Str.c_str());

        LOG_D("[webgl::exec] bindAttribLocation(%d,%d), error=%d",
                v[0], v[1], getError());
    }
}

WEBGL_FUNC_IMP_PARAM_2_INT(bindBuffer)
void GCommandDecoderWebGL::WebGL_bindFramebuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    int32_t v[2];
    bool ret = buffer.parseArray<int32_t>(v, 2);
    if( !ret ) return;
    if( v[1] > 0 )
    {
        bindFramebuffer(v[0], v[1]);
        LOG_D("[webgl::exec] bindFramebuffer(%s,%d), error=%d",
                WebGL_CONST(v[0]), v[1], getError());
    }
    else
    {
        if( mRenderContext->mBindFramebufferNullFunc ){
            mRenderContext->BindFramebufferNull();
        } else {
            //todo 暂时处理参数-1的情况
            if(v[1] < 0){
                v[1] = 0;
            }
            bindFramebuffer(v[0], v[1]);
        }
        LOG_D("[webgl::exec] bindFramebuffer_null(%s,%d), error=%d",
                WebGL_CONST(v[0]), v[1], getError());
    }
}
WEBGL_FUNC_IMP_PARAM_2_INT(bindRenderbuffer)
WEBGL_FUNC_IMP_PARAM_2_INT(bindTexture)

void GCommandDecoderWebGL::WebGL_blendColor(GCommandBuffer& buffer, GDecodeRes& res)
{
    float v[4];
    bool ret = buffer.parseArray<float>(v, 4);
    if( !ret ) return;
    blendColor(v[0], v[1], v[2], v[3]);
    LOG_D("[webgl::exec] blendColor(%f,%f,%f,%f), error=%d",
          v[0], v[1], v[2], v[3], getError());
}

WEBGL_FUNC_IMP_PARAM_1_INT(blendEquation)
WEBGL_FUNC_IMP_PARAM_2_INT(blendEquationSeparate)
WEBGL_FUNC_IMP_PARAM_2_INT(blendFunc)
WEBGL_FUNC_IMP_PARAM_4_INT(blendFuncSeparate)

void GCommandDecoderWebGL::WebGL_bufferData_size(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint64_t *size = buffer.parseValue<uint64_t>();
    if( !size ) return;

    bufferData(v[0], (size_t)*size, NULL, v[1]);
    LOG_D("[webgl::exec] bufferData_size(%s,%d,%d), error=%d",
          WebGL_CONST(v[0]), size, WebGL_CONST(v[1]), getError());
}

void GCommandDecoderWebGL::WebGL_bufferData_buffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray<uint32_t>(v, 3);
    if( !ret ) return;

    size_t size = v[2];
    if( v[2] > 0  )
    {
        const void *data = buffer.parseBufferAlign(size);
        if( data != nullptr )
        {
            bufferData(v[0], size, data, v[1]);

            LOG_D("[webgl::exec] bufferData_buffer(%s,%d,%s), error=%d",
                  WebGL_CONST(v[0]), size, WebGL_CONST(v[1]), getError());
        }
    }
}

void GCommandDecoderWebGL::WebGL_bufferSubData(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray<uint32_t>(v, 3);
    if( !ret ) return;

    size_t size = v[2];
    if( size > 0  )
    {
        const void *data = buffer.parseBufferAlign(size);
        if ( data != nullptr )
        {
            bufferSubData(v[0], v[1], size, data );

            LOG_D("[webgl::exec] bufferSubData(%s,%d,%d), error=%d",
                  WebGL_CONST(v[0]), v[1], size, getError());
        }
    }
}


void GCommandDecoderWebGL::WebGL_checkFramebufferStatus(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;

    res.hasResult = true;
    res.type = GResType_UInt32;
    res.uintValue = checkFramebufferStatus(*v);
    LOG_D("[webgl::exec] checkFramebufferStatus(%s)=%s, error=%d",
          WebGL_CONST(*v), WebGL_CONST(res.uintValue), getError());
}

WEBGL_FUNC_IMP_PARAM_4_FLOAT(clearColor)

WEBGL_FUNC_IMP_PARAM_1_FLOAT(clearDepth)
WEBGL_FUNC_IMP_PARAM_1_INT(clearStencil)

WEBGL_FUNC_IMP_PARAM_4_INT(colorMask)

void GCommandDecoderWebGL::WebGL_commit(GCommandBuffer& buffer, GDecodeRes& res)
{
    flush();
    LOG_D("[webgl::exec] commit(), error=%d", getError());
    mRenderContext->mNeed_draw = true;
}

WEBGL_FUNC_IMP_PARAM_1_INT(compileShader)

void GCommandDecoderWebGL::WebGL_compressedTexImage2D(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[7];
    bool ret = buffer.parseArray<uint32_t>(v, 7);
    if( !ret ) return;
    uint32_t size = v[6];
    if (size > 0)
    {
        const void *data = buffer.parseBufferAlign(size);
        if ( data != nullptr )
        {
            compressedTexImage2D(v[0], v[1], v[2], v[3], v[4], v[5], size, data);
            LOG_D("[webgl::exec] compressedTexImage2D(%s,%d,%s,%d,%d,%d,%d), error=%d",
                  WebGL_CONST(v[0]), v[1], WebGL_CONST(v[2]), v[3], v[4], v[5], size, getError());
        }
    }
}

void GCommandDecoderWebGL::WebGL_compressedTexSubImage2D(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[8];
    bool ret = buffer.parseArray<uint32_t>(v, 7);
    if( !ret ) return;
    uint32_t size = v[7];
    if (size > 0)
    {
        const void *data = buffer.parseBuffer(size);
        if ( data != nullptr )
        {
            compressedTexSubImage2D(v[0], v[1], v[2], v[3], v[4], v[5], v[6], size, data);
            LOG_D("[webgl::exec] compressedTexSubImage2D(%s,%d,%s,%d,%d,%d,%d), error=%d",
                  WebGL_CONST(v[0]), v[1], v[2], v[3], v[4], v[5],WebGL_CONST(v[6]), size, getError());
        }
    }
}

WEBGL_FUNC_IMP_PARAM_8_INT(copyTexImage2D)
WEBGL_FUNC_IMP_PARAM_8_INT(copyTexSubImage2D)

//Sync API
void GCommandDecoderWebGL::WebGL_createBuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = WebGL::createBuffer();
    LOG_D("[webgl::exec] createBuffer()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateBuffer;
    }
}

void GCommandDecoderWebGL::WebGL_clear(GCommandBuffer& buffer, GDecodeRes& res){
    uint32_t *p = buffer.parseValue<uint32_t>();
    uint32_t bitmask=*p;
    WebGL::clear((GLbitfield)bitmask);
    LOG_D("[webgl::exec] clear(%d), error=%d", bitmask, getError());
    mRenderContext->mNeed_draw = true;
}

void GCommandDecoderWebGL::WebGL_createFramebuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = WebGL::createFrameBuffer();
    LOG_D("[webgl::exec] createFrameBuffer()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateFrameBuffer;
    }
}

void GCommandDecoderWebGL::WebGL_createProgram(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = createProgram();
    LOG_D("[webgl::exec] createProgram()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateProgram;
    }
}

void GCommandDecoderWebGL::WebGL_createRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = WebGL::createRenderbuffer();
    LOG_D("[webgl::exec] createRenderbuffer()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateRenderBuffer;
    }
}

void GCommandDecoderWebGL::WebGL_createShader(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *type = buffer.parseValue<uint32_t>();
    GLuint v = createShader(*type);
    LOG_D("[webgl::exec] createShader()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateShader;
    }
}

void GCommandDecoderWebGL::WebGL_createTexture(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = WebGL::createTexture();
    LOG_D("[webgl::exec] createTexture()=%d, error=%d", v, getError());
    if( v > 0 )
    {
        res.hasResult = true;
        res.uintValue = v;
        res.type = GResType_UInt32_CreateTexture;
    }
}

WEBGL_FUNC_IMP_PARAM_1_INT(cullFace)

void GCommandDecoderWebGL::WebGL_deleteBuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    WebGL::deleteBuffer(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::Buffer, *v);
    LOG_D("[webgl::exec] deleteBuffer(%d), error=%d", *v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteFramebuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    WebGL::deleteFramebuffer(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::FrameBuffer, *v);
    LOG_D("[webgl::exec] deleteFramebuffer(%d), error=%d", *v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteProgram(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    deleteProgram(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::Program, *v);
    LOG_D("[webgl::exec] deleteProgram(%d), error=%d", *v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    WebGL::deleteRenderbuffer(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::RenderBuffer, *v);
    LOG_D("[webgl::exec] deleteRenderbuffer(%d), error=%d", *v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteShader(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    deleteShader(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::Shader, *v);
    LOG_D("[webgl::exec] deleteShader(%d), error=%d", *v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteTexture(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    WebGL::deleteTexture(*v);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::Texture, *v);
    LOG_D("[webgl::exec] deleteTexture(%d), error=%d", *v, getError());
}


WEBGL_FUNC_IMP_PARAM_1_INT(depthFunc)
WEBGL_FUNC_IMP_PARAM_1_INT(depthMask)
WEBGL_FUNC_IMP_PARAM_2_FLOAT(depthRange);
WEBGL_FUNC_IMP_PARAM_2_INT(detachShader)
WEBGL_FUNC_IMP_PARAM_1_INT(disable)
WEBGL_FUNC_IMP_PARAM_1_INT(disableVertexAttribArray)

void GCommandDecoderWebGL::WebGL_drawArrays(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray<uint32_t>(v, 3);
    if( !ret ) return;
    drawArrays(v[0], v[1], v[2]);
    LOG_D("[webgl::exec] drawArrays(%s,%d,%d), error=%d", WebGL_CONST(v[0]), v[1], v[2], getError());
    mRenderContext->mNeed_draw = true;
    mRenderContext->mDrawCall++;
}

void GCommandDecoderWebGL::WebGL_drawElements(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[4];
    bool ret = buffer.parseArray<uint32_t>(v, 4);
    if( !ret ) return;
    drawElements(v[0], v[1], v[2], (char*)NULL +v[3]);
    LOG_D("[webgl::exec] drawElements(%s,%d,%s,%d), error=%d",
          WebGL_CONST(v[0]), v[1], WebGL_CONST(v[2]), v[3], getError());
    mRenderContext->mNeed_draw=true;
    mRenderContext->mDrawCall++;
}

WEBGL_FUNC_IMP_PARAM_1_INT(enable)
WEBGL_FUNC_IMP_PARAM_1_INT(enableVertexAttribArray)
WEBGL_FUNC_IMP_PARAM_4_INT(framebufferRenderbuffer);
WEBGL_FUNC_IMP_PARAM_5_INT(framebufferTexture2D);
WEBGL_FUNC_IMP_PARAM_1_INT(frontFace)
WEBGL_FUNC_IMP_PARAM_1_INT(generateMipmap)
void GCommandDecoderWebGL::WebGL_finish(GCommandBuffer& buffer, GDecodeRes& res){
    finish();
    LOG_D("[webgl::exec] finish(), error=%d", getError());
    mRenderContext->mNeed_draw = true;
}

//Sync API
void GCommandDecoderWebGL::WebGL_getActiveAttrib(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    GLsizei length;
    GLint size;
    GLenum type;

    char* name = nullptr;
    WebGL::getActiveAttrib(v[0], v[1], &length, &size, &type, &name);
    if( length > 0 && name != nullptr )
    {
        res.hasResult = true;

        //格式化"type,size,name"
        std::ostringstream ostr;
        ostr << type << "," << size << "," << name;
        std::string strValue = ostr.str();

        res.type = GResType_CharPtr;
        res.byteSize = strValue.size()+1;
        strcpy(res.charPtrValue, strValue.c_str());

        //new in WebGL::getActiveAttrib
        delete name;
    }
}


void GCommandDecoderWebGL::WebGL_getActiveUniform(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    GLsizei length;
    GLint size;
    GLenum type;
    char* name = nullptr;

    WebGL::getActiveUniform(v[0], v[1], &length, &size, &type, &name);
    if( length > 0 && name != nullptr )
    {
        res.hasResult = true;

        //格式化"type,size,name"
        std::ostringstream ostr;
        ostr << type << "," << size << "," << name;
        std::string strValue = ostr.str();

        res.type = GResType_CharPtr;
        res.byteSize = strValue.size()+1;
        strcpy(res.charPtrValue, strValue.c_str());

        delete name;
    }
}

void GCommandDecoderWebGL::WebGL_flush(GCommandBuffer& buffer, GDecodeRes& res){
    flush();
    LOG_D("[webgl::exec] finish(), error=%d", getError());
    mRenderContext->mNeed_draw=true;
}

void GCommandDecoderWebGL::WebGL_getAttachedShaders(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;
    GLsizei count = 0;
    GLuint *shaders = nullptr;
    WebGL::getAttachedShaders(*v, &count, &shaders);
    if( count > 0 && shaders != nullptr )
    {
        res.hasResult = true;

        //格式化"id,id,id"
        std::ostringstream ostr;
        for (int i=0; i<count; ++i)
        {
            ostr << shaders[i];
            if( i != count-1 )
            {
                ostr << ",";
            }
        }
        std::string strValue = ostr.str();

        res.type = GResType_CharPtr;
        res.byteSize = strValue.size()+1;
        strcpy(res.charPtrValue, strValue.c_str());

        delete shaders;
    }
}

void GCommandDecoderWebGL::WebGL_getAttribLocation(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    size_t size = v[1];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        std::string u8Str = U16StrToU8Str(u16Char);
        int loc = getAttribLocation(v[0], u8Str.c_str());
        LOG_D("[webgl::exec] getAttribLocation(%d,%s)=%d, error=%d", v[0], u8Str.c_str(), loc, getError());
        res.hasResult = true;
        res.intValue = loc;
        res.type = GResType_Int32;
    }
}

void GCommandDecoderWebGL::WebGL_getBufferParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    buffer.parseArray<uint32_t>(v, 2);

    GLint ret = WebGL::getBufferParameter(v[0], v[1]);
    LOG_D("[webgl::exec] getBufferParameter(%d,%s), error=%d",
          WebGL_CONST(v[0]), WebGL_CONST(v[1]), getError());
    res.hasResult = true;
    res.intValue = ret;
    res.type = ret > 0 ? GResType_Int32 : GResType_NULL;
}

void GCommandDecoderWebGL::WebGL_getContextAttributes(GCommandBuffer& buffer, GDecodeRes& res)
{
    res.hasResult = true;
    res.type = GResType_GetContextAttributes;
}

void GCommandDecoderWebGL::WebGL_getError(GCommandBuffer& buffer, GDecodeRes& res)
{
    res.hasResult = true;
    res.uintValue = getError();
    res.type = GResType_UInt32;
}

//WebGL_getExtension 直接返回扩展的函数的名字给上层,由上层直接处理
void GCommandDecoderWebGL::WebGL_getExtension(GCommandBuffer &buffer, GDecodeRes &res) {
    uint32_t v[1];
    bool ret = buffer.parseArray<uint32_t>(v, 1);
    if( !ret ) return;

    size_t size = v[0];
    if (size > 0) {
        char16_t *u16Char = (char16_t *) buffer.parseBufferAlign(size);
        std::string extName = U16StrToU8Str(u16Char);
        res.hasResult = true;
        res.type = GResType_CharPtr_GetExtention;
        strcpy(res.charPtrValue, extName.c_str());
        LOG_D("res.hasResult is %d extension name is %s",res.hasResult,extName.c_str());
    }else{

    }
}
void GCommandDecoderWebGL::WebGL_getFramebufferAttachmentParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray<uint32_t>(v, 3);
    if( !ret ) return;

    GLint param = WebGL::getFramebufferAttachmentParameter(v[0], v[1], v[2]);
    LOG_D("[webgl::exec] getFramebufferAttachmentParameter(%s,%s,%s), error=%d",
          WebGL_CONST(v[0]), WebGL_CONST(v[1]), WebGL_CONST(v[2]), param, getError());
    res.hasResult = true;
    res.intValue = param;
    res.type = GResType_UInt32;
}

void GCommandDecoderWebGL::WebGL_getParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    /** 不支持类型
        GL_COMPRESSED_TEXTURE_FORMATS
     */
    uint32_t pname = *(buffer.parseValue<uint32_t>());
    switch (pname)
    {
        //return type - Int32Array 0 element
        case GL_COMPRESSED_TEXTURE_FORMATS:
        {
            //TODO 返回
            LOG_E("getParameter not support GL_COMPRESSED_TEXTURE_FORMATS");
            return;
        }
        //return type - Float32Array 2 element
        case GL_ALIASED_LINE_WIDTH_RANGE:
        case GL_ALIASED_POINT_SIZE_RANGE:
        case GL_DEPTH_RANGE:
        {
            res.type = GResType_FloatPtr;
            res.length = 2;
            res.byteSize = 8;
            float *fv = res.floatPtrValue;
            getFloatv(pname, fv);
            LOG_D("[webgl::exec] getParameter(%s)=[%f,%f], error=%d",
                  WebGL_CONST(pname), fv[0], fv[1], getError());
            break;
        }
        //return type - Float32Array 4 element
        case GL_BLEND_COLOR:
        case GL_COLOR_CLEAR_VALUE:
        {
            res.type = GResType_FloatPtr;
            res.length = 4;
            res.byteSize = 16;
            float *fv = res.floatPtrValue;
            getFloatv(pname, fv);
            LOG_D("[webgl::exec] getParameter(%s)=[%f,%f,%f,%f], error=%d",
                  WebGL_CONST(pname), fv[0], fv[1], fv[2], fv[3], getError());
            break;
        }
        //return type - Int32Array 2 element
        case GL_MAX_VIEWPORT_DIMS:
        {
            res.type = GResType_Int32Ptr;
            res.length = 2;
            res.byteSize = 8;
            int32_t *iv = res.intPtrValue;
            getIntegerv(pname, iv);
            LOG_D("[webgl::exec] getParameter(%s)=[%d,%d], error=%d",
                  WebGL_CONST(pname), iv[0], iv[1], getError());
            break;
        }
        //return type - Int32Array 4 element
        case GL_SCISSOR_BOX:
        case GL_VIEWPORT:
        {
            res.type = GResType_Int32Ptr;
            res.length = 4;
            res.byteSize = 16;
            int32_t *iv = res.intPtrValue;
            getIntegerv(pname, iv);
            LOG_D("[webgl::exec] getParameter(%s)=[%d,%d,%d,%d], error=%d",
                  WebGL_CONST(pname), iv[0], iv[1], iv[2], iv[3], getError());
            break;
        }
        //return type - boolean[4] element
        case GL_COLOR_WRITEMASK:
        {
            res.type = GResType_BoolPtr;
            res.intPtrValue = new int32_t[4];
            res.length = 4;
            res.byteSize = 16;
            int32_t *iv = (int32_t*)res.intPtrValue;
            getIntegerv(pname, (int32_t*)iv);
            LOG_D("[webgl::exec] getParameter(%s)=[%d,%d,%d,%d], error=%d",
                  WebGL_CONST(pname), iv[0], iv[1], iv[2], iv[3], getError());
            break;
        }
        //return type - string
        case GL_RENDERER:
        case GL_SHADING_LANGUAGE_VERSION:
        case GL_VENDOR:
        case GL_VERSION:
        {
            const GLubyte* charStr = glGetString(pname);
            size_t len = strlen((char*)charStr) + 1;
            res.type = GResType_CharPtr;
            res.byteSize = len;
            res.length = len;
            strcpy(res.charPtrValue, (char*)charStr);
            LOG_D("[webgl::exec] getParameter(%s)=[%s], error=%d",
                  WebGL_CONST(pname), charStr, getError());
            break;
        }
        // return type - float
        case GL_DEPTH_CLEAR_VALUE:
        case GL_LINE_WIDTH:
        case GL_POLYGON_OFFSET_FACTOR:
        case GL_POLYGON_OFFSET_UNITS:
        case GL_SAMPLE_COVERAGE_VALUE:
        {
            float v;
            getFloatv(pname, &v);
            res.floatValue = v;
            res.type = GResType_Float;
            LOG_D("[webgl::exec] getParameter(%s)=[%f], error=%d",
                WebGL_CONST(pname), v, getError());
            break;
        }
        case GL_BLEND:
        case GL_CULL_FACE:
        case GL_DEPTH_TEST:
        case GL_DEPTH_WRITEMASK:
        case GL_DITHER:
        case GL_POLYGON_OFFSET_FILL:
        case GL_SAMPLE_COVERAGE_INVERT:
        case GL_SCISSOR_TEST:
        case GL_STENCIL_TEST:
        {
            int32_t v;
            getIntegerv(pname, &v);
            res.boolValue = v;
            res.type = GResType_Bool;
            LOG_D("[webgl::exec] getParameter(%s)=[%d], error=%d",
                  WebGL_CONST(pname), v, getError());
            break;
        }
        //webgl object id
        case GL_ARRAY_BUFFER_BINDING:
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
        case GL_CURRENT_PROGRAM:
        case GL_FRAMEBUFFER_BINDING:
        case GL_RENDERBUFFER_BINDING:
        case GL_TEXTURE_BINDING_2D:
        case GL_TEXTURE_BINDING_CUBE_MAP:
        {
            int32_t v;
            getIntegerv(pname, &v);
            res.intValue = v;
            res.type = v > 0 ? GResType_Int32 : GResType_NULL;
            LOG_D("[webgl::exec] getParameter(%s)=[%d], error=%d",
                  WebGL_CONST(pname), v, getError());
            break;
        }
        default:
        {
            int32_t v;
            getIntegerv(pname, &v);
            res.intValue = v;
            res.type = GResType_Int32;
            LOG_D("[webgl::exec] getParameter(%s)=[%d], error=%d",
                  WebGL_CONST(pname), v, getError());
            break;
        }
        case 0x9240:    //GL_UNPACK_FLIP_Y_WEBGL
        {
            res.type = GResType_Bool;
            res.boolValue = mRenderContext->mUnpackFlipY_WebGL;
            break;
        }
        case 0x9241:    //GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL
        {
            res.type = GResType_Bool;
            res.boolValue = mRenderContext->mPremultiplyAlpha_WebGL;
            break;
        }
        case 0x9243:    //GL_UNPACK_COLORSPACE_CONVERSION_WEBGL
        {
            res.intValue = false;
            break;
        }
    }
    res.hasResult = true;
}

void GCommandDecoderWebGL::WebGL_getProgramInfoLog(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;

    std::string programInfoLog = WebGL::getProgramInfoLog(*v);
    res.hasResult = true;

    size_t len = programInfoLog.size()+1;
    res.type = GResType_CharPtr;
    res.byteSize = len;
    strcpy(res.charPtrValue, programInfoLog.c_str());

    LOG_D("[webgl::exec] getProgramInfoLog(%d)=[%s], error=%d",
          *v, programInfoLog.c_str(), getError());
}

void GCommandDecoderWebGL::WebGL_getProgramParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    res.hasResult = true;
    res.intValue = WebGL::getProgramParameter(v[0], v[1]);
    res.type = GResType_Int32;
    LOG_D("[webgl::exec] getProgramParameter(%d,%s)=%d, error=%d",
          v[0], WebGL_CONST(v[1]), res.intValue, getError());
}

void GCommandDecoderWebGL::WebGL_getRenderbufferParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if ( !ret ) return;

    res.hasResult = true;
    res.intValue = WebGL::getRenderbufferParameter(v[0], v[1]);
    res.type = GResType_Int32;
    LOG_D("[webgl::exec] getProgramParameter(%s,%s)=%d, error=%d",
        WebGL_CONST(v[0]), WebGL_CONST(v[1]), res.intValue, getError());
}

void GCommandDecoderWebGL::WebGL_getShaderInfoLog(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *v = buffer.parseValue<uint32_t>();
    if( !v ) return;

    std::string shaderInfoLog = WebGL::getShaderInfoLog(*v);

    size_t len = shaderInfoLog.size()+1;

    res.hasResult = true;
    res.type = GResType_CharPtr;
    res.byteSize = len;
    strcpy(res.charPtrValue, shaderInfoLog.c_str());
    LOG_D("[webgl::exec] getShaderInfoLog(%d)=[%s], error=%d",
          *v, shaderInfoLog.c_str(), getError());
}

void GCommandDecoderWebGL::WebGL_getShaderParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if(!ret) return;

    res.hasResult = true;
    res.uintValue = WebGL::getShaderParameter(v[0], v[1]);
    res.type = GResType_UInt32;

    LOG_D("[webgl::exec] getShaderParameter(%d,%s)=%d, error=%d",
        v[0], WebGL_CONST(v[1]), res.intValue, getError());
}

void GCommandDecoderWebGL::WebGL_getShaderPrecisionFormat(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if(!ret) return;

    GLint precision = 0;
    GLint range[2] = {0, 0};
    getShaderPrecisionFormat(v[0], v[1], range, &precision);
    //格式化"rangeMin,rangeMax,precision"
    std::ostringstream ostr;
    ostr << range[0] << "," << range[1] << "," << precision;
    std::string strValue = ostr.str();

    size_t len = strValue.size()+1;

    res.hasResult = true;
    res.type = GResType_CharPtr;
    res.length = len;
    strcpy(res.charPtrValue, strValue.c_str());

    LOG_D("[webgl::exec] getShaderPrecisionFormat(%s,%s)=(%s), error=%d",
         WebGL_CONST(v[0]), WebGL_CONST(v[1]), strValue.c_str(), getError());
}

void GCommandDecoderWebGL::WebGL_getShaderSource(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *shaderPtr = buffer.parseValue<uint32_t>();
    if( !shaderPtr ) return;

    GLint size;
    getShaderiv(*shaderPtr, GL_SHADER_SOURCE_LENGTH, &size);
    res.checkSize(size);
    res.hasResult = true;
    res.length = size;
    res.type = GResType_CharPtr;
    getShaderSource(*shaderPtr, size, nullptr, res.charPtrValue);
    LOG_D("[webgl::exec] getShaderSource(%d)=(%s), error=%d",
        *shaderPtr, res.charPtrValue, getError());
}


// 直接返回一个 GResType_CharPtr_GetSupportedExtention的枚举，由jsbinding层处理
void GCommandDecoderWebGL::WebGL_getSupportedExtensions(GCommandBuffer& buffer, GDecodeRes& res)
{
    res.hasResult = true;
    res.type = GResType_CharPtr_GetSupportedExtention;
}


void GCommandDecoderWebGL::WebGL_getTexParameter(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    int32_t params;
    getTexParameteriv(v[0], v[1], &params);  //webgl1.0获取整数类型即可
    res.hasResult = true;
    res.intValue = params;
    res.type = GResType_Int32;

    LOG_D("[webgl::exec] getTexParameter(%s,%s)=(%d), error=%d",
          WebGL_CONST(v[0]), WebGL_CONST(v[1]), res.intValue, getError());
}

void GCommandDecoderWebGL::WebGL_getUniform(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    GLuint programID = v[0];
    GLuint locationID = v[1];

    GLint maxLen;
    getProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    GLchar *nameBuffer = new GLchar[maxLen];
    GLenum type = 0;
    GLint num = 0;
    getProgramiv(programID, GL_ACTIVE_UNIFORMS, &num);
    for (int i=0; i<num; i++)
    {
        glGetActiveUniform(programID, i, maxLen, NULL, &maxLen, &type, nameBuffer);
        GLint loc = getUniformLocation(programID, nameBuffer);
        if ( loc ==  locationID)
        {
            //find location & type
            break;
        }
    }

    LOG_D("[webgl::exec] getUniform(%d,%d), error=%d", v[0], v[1], getError());

    res.hasResult = true;
    switch (type)
    {
        case GL_INT:
        case GL_BOOL:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
        {
            int32_t v;
            getUniformiv(programID, locationID, &v);
            res.type = GResType_Int32;
            res.intValue = v;
            break;
        }
        case GL_INT_VEC2:
        case GL_BOOL_VEC2:
        case GL_INT_VEC3:
        case GL_BOOL_VEC3:
        case GL_INT_VEC4:
        case GL_BOOL_VEC4:
        {
            uint8_t length = 4;
            if( type == GL_INT_VEC2 || type == GL_BOOL_VEC2  ){
                length = 2;
            } else if(type == GL_INT_VEC3 || type == GL_BOOL_VEC3){
                length = 3;
            }

            res.type = GResType_Int32Ptr;
            res.length = length;
            res.byteSize = length * 4;
            getUniformiv(programID, locationID, res.intPtrValue);
            break;
        }
        case GL_FLOAT:
        {
            float v;
            getUniformfv(programID, locationID, &v);
            res.type = GResType_Float;
            res.floatValue = v;
            break;
        }
        case GL_FLOAT_VEC2:
        case GL_FLOAT_MAT2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT4:
        {
            uint8_t length = 4;
            if( type == GL_INT_VEC2 || type == GL_BOOL_VEC2  ){
                length = 2;
            } else if(type == GL_INT_VEC3 || type == GL_BOOL_VEC3){
                length = 3;
            }

            res.type = GResType_FloatPtr;
            res.length = length;
            res.byteSize = length * 4;
            getUniformfv(programID, locationID, res.floatPtrValue);
            break;
        }
        default:
        {
            res.hasResult = false;
            break;
        }
    }
}

void GCommandDecoderWebGL::WebGL_getUniformLocation(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    size_t size = v[1];
    if (size > 0)
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        std::string u8Str = U16StrToU8Str(u16Char);

        res.hasResult = true;
        res.type = GResType_Int32;
        res.intValue = getUniformLocation(v[0], u8Str.c_str());
        LOG_D("[webgl::exec] getUniformLocation(%d, %s)=%d, error=%d", v[0], u8Str.c_str(), res.intValue, getError());
    }
}

void GCommandDecoderWebGL::WebGL_getVertexAttrib(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    res.hasResult = true;
    GLenum pname = v[1];
    switch (pname)
    {
        case GL_CURRENT_VERTEX_ATTRIB:
        {
            res.length = 4;
            res.byteSize = sizeof(float) * 4;
            res.type = GResType_FloatPtr;
            getVertexAttribfv(v[0], pname, res.floatPtrValue);
            LOG_D("[webgl::exec] getVertexAttrib(%d,%s)=[%f,%f,%f,%f], error=%d",
                  v[0], WebGL_CONST(pname), res.floatPtrValue[0], res.floatPtrValue[1], res.floatPtrValue[2], res.floatPtrValue[3], getError());
            break;
        }
        case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        {
            int32_t params;
            getVertexAttribiv(v[0], pname, &params);
            res.boolValue = params;
            res.type = GResType_Bool;
            LOG_D("[webgl::exec] getVertexAttrib(%d,%s)=%d, error=%d",
                  v[0], WebGL_CONST(pname), params, getError());
            break;
        }
        default:
        {
            int32_t params;
            getVertexAttribiv(v[0], pname, &params);
            res.intValue = params;
            res.type = GResType_Int32;
            LOG_D("[webgl::exec] getVertexAttrib(%d,%s)=%d, error=%d",
                  v[0], WebGL_CONST(pname), params, getError());
            break;
        }
    }
}

void GCommandDecoderWebGL::WebGL_getVertexAttribOffset(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t offset = 0;
    GLvoid *pointer = &offset;
    getVertexAttribPointerv(v[0], v[1], &pointer);
    res.hasResult = true;
    res.uintValue = *((uint32_t*)&offset);
    res.type = GResType_UInt32;

    LOG_D("[webgl::exec] getVertexAttrib(%d,%s)=%d, error=%d",
          v[0], WebGL_CONST(v[1]), res.intValue, getError());
}

WEBGL_FUNC_IMP_PARAM_2_INT(hint)

//Sync API
#define WEBGL_FUNC_IMP_IS_API(NAME) \
void GCommandDecoderWebGL::WebGL_##NAME(GCommandBuffer& buffer, GDecodeRes& res) \
{ \
    GLuint v = *(buffer.parseValue<GLuint>()); \
    bool ret = NAME(v); \
    res.hasResult = true; \
    res.boolValue = ret; \
    res.type = GResType_Bool;\
}

WEBGL_FUNC_IMP_IS_API(isBuffer)

void GCommandDecoderWebGL::WebGL_isContextLost(GCommandBuffer& buffer, GDecodeRes& res)
{
    res.hasResult = true;
    res.boolValue = false;
    res.type = GResType_Bool;
}

WEBGL_FUNC_IMP_IS_API(isEnabled)
WEBGL_FUNC_IMP_IS_API(isFramebuffer)
WEBGL_FUNC_IMP_IS_API(isProgram)
WEBGL_FUNC_IMP_IS_API(isRenderbuffer)
WEBGL_FUNC_IMP_IS_API(isShader)
WEBGL_FUNC_IMP_IS_API(isTexture)


WEBGL_FUNC_IMP_PARAM_1_FLOAT(lineWidth)
WEBGL_FUNC_IMP_PARAM_1_INT(linkProgram)

void GCommandDecoderWebGL::WebGL_pixelStorei(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    LOG_D("[webgl::exec] pixelStorei(%s,%d), error=%d",
          WebGL_CONST(v[0]), v[1], getError());
    switch (v[0])
    {
        case 0x9240:    //GL_UNPACK_FLIP_Y_WEBGL
        {
            mRenderContext->mUnpackFlipY_WebGL = v[1];
            break;
        }
        case 0x9241:    //GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL
        {
            mRenderContext->mPremultiplyAlpha_WebGL = v[1];
            break;
        }
        case 0x9243:    //GL_UNPACK_COLORSPACE_CONVERSION_WEBGL
        {
            break;
        }
        default:
        {
            pixelStorei(v[0], v[1]);
            break;
        }
    }
}

WEBGL_FUNC_IMP_PARAM_2_FLOAT(polygonOffset)

void GCommandDecoderWebGL::WebGL_readPixels(GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO 直接调binding接口
}

void GCommandDecoderWebGL::WebGL_renderbufferStorage(GCommandBuffer &buffer, GDecodeRes &res)
{
    uint32_t v[4];
    bool ret = buffer.parseArray<uint32_t>(v, 4);
    if( !ret ) return;
    if (v[1] == GL_DEPTH_STENCIL_OES) {
        v[1] = GL_DEPTH24_STENCIL8_OES;
    }
    renderbufferStorage(v[0], v[1], v[2], v[3]);
    LOG_D("[webgl::exec] renderbufferStorage(%d,%d,%d,%d), error=%d", v[0],v[1],v[2],v[3],getError());
}

void GCommandDecoderWebGL::WebGL_sampleCoverage(GCommandBuffer& buffer, GDecodeRes& res)
{
    float *fv = buffer.parseValue<float>();
    uint32_t *iv = buffer.parseValue<uint32_t>();
    if( fv == nullptr || iv == nullptr )
        return;

    sampleCoverage(*fv, *iv);
    LOG_D("[webgl::exec] sampleCoverage(%.2f,%d), error=%d", *fv, *iv, getError());
}

WEBGL_FUNC_IMP_PARAM_4_INT(scissor)

void GCommandDecoderWebGL::WebGL_shaderSource(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if(!ret) {
        DECODER_PARSE_ERROR
        return;
    }

    size_t size = v[1];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( u16Char )
        {
            std::string u8Str = U16StrToU8Str(u16Char);
            const char *pSource = u8Str.c_str();
            GLint len = (GLint)u8Str.length();
            shaderSource(v[0], 1, &pSource, &len);
            LOG_D("[webgl::exec] shaderSource(%d,%d,%s,%d), error=%d",
                  v[0], v[1], u8Str.c_str(), len, getError());
        }
    }
}

WEBGL_FUNC_IMP_PARAM_3_INT(stencilFunc)
WEBGL_FUNC_IMP_PARAM_4_INT(stencilFuncSeparate)
WEBGL_FUNC_IMP_PARAM_1_INT(stencilMask)
WEBGL_FUNC_IMP_PARAM_2_INT(stencilMaskSeparate)
WEBGL_FUNC_IMP_PARAM_3_INT(stencilOp)
WEBGL_FUNC_IMP_PARAM_4_INT(stencilOpSeparate)


    GLint ConvertImageInternalFormat(GLint internalformat, GLenum format, GLenum type) {
        if (type == GL_FLOAT) {
            if (format == GL_RGBA) {
                return GL_RGBA32F_EXT;
            } else if (format == GL_RGB) {
                return GL_RGB32F_EXT;
            }
        } else if (type == GL_HALF_FLOAT_OES) {
            if (format == GL_RGBA) {
                return GL_RGBA16F_EXT;
            } else if (format == GL_RGB) {
                return GL_RGB16F_EXT;
            }
        }
        return internalformat;
    }


/**
//6个参数
texImage2D(target, level, internalformat, format, type, ImageData? pixels);
//9个参数
texImage2D(target, level, internalformat, width, height, border, format, type, ArrayBufferView? pixels);
 */
void GCommandDecoderWebGL::WebGL_texImage2D(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[5];
    bool ret = buffer.parseArray<uint32_t>(v, 5);
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !ret || !uID ) return;

    //fetch pixels
    WebGL::JSBindingPixels gPixels;
    mRenderContext->FetchPixels(*uID, 0, &gPixels);

    short width = gPixels.width;
    short height = gPixels.height;


#ifdef  ANDROID
    uint32_t target = v[0];
    uint32_t level = v[1];
    uint32_t internalFormat = v[2];
    uint32_t format = v[3];
    uint32_t type = v[4];
    int templ = internalFormat;
    internalFormat = ConvertImageInternalFormat(internalFormat, format, type);
    v[2] = internalFormat;
#endif

    if ( gPixels.pixels != nullptr)
    {
        uint8_t* destPixels =  gPixels.pixels;
#ifdef  ANDROID
        if (format == GL_RGB)
        {
            size_t size = width*height*4;
            size_t newSize = width*height*3;
            uint8_t *newPixels = new uint8_t[newSize];
            for (int i = 0, newIdx=0; i < size; i++)
            {
                if( (i+1) % 4 == 0 ) continue;
                newPixels[newIdx++] = gPixels.pixels[i];
            }
            destPixels = newPixels;
        }
#endif
        if( mRenderContext->mUnpackFlipY_WebGL )
        {
            gcanvas::FlipPixel(gPixels.pixels, width, height);
        }
        texImage2D(v[0],v[1],v[2], width, height, 0, v[3], v[4], destPixels);

        LOG_D("[webgl::exec] texImage2D[imagedata](%s,%d,%d,%s,%s), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], WebGL_CONST(v[3]),WebGL_CONST(v[4]) ,getError());
    }
    else if( gPixels.textureId > 0 ) //image/canvas
    {
        size_t size = width*height*4;
        uint8_t *pixels = new uint8_t[size];
        gcanvas::ReadPixelsFromTexture(pixels, width, height, gPixels.textureId);
        if( mRenderContext->mUnpackFlipY_WebGL )
        {
            gcanvas::FlipPixel(pixels, width, height);
        }

        //format convert
        GLenum format = v[3];
        if (format == GL_RGB)
        {
            size_t newSize = width*height*3;
            uint8_t *newPixels = new uint8_t[newSize];
            for (int i = 0, newIdx=0; i < size; i++)
            {
                if( (i+1) % 4 == 0 ) continue;
                newPixels[newIdx++] = pixels[i];
            }

            delete [] pixels;
            pixels = newPixels;
            size = newSize;
        }

        texImage2D(v[0],v[1],v[2], width, height, 0, v[3], v[4], pixels);
        delete [] pixels;

        LOG_D("[webgl::exec] texImage2D[image/canvas](%s,%d,%d,%s,%s,%d,0), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], WebGL_CONST(v[3]),WebGL_CONST(v[4]), gPixels.textureId ,getError());
    }
}

void GCommandDecoderWebGL::WebGL_texImage2D_null(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[8];
    bool ret = buffer.parseArray<uint32_t>(v, 8);
    if( !ret ) return;

#ifdef  ANDROID
    uint32_t target = v[0];
    uint32_t level = v[1];
    uint32_t internalFormat = v[2];
    uint32_t format = v[6];
    uint32_t type = v[7];
    int templ = internalFormat;
    internalFormat = ConvertImageInternalFormat(internalFormat, format, type);
    v[2] = internalFormat;
#endif

    texImage2D(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],nullptr);
    LOG_D("[webgl::exec] texImage2D_null(%s,%d,%d,%d,%d,%d,%s,%s,0), error=%d",
          WebGL_CONST(v[0]), v[1], v[2], v[3], v[4], v[5], WebGL_CONST(v[6]),WebGL_CONST(v[7]),getError());
}

void GCommandDecoderWebGL::WebGL_texImage2D_pixels(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[9];
    bool ret = buffer.parseArray<uint32_t>(v, 9);
    if( !ret ) return;

    GLenum type = v[7];
    uint8_t eachByteSize = (type == GL_UNSIGNED_BYTE) ? 1 : 4;
    uint32_t size = v[8] * eachByteSize;

#ifdef  ANDROID
    uint32_t target = v[0];
    uint32_t level = v[1];
    uint32_t internalFormat = v[2];
    uint32_t format = v[6];
//    uint32_t type = v[7];
    int templ = internalFormat;
    internalFormat = ConvertImageInternalFormat(internalFormat, format, type);
    v[2] = internalFormat;
#endif

    if( size > 0 )
    {
        const void* data = nullptr;
        if ( type == GL_UNSIGNED_BYTE )
        {
            data = buffer.parseBufferAlign(size);
        }
        else
        {
            data = buffer.parseBuffer(size);
        }
        texImage2D(v[0],v[1],v[2],v[3],v[4],v[5],v[6],type,data);
        // LOG_E("WebGL_texImage2D_pixels:%i,%i,%i,%i,%i,%i,%i,%i", v[0],v[1],v[2],v[3],v[4],v[5],v[6], type);
        LOG_D("[webgl::exec] texImage2D_pixels(%s,%d,%d,%d,%d,%d,%s,%s,%d), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], v[3], v[4], v[5], WebGL_CONST(v[6]),WebGL_CONST(type), size, getError());
    }
}

void GCommandDecoderWebGL::WebGL_texParameterf(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    float *fv = buffer.parseValue<float>();
    if( !fv ) return;
    texParameterf(v[0], v[1], *fv);
    LOG_D("[webgl::exec] texParameterf(%s,%s,%d), error=%d",
          WebGL_CONST(v[0]), WebGL_CONST(v[1]), *fv, getError());

}

WEBGL_FUNC_IMP_PARAM_3_INT(texParameteri)

/**
7个参数
texSubImage2D(target, level, xoffset, yoffset, format, type, ImageData? pixels);
9个参数
texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, ArrayBufferView? pixels);
 */
void GCommandDecoderWebGL::WebGL_texSubImage2D(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[6];
    bool ret = buffer.parseArray<uint32_t>(v, 6);
    uint64_t *uID = buffer.parseValue<uint64_t>();

    if( !ret || !uID ) return;

    //fetch pixels
    WebGL::JSBindingPixels gPixels;
    mRenderContext->FetchPixels(*uID, 0, &gPixels);

    short width = gPixels.width;
    short height = gPixels.height;

    if ( gPixels.pixels != nullptr)
    {
        if( mRenderContext->mUnpackFlipY_WebGL )
        {
            gcanvas::FlipPixel(gPixels.pixels, width, height);
        }
        texSubImage2D(v[0],v[1],v[2],width,height,0,v[3],v[4],gPixels.pixels);

        LOG_D("[webgl::exec] texSubImage2D[imagedata](%s,%d,%d,%s,%s), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], WebGL_CONST(v[3]),WebGL_CONST(v[4]), getError());
    }
    else if( gPixels.textureId > 0 ) //image/canvas
    {
        size_t size = width*height*4;
        uint8_t *pixels = new uint8_t[size];
        gcanvas::ReadPixelsFromTexture(pixels, width, height, gPixels.textureId);

        if( mRenderContext->mUnpackFlipY_WebGL )
        {
            gcanvas::FlipPixel(pixels, width, height);
        }

        //format convert
        GLenum format = v[3];
        if (format == GL_RGB)
        {
            size_t newSize = width*height*3;
            uint8_t *newPixels = new uint8_t[newSize];
            for (int i = 0, newIdx=0; i < size; i++)
            {
                if( (i+1) % 4 == 0 ) continue;
                newPixels[newIdx++] = pixels[i];
            }

            delete [] pixels;
            pixels = newPixels;
            size = newSize;
        }

        texSubImage2D(v[0],v[1],v[2], width, height, 0, v[3], v[4], pixels);
        delete [] pixels;
        LOG_D("[webgl::exec] texSubImage2D[image/canvas](%s,%d,%d,%s,%s,%d), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], WebGL_CONST(v[3]),WebGL_CONST(v[4]), gPixels.textureId ,getError());
    }
}

void GCommandDecoderWebGL::WebGL_texSubImage2D_null(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[8];
    bool ret = buffer.parseArray<uint32_t>(v, 8);
    if( !ret ) return;

    texSubImage2D(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], nullptr);
    LOG_D("[webgl::exec] texSubImage2D_null(%s,%d,%d,%d,%d,%d,%s,%s,null), error=%d",
          WebGL_CONST(v[0]), v[1], v[2], v[3], v[4], v[5], WebGL_CONST(v[6]),WebGL_CONST(v[7]),getError());
}

void GCommandDecoderWebGL::WebGL_texSubImage2D_pixels(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[9];
    bool ret = buffer.parseArray<uint32_t>(v, 9);
    if( !ret ) return;

    GLenum type = v[7];
    uint8_t eachByteSize = (type == GL_UNSIGNED_BYTE) ? 1 : 4;
    uint32_t size = v[8] * eachByteSize;
    if( size > 0 )
    {
        const void* data = nullptr;

        if (type == GL_UNSIGNED_BYTE )
        {
            data = buffer.parseBufferAlign(size);
        }
        else
        {
            data = buffer.parseBuffer(size);
        }
        texSubImage2D(v[0],v[1],v[2],v[3],v[4],v[5],v[6],type,data);
        LOG_D("[webgl::exec] texSubImage2D_pixels(%s,%d,%d,%d,%d,%d,%s,%s,pixelSize=%d), error=%d",
              WebGL_CONST(v[0]), v[1], v[2], v[3], v[4], v[5], WebGL_CONST(v[6]),WebGL_CONST(type), size, getError());
    }
}
void GCommandDecoderWebGL::WebGL_uniform1f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();
    float *fv = buffer.parseValue<float>();
    if( fv == nullptr || iv == nullptr )
        return;
    uniform1f(*iv, *fv);
    LOG_D("[webgl::exec] uniform1f(%d,%f), error=%d", *iv, *fv, getError());

}
void GCommandDecoderWebGL::WebGL_uniform2f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();
    if( !iv ) return;

    float v[2];
    bool ret = buffer.parseArray<float>(v, 2);
    if( !ret ) return;

    uniform2f(*iv, v[0], v[1]);
    LOG_D("[webgl::exec] uniform2f(%d,%f,%f), error=%d", *iv, v[0], v[1], getError());
}

void GCommandDecoderWebGL::WebGL_uniform3f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();
    if( !iv ) return;

    float v[3];
    buffer.parseArray<float>(v, 3);
    uniform3f(*iv, v[0], v[1], v[2]);
    LOG_D("[webgl::exec] uniform3f(%d,%f,%f,%f), error=%d", *iv, v[0], v[1], v[2], getError());
}

void GCommandDecoderWebGL::WebGL_uniform4f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();
    if( !iv ) return;

    float v[4];
    buffer.parseArray<float>(v, 4);
    uniform4f(*iv, v[0], v[1], v[2], v[3]);
    LOG_D("[webgl::exec] uniform3f(%d,%f,%f,%f,%f), error=%d", *iv, v[0], v[1], v[2], v[3], getError());
}

WEBGL_FUNC_IMP_PARAM_2_INT(uniform1i)
WEBGL_FUNC_IMP_PARAM_3_INT(uniform2i)
WEBGL_FUNC_IMP_PARAM_4_INT(uniform3i)
WEBGL_FUNC_IMP_PARAM_5_INT(uniform4i)

void GCommandDecoderWebGL::WebGL_uniform1fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4; //4byte
    if( size > 0 )
    {
        float * data = (float *)buffer.parseBuffer(size);
        uniform1fv(v[0], v[1], data);
        LOG_D("[webgl::exec] uniform1fv(%d,%f), error=%d",
              v[0], *data, getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform2fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniform2fv(v[0], v[1]/2, data);
        LOG_D("[webgl::exec] uniform2fv(%d,%f,%f), error=%d",
              v[0], data[0], data[1], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform3fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniform3fv(v[0], v[1]/3, data);
        LOG_D("[webgl::exec] uniform3fv(%d,%f,%f,%f), error=%d",
              v[0], data[0], data[1], data[2], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform4fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniform4fv(v[0], v[1]/4, data);
        LOG_D("[webgl::exec] uniform4fv(%d,%f,%f,%f,%f), error=%d",
              v[0], data[0], data[1], data[2], data[3], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform1iv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        GLint * data = (GLint *)buffer.parseBuffer(size);
        uniform1iv(v[0], v[1], data);
        LOG_D("[webgl::exec] uniform1iv(%d,%d,%d), error=%d",
              v[0], v[1], data[0], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform2iv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        GLint * data = (GLint *)buffer.parseBuffer(size);
        uniform2iv(v[0], v[1]/2, data);
        LOG_D("[webgl::exec] uniform2iv(%d,%d,%d,%d), error=%d",
              v[0], v[1]/2, data[0], data[1], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform3iv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        GLint * data = (GLint *)buffer.parseBuffer(size);
        uniform3iv(v[0], v[1]/3, data);
        LOG_D("[webgl::exec] uniform3iv(%d,%d,%d,%d,%d), error=%d",
              v[0], v[1]/3, data[0], data[1], data[2], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniform4iv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if( size > 0 )
    {
        GLint * data = (GLint *)buffer.parseBuffer(size);
        uniform4iv(v[0], v[1]/4, data);
        LOG_D("[webgl::exec] uniform4iv(%d,%d,%d,%d,%d,%d), error=%d",
              v[0], v[1]/4, data[0], data[1], data[2], data[3], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniformMatrix2fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray(v, 3);
    if( !ret ) return;

    uint32_t size = v[2] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniformMatrix2fv(v[0], v[2]/4, v[1], data);

        LOG_D("[webgl::exec] uniformMatrix2fv(%d,%d,%d,%f,%f), error=%d",
              v[0], v[2]/4, v[1], data[0], data[1], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniformMatrix3fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray(v, 3);
    if( !ret ) return;

    uint32_t size = v[2] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniformMatrix3fv(v[0], v[2]/9, v[1], data);
        LOG_D("[webgl::exec] uniformMatrix3fv(%d,%d,%d,%f,%f,%f), error=%d",
              v[0], v[2]/9, v[1], data[0], data[1], data[2], getError());
    }
}

void GCommandDecoderWebGL::WebGL_uniformMatrix4fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[3];
    bool ret = buffer.parseArray(v, 3);
    if( !ret ) return;

    uint32_t size = v[2] * 4;//4byte;
    if( size > 0 )
    {
        float* data = (float*)buffer.parseBuffer(size);
        uniformMatrix4fv(v[0], v[2]/16, v[1], data);
        LOG_D("[webgl::exec] uniformMatrix4fv(%d,%d,%d,%f,%f,%f,%f), error=%d",
              v[0], v[2]/16, v[1], data[0], data[1], data[2], data[3], getError());
    }
}

WEBGL_FUNC_IMP_PARAM_1_INT(useProgram)
WEBGL_FUNC_IMP_PARAM_1_INT(validateProgram)

void GCommandDecoderWebGL::WebGL_vertexAttrib1f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();
    float *fv = buffer.parseValue<float>();
    if( iv == nullptr || fv == nullptr )
        return;
    vertexAttrib1f(*iv, *fv);
}
void GCommandDecoderWebGL::WebGL_vertexAttrib2f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();

    float fv[2];
    bool ret = buffer.parseArray<float>(fv, 2);

    if( iv == nullptr || !ret )
        return;

    vertexAttrib2f(*iv, fv[0], fv[1]);
}

void GCommandDecoderWebGL::WebGL_vertexAttrib3f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();

    float fv[3];
    bool ret = buffer.parseArray<float>(fv, 3);

    if( iv == nullptr || !ret )
        return;

    vertexAttrib3f(*iv, fv[0], fv[1], fv[2]);
}

void GCommandDecoderWebGL::WebGL_vertexAttrib4f(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t *iv = buffer.parseValue<uint32_t>();

    float fv[4];
    bool ret = buffer.parseArray<float>(fv, 4);

    if( iv == nullptr || !ret )
        return;

    vertexAttrib4f(*iv, fv[0], fv[1], fv[2], fv[3]);
}

void GCommandDecoderWebGL::WebGL_vertexAttrib1fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if ( size > 0 ) {
        const void* data = buffer.parseBuffer(size);
        vertexAttrib1fv(v[0], (const float*)data);
    }
}

void GCommandDecoderWebGL::WebGL_vertexAttrib2fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if ( size > 0 ) {
        const void* data = buffer.parseBuffer(size);
        vertexAttrib2fv(v[0], (const float*)data);
    }
}

void GCommandDecoderWebGL::WebGL_vertexAttrib3fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if ( size > 0 ) {
        const void* data = buffer.parseBuffer(size);
        vertexAttrib3fv(v[0], (const float*)data);
    }
}

void GCommandDecoderWebGL::WebGL_vertexAttrib4fv(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray<uint32_t>(v, 2);
    if( !ret ) return;

    uint32_t size = v[1] * 4;//4byte;
    if ( size > 0 ) {
        const void* data = buffer.parseBuffer(size);
        vertexAttrib4fv(v[0], (const float*)data);
    }
}

void GCommandDecoderWebGL::WebGL_vertexAttribPointer(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[6];
    bool ret = buffer.parseArray<uint32_t>(v, 6);
    if( !ret ) return;

    vertexAttribPointer(v[0], v[1], v[2], v[3], v[4], (char*)NULL + v[5]);
    LOG_D("[webgl::exec] vertexAttribPointer(%d,%d,%d,%d,%d), error=%d",
    v[0], v[1], v[2], v[3], v[4], getError());
}

WEBGL_FUNC_IMP_PARAM_4_INT(viewport)


/////////////////////////////////////////
///      WebGL Extnsions API
/////////////////////////////////////////
void GCommandDecoderWebGL::WebGL_drawArraysInstancedANGLE(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[4];
    bool ret = buffer.parseArray(v, 4);
    if( !ret ) return;

    GLenum mode = v[0];
    GLint first = v[1];
    GLsizei count = v[2];
    GLsizei instanceCount = v[3];
    WebGL::drawArraysInstancedEXT(mode,first,count,instanceCount);
    LOG_D("[webgl::exec] drawArraysInstancedANGLE(%d,%d,%d,%d), error=%d",
          v[0],v[1],v[2],v[3], getError());
}

void GCommandDecoderWebGL::WebGL_drawElementsInstancedANGLE(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[5];
    bool ret = buffer.parseArray(v, 5);
    if( !ret ) return;

    GLenum mode= v[0];
    GLsizei count=v[1];
    GLenum type=v[2];
    GLint indices=v[3];
    GLsizei instanceCount=v[4];
    WebGL::drawElementsInstancedEXT(mode,count,type,indices,instanceCount);
    LOG_D("[webgl::exec] drawElementsInstancedANGLE(%d,%d,%d,%d,%d), error=%d",
          v[0],v[1],v[2],v[3],v[4], getError());
}
void GCommandDecoderWebGL::WebGL_vertexAttribDivisorANGLE(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[2];
    bool ret = buffer.parseArray(v, 2);
    if( !ret ) return;

    GLuint index=v[0];
    GLuint divisor=v[1];
    WebGL::vertexAttribDivisorEXT(index,divisor);
    LOG_D("[webgl::exec] vertexAttribDivisorANGLE(%d,%d), error=%d",
          v[0],v[1], getError());
}

void GCommandDecoderWebGL::WebGL_createVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res)
{
    GLuint v = WebGL::createVertexArrayOES();
    res.hasResult = true;
    res.type = GResType_UInt32_CreateVertexArray;
    res.uintValue= v;
    LOG_D("[webgl::exec] createVertexArrayOES()=%d, error=%d",
          v, getError());
}

void GCommandDecoderWebGL::WebGL_deleteVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[1];
    buffer.parseArray(v, 1);
    deleteVertexArraysOES(1,&v[0]);
    mRenderContext->DeleteGLResource(WebGL::GLResourceType::VertexArray, v[0]);
    LOG_D("[webgl::exec] deleteVertexArrayOES(%d), error=%d",
          v[0], getError());
}

void GCommandDecoderWebGL::WebGL_isVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t v[1];
    buffer.parseArray(v, 1);
    res.hasResult=true;
    res.type=GResType_Bool;
    res.boolValue= isVertexArrayOES(v[0]);
    LOG_D("[webgl::exec] isVertexArrayOES(%d)=%d, error=%d",
          v[0], res.boolValue, getError());
}

WEBGL_FUNC_IMP_PARAM_1_INT(bindVertexArrayOES)


}
 
