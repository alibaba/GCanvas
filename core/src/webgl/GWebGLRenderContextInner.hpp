/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GWEBGLRENDERCONTEXTINNER_H
#define GCANVAS_GWEBGLRENDERCONTEXTINNER_H

#include "GGL.h"
#include <iostream>

namespace gcanvas {

namespace WebGL {
#define activeTexture                       glActiveTexture
#define attachShader                        glAttachShader
#define bindAttribLocation                  glBindAttribLocation
#define bindBuffer                          glBindBuffer
#define bindFramebuffer                     glBindFramebuffer
#define bindRenderbuffer                    glBindRenderbuffer
#define bindTexture                         glBindTexture
#define blendColor                          glBlendColor
#define blendEquation                       glBlendEquation
#define blendEquationSeparate               glBlendEquationSeparate
#define blendFunc                           glBlendFunc
#define blendFuncSeparate                   glBlendFuncSeparate
#define bufferData                          glBufferData
#define bufferSubData                       glBufferSubData
#define checkFramebufferStatus              glCheckFramebufferStatus
void clear(GLbitfield mask);
#define clearColor                          glClearColor
#define clearDepth                          glClearDepthf
#define clearStencil                        glClearStencil
#define colorMask                           glColorMask
#define commit                              glCommit
#define compileShader                       glCompileShader
#define compressedTexImage2D                glCompressedTexImage2D
#define compressedTexSubImage2D             glCompressedTexSubImage2D
#define copyTexImage2D                      glCopyTexImage2D
#define copyTexSubImage2D                   glCopyTexSubImage2D
GLuint createBuffer();
GLuint createFrameBuffer();
#define createProgram                       glCreateProgram
GLuint createRenderbuffer();
#define createShader                        glCreateShader
GLuint createTexture();
#define cullFace                            glCullFace
void deleteBuffer(GLuint buffer);
void deleteFramebuffer(GLuint framebuffer);
#define deleteProgram                       glDeleteProgram
void deleteRenderbuffer(GLuint renderbuffer);
#define deleteShader                        glDeleteShader
void deleteTexture(GLuint texture);
#define depthFunc                           glDepthFunc
#define depthMask                           glDepthMask
#define depthRange                          glDepthRangef
#define detachShader                        glDetachShader
#define disable                             glDisable
#define disableVertexAttribArray            glDisableVertexAttribArray
#define drawArrays                          glDrawArrays
#define drawElements                        glDrawElements
#define enable                              glEnable
#define enableVertexAttribArray             glEnableVertexAttribArray
#define finish                              glFinish
#define flush                               glFlush
#define framebufferRenderbuffer             glFramebufferRenderbuffer
#define framebufferTexture2D                glFramebufferTexture2D
#define frontFace                           glFrontFace
#define generateMipmap                      glGenerateMipmap
void getActiveAttrib(GLuint program, GLuint index, GLsizei *length, GLint *size, GLenum *type, char **name);
void getActiveUniform(GLuint program, GLuint index, GLsizei *length, GLint *size, GLenum *type, char **name);
void getAttachedShaders(GLuint program, GLsizei *count, GLuint **shaders);
#define getAttribLocation                   glGetAttribLocation
GLint getBufferParameter(GLenum target, GLenum pname);
#define getError                            glGetError
#define getFloatv                           glGetFloatv
//TODO
#define getExtension                        glGetExtension
GLint getFramebufferAttachmentParameter(GLenum target, GLenum attachment, GLenum pname);
#define getIntegerv                         glGetIntegerv
#define getProgramiv                        glGetProgramiv
std::string getProgramInfoLog(GLuint program);
GLint getProgramParameter(GLuint program, GLenum pname);
GLint getRenderbufferParameter(GLuint program, GLenum pname);
#define getShaderiv                         glGetShaderiv
std::string getShaderInfoLog(GLuint shader);
GLint getShaderParameter(GLuint shader, GLenum pname);
#define getShaderPrecisionFormat            glGetShaderPrecisionFormat
#define getShaderSource                     glGetShaderSource
#define getString                           glGetString
#define getTexParameterfv                   glGetTexParameterfv
#define getTexParameteriv                   glGetTexParameteriv
#define getUniformfv                        glGetUniformfv
#define getUniformiv                        glGetUniformiv
#define getUniformLocation                  glGetUniformLocation
#define getVertexAttribfv                   glGetVertexAttribfv
#define getVertexAttribiv                   glGetVertexAttribiv
#define getVertexAttribOffset               glGetVertexAttribOffset
#define getVertexAttribPointerv             glGetVertexAttribPointerv
#define hint                                glHint
#define isBuffer                            glIsBuffer
#define isEnabled                           glIsEnabled
#define isFramebuffer                       glIsFramebuffer
#define isProgram                           glIsProgram
#define isRenderbuffer                      glIsRenderbuffer
#define isShader                            glIsShader
#define isTexture                           glIsTexture
#define lineWidth                           glLineWidth
#define linkProgram                         glLinkProgram
#define pixelStorei                         glPixelStorei
#define polygonOffset                       glPolygonOffset
#define readPixels                          glReadPixels
#define renderbufferStorage                 glRenderbufferStorage
#define sampleCoverage                      glSampleCoverage
#define scissor                             glScissor
#define shaderSource                        glShaderSource
#define stencilFunc                         glStencilFunc
#define stencilFuncSeparate                 glStencilFuncSeparate
#define stencilMask                         glStencilMask
#define stencilMaskSeparate                 glStencilMaskSeparate
#define stencilOp                           glStencilOp
#define stencilOpSeparate                   glStencilOpSeparate
#define texImage2D                          glTexImage2D
#define texParameterf                       glTexParameterf
#define texParameterfv                      glTexParameterfv
#define texParameteri                       glTexParameteri
#define texParameteriv                      glTexParameteriv
#define texSubImage2D                       glTexSubImage2D
#define uniform1f                           glUniform1f
#define uniform1fv                          glUniform1fv
#define uniform1i                           glUniform1i
#define uniform1iv                          glUniform1iv
#define uniform2f                           glUniform2f
#define uniform2fv                          glUniform2fv
#define uniform2i                           glUniform2i
#define uniform2iv                          glUniform2iv
#define uniform3f                           glUniform3f
#define uniform3fv                          glUniform3fv
#define uniform3i                           glUniform3i
#define uniform3iv                          glUniform3iv
#define uniform4f                           glUniform4f
#define uniform4fv                          glUniform4fv
#define uniform4i                           glUniform4i
#define uniform4iv                          glUniform4iv
#define uniformMatrix2fv                    glUniformMatrix2fv
#define uniformMatrix3fv                    glUniformMatrix3fv
#define uniformMatrix4fv                    glUniformMatrix4fv
#define useProgram                          glUseProgram
#define validateProgram                     glValidateProgram
#define vertexAttrib1f                      glVertexAttrib1f
#define vertexAttrib1fv                     glVertexAttrib1fv
#define vertexAttrib2f                      glVertexAttrib2f
#define vertexAttrib2fv                     glVertexAttrib2fv
#define vertexAttrib3f                      glVertexAttrib3f
#define vertexAttrib3fv                     glVertexAttrib3fv
#define vertexAttrib4f                      glVertexAttrib4f
#define vertexAttrib4fv                     glVertexAttrib4fv
#define vertexAttribPointer                 glVertexAttribPointer
#define viewport                            glViewport
//extension
#define bindVertexArrayOES                  glBindVertexArrayOES
#define isVertexArrayOES                    glIsVertexArrayOES
#define deleteVertexArraysOES               glDeleteVertexArraysOES
GLuint createVertexArrayOES();
void drawArraysInstancedEXT(GLenum mode,GLint first,GLsizei count,GLsizei instanceCount);
void drawElementsInstancedEXT(GLenum mode,GLint  count,GLsizei type, GLint indices, GLsizei instanceCount);
void vertexAttribDivisorEXT(GLint index,GLint divisor);

#ifdef DEBUG

//webgl info
const char *webglConstStr(GLenum v);
const char *webglAPIName(uint32_t ID);

#define WebGL_CONST(v)  (WebGL::webglConstStr(v))
#define WEBGL_API(v)    (WebGL::webglAPIName(v))

#else

#define WebGL_CONST(v)
#define WEBGL_API(v)

#endif

} //namespace WebGL
} //namespace gcanvas
#endif //GCANVAS_GWEBGLRENDERCONTEXTINNER_H
