/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GCOMMANDDECODERWEBGL_H
#define GCANVAS_GCOMMANDDECODERWEBGL_H

#include "GCommandDecoder.hpp"
#include "GWebGLRenderContext.hpp"
#include <string>


namespace gcanvas
{

class GCommandDecoderWebGL : public GCommandDecoder
{
    enum GWEBGL_FUNC_ID
    {
        GWEBGL_FUNC_activeTexture = 1,
        GWEBGL_FUNC_attachShader,
        GWEBGL_FUNC_bindAttribLocation,
        GWEBGL_FUNC_bindBuffer,
        GWEBGL_FUNC_bindFramebuffer,
        GWEBGL_FUNC_bindRenderbuffer,
        GWEBGL_FUNC_bindTexture,
        GWEBGL_FUNC_blendColor,
        GWEBGL_FUNC_blendEquation,
        GWEBGL_FUNC_blendEquationSeparate,
        GWEBGL_FUNC_blendFunc,
        GWEBGL_FUNC_blendFuncSeparate,
        GWEBGL_FUNC_bufferData_size,
        GWEBGL_FUNC_bufferData_buffer,
        GWEBGL_FUNC_bufferSubData,
        GWEBGL_FUNC_checkFramebufferStatus,
        GWEBGL_FUNC_clear,
        GWEBGL_FUNC_clearColor,
        GWEBGL_FUNC_clearDepth,
        GWEBGL_FUNC_clearStencil,
        GWEBGL_FUNC_colorMask,
        GWEBGL_FUNC_commit,
        GWEBGL_FUNC_compileShader,
        GWEBGL_FUNC_compressedTexImage2D,
        GWEBGL_FUNC_compressedTexSubImage2D,
        GWEBGL_FUNC_copyTexImage2D,
        GWEBGL_FUNC_copyTexSubImage2D,
        GWEBGL_FUNC_createBuffer,
        GWEBGL_FUNC_createFramebuffer,
        GWEBGL_FUNC_createProgram,
        GWEBGL_FUNC_createRenderbuffer,
        GWEBGL_FUNC_createShader,
        GWEBGL_FUNC_createTexture,
        GWEBGL_FUNC_cullFace,
        GWEBGL_FUNC_deleteBuffer,
        GWEBGL_FUNC_deleteFramebuffer,
        GWEBGL_FUNC_deleteProgram,
        GWEBGL_FUNC_deleteRenderbuffer,
        GWEBGL_FUNC_deleteShader,
        GWEBGL_FUNC_deleteTexture,
        GWEBGL_FUNC_depthFunc,
        GWEBGL_FUNC_depthMask,
        GWEBGL_FUNC_depthRange,
        GWEBGL_FUNC_detachShader,
        GWEBGL_FUNC_disable,
        GWEBGL_FUNC_disableVertexAttribArray,
        GWEBGL_FUNC_drawArrays,
        GWEBGL_FUNC_drawElements,
        GWEBGL_FUNC_enable,
        GWEBGL_FUNC_enableVertexAttribArray,
        GWEBGL_FUNC_finish,
        GWEBGL_FUNC_flush,
        GWEBGL_FUNC_framebufferRenderbuffer,
        GWEBGL_FUNC_framebufferTexture2D,
        GWEBGL_FUNC_frontFace,
        GWEBGL_FUNC_generateMipmap,
        GWEBGL_FUNC_getActiveAttrib,
        GWEBGL_FUNC_getActiveUniform,
        GWEBGL_FUNC_getAttachedShaders,
        GWEBGL_FUNC_getAttribLocation,
        GWEBGL_FUNC_getBufferParameter,
        GWEBGL_FUNC_getContextAttributes,
        GWEBGL_FUNC_getError,
        GWEBGL_FUNC_getExtension,
        GWEBGL_FUNC_getFramebufferAttachmentParameter,
        GWEBGL_FUNC_getParameter,
        GWEBGL_FUNC_getProgramInfoLog,
        GWEBGL_FUNC_getProgramParameter,
        GWEBGL_FUNC_getRenderbufferParameter,
        GWEBGL_FUNC_getShaderInfoLog,
        GWEBGL_FUNC_getShaderParameter,
        GWEBGL_FUNC_getShaderPrecisionFormat,
        GWEBGL_FUNC_getShaderSource,
        GWEBGL_FUNC_getSupportedExtensions,
        GWEBGL_FUNC_getTexParameter,
        GWEBGL_FUNC_getUniform,
        GWEBGL_FUNC_getUniformLocation,
        GWEBGL_FUNC_getVertexAttrib,
        GWEBGL_FUNC_getVertexAttribOffset,
        GWEBGL_FUNC_hint,
        GWEBGL_FUNC_isBuffer,
        GWEBGL_FUNC_isContextLost,
        GWEBGL_FUNC_isEnabled,
        GWEBGL_FUNC_isFramebuffer,
        GWEBGL_FUNC_isProgram,
        GWEBGL_FUNC_isRenderbuffer,
        GWEBGL_FUNC_isShader,
        GWEBGL_FUNC_isTexture,
        GWEBGL_FUNC_lineWidth,
        GWEBGL_FUNC_linkProgram,
        GWEBGL_FUNC_pixelStorei,
        GWEBGL_FUNC_polygonOffset,
        GWEBGL_FUNC_readPixels,
        GWEBGL_FUNC_renderbufferStorage,
        GWEBGL_FUNC_sampleCoverage,
        GWEBGL_FUNC_scissor,
        GWEBGL_FUNC_shaderSource,
        GWEBGL_FUNC_stencilFunc,
        GWEBGL_FUNC_stencilFuncSeparate,
        GWEBGL_FUNC_stencilMask,
        GWEBGL_FUNC_stencilMaskSeparate,
        GWEBGL_FUNC_stencilOp,
        GWEBGL_FUNC_stencilOpSeparate,
        GWEBGL_FUNC_texImage2D,
        GWEBGL_FUNC_texImage2D_null,
        GWEBGL_FUNC_texImage2D_pixels,
        GWEBGL_FUNC_texParameterf,
        GWEBGL_FUNC_texParameteri,
        GWEBGL_FUNC_texSubImage2D,
        GWEBGL_FUNC_texSubImage2D_null,
        GWEBGL_FUNC_texSubImage2D_pixels,
        GWEBGL_FUNC_uniform1f,
        GWEBGL_FUNC_uniform2f,
        GWEBGL_FUNC_uniform3f,
        GWEBGL_FUNC_uniform4f,
        GWEBGL_FUNC_uniform1i,
        GWEBGL_FUNC_uniform2i,
        GWEBGL_FUNC_uniform3i,
        GWEBGL_FUNC_uniform4i,
        GWEBGL_FUNC_uniform1fv,
        GWEBGL_FUNC_uniform2fv,
        GWEBGL_FUNC_uniform3fv,
        GWEBGL_FUNC_uniform4fv,
        GWEBGL_FUNC_uniform1iv,
        GWEBGL_FUNC_uniform2iv,
        GWEBGL_FUNC_uniform3iv,
        GWEBGL_FUNC_uniform4iv,
        GWEBGL_FUNC_uniformMatrix2fv,
        GWEBGL_FUNC_uniformMatrix3fv,
        GWEBGL_FUNC_uniformMatrix4fv,
        GWEBGL_FUNC_useProgram,
        GWEBGL_FUNC_validateProgram,
        GWEBGL_FUNC_vertexAttrib1f,
        GWEBGL_FUNC_vertexAttrib2f,
        GWEBGL_FUNC_vertexAttrib3f,
        GWEBGL_FUNC_vertexAttrib4f,
        GWEBGL_FUNC_vertexAttrib1fv,
        GWEBGL_FUNC_vertexAttrib2fv,
        GWEBGL_FUNC_vertexAttrib3fv,
        GWEBGL_FUNC_vertexAttrib4fv,
        GWEBGL_FUNC_vertexAttribPointer,
        GWEBGL_FUNC_viewport,
        
        //extension
        GWEBGL_FUNC_drawArraysInstancedANGLE = 200,
        GWEBGL_FUNC_drawElementsInstancedANGLE,
        GWEBGL_FUNC_vertexAttribDivisorANGLE,
        GWEBGL_FUNC_createVertexArrayOES,
        GWEBGL_FUNC_deleteVertexArrayOES,
        GWEBGL_FUNC_isVertexArrayOES,
        GWEBGL_FUNC_bindVertexArrayOES,
        
        //API数量
        GWEBGL_FUNC_COUNT,
    };
    
    
public:
    API_EXPORT GCommandDecoderWebGL(WebGL::GWebGLRenderContext* renderContext);
    API_EXPORT ~GCommandDecoderWebGL();
        
    virtual void InitCommands();
    inline virtual bool IsSyncCommand(uint32_t cmdId){
        auto iter = mWebGLSyncFuncSet.find(cmdId);
        return iter != mWebGLSyncFuncSet.end();
    }
    API_EXPORT virtual bool Decode(GCommandBuffer& buffer, GDecodeRes& res);

    //WebGL1.0 API
    static void WebGL_activeTexture(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_attachShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindAttribLocation(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindBuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindFramebuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindTexture(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_blendColor(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_blendEquation(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_blendEquationSeparate(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_blendFunc(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_blendFuncSeparate(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bufferData_size(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bufferData_buffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bufferSubData(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_checkFramebufferStatus(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_clear(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_clearColor(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_clearDepth(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_clearStencil(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_colorMask(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_commit(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_compileShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_compressedTexImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_compressedTexSubImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_copyTexImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_copyTexSubImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createBuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createFramebuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createTexture(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_cullFace(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteBuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteFramebuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteTexture(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_depthFunc(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_depthMask(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_depthRange(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_detachShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_disable(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_disableVertexAttribArray(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_drawArrays(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_drawElements(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_enable(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_enableVertexAttribArray(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_finish(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_flush(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_framebufferRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_framebufferTexture2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_frontFace(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_generateMipmap(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getActiveAttrib(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getActiveUniform(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getAttachedShaders(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getAttribLocation(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getBufferParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getContextAttributes(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getError(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getExtension(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getFramebufferAttachmentParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getProgramInfoLog(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getProgramParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getRenderbufferParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getShaderInfoLog(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getShaderParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getShaderPrecisionFormat(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getShaderSource(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getSupportedExtensions(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getTexParameter(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getUniform(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getUniformLocation(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getVertexAttrib(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_getVertexAttribOffset(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_hint(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isBuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isContextLost(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isEnabled(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isFramebuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isRenderbuffer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isShader(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isTexture(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_lineWidth(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_linkProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_pixelStorei(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_polygonOffset(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_readPixels(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_renderbufferStorage(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_sampleCoverage(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_scissor(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_shaderSource(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilFunc(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilFuncSeparate(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilMask(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilMaskSeparate(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilOp(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_stencilOpSeparate(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texImage2D_null(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texImage2D_pixels(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texParameterf(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texParameteri(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texSubImage2D(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texSubImage2D_null(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_texSubImage2D_pixels(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform1f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform2f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform3f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform4f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform1i(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform2i(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform3i(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform4i(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform1fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform2fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform3fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform4fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform1iv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform2iv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform3iv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniform4iv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniformMatrix2fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniformMatrix3fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_uniformMatrix4fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_useProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_validateProgram(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib1f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib2f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib3f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib4f(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib1fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib2fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib3fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttrib4fv(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttribPointer(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_viewport(GCommandBuffer& buffer, GDecodeRes& res);
    
    
    //WebGL1.0 Extension
    static void WebGL_drawArraysInstancedANGLE(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_drawElementsInstancedANGLE(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_vertexAttribDivisorANGLE(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_createVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_deleteVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_isVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res);
    static void WebGL_bindVertexArrayOES(GCommandBuffer& buffer, GDecodeRes& res);
    
protected:
//    GCommandFunc mWebGLFuncs[GWEBGL_FUNC_COUNT];
//    GCommandFuncNew mWebGLFuncs[GWEBGL_FUNC_COUNT] = { nullptr };
    std::unordered_set<unsigned int> mWebGLSyncFuncSet;
//    WebGL::GWebGLRenderContext* mRenderContext;
};

} //namespace gcanvas

#endif /* GCANVAS_GCOMMANDDECODERWEBGL_H */
