/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#include "GWebGLRenderContext.hpp"
#include "GWebGLRenderContextInner.hpp"
#include <algorithm>
#include "../support/Log.h"

namespace gcanvas {

    namespace WebGL {
#ifdef ANDROID
        PFNGLDRAWARRAYSINSTANCEDEXTPROC    drawArraysInstancedEXTFunc= nullptr;
        PFNGLDRAWELEMENTSINSTANCEDEXTPROC  drawElementsInstancedEXTFunc =nullptr;
        PFNGLVERTEXATTRIBDIVISOREXTPROC    vertexAttribDivisorEXTFunc =nullptr;
#endif
        void clear(GLbitfield mask) {
            glClear(mask);
        }

        GLuint createBuffer() {
            GLuint v = 0;
            glGenBuffers(1, &v);
            return v;
        }

        GLuint createFrameBuffer() {
            GLuint v = 0;
            glGenFramebuffers(1, &v);
            return v;
        }

        GLuint createRenderbuffer() {
            GLuint v = 0;
            glGenRenderbuffers(1, &v);
            return v;
        }

        GLuint createTexture() {
            GLuint v = 0;
            glGenTextures(1, &v);
            return v;
        }

        void deleteBuffer(GLuint buffer) {
            glDeleteBuffers(1, &buffer);
        }

        void deleteFramebuffer(GLuint framebuffer) {
            glDeleteFramebuffers(1, &framebuffer);
        }

        void deleteRenderbuffer(GLuint renderbuffer) {
            glDeleteRenderbuffers(1, &renderbuffer);
        }

        void deleteTexture(GLuint texture) {
            glDeleteTextures(1, &texture);
        }

        void
        getActiveAttrib(GLuint program, GLuint index, GLsizei *length, GLint *size, GLenum *type,
                        char **name) {
            GLint nameBufferSize = 0;
            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameBufferSize);

            *name = new GLchar[nameBufferSize];
            glGetActiveAttrib(program, index, nameBufferSize, length, size, type, *name);
        }

        void
        getActiveUniform(GLuint program, GLuint index, GLsizei *length, GLint *size, GLenum *type,
                         char **name) {
            GLint nameBufferSize = 0;
            glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameBufferSize);

            *name = new GLchar[nameBufferSize];
            glGetActiveUniform(program, index, nameBufferSize, length, size, type, *name);
        }

        void getAttachedShaders(GLuint program, GLsizei *count, GLuint **shaders) {
            GLsizei attachedCount = 0;
            glGetProgramiv(program, GL_ATTACHED_SHADERS, &attachedCount);
            if (attachedCount > 0) {
                *shaders = new GLuint[attachedCount];
                glGetAttachedShaders(program, attachedCount, count, *shaders);
            }
        }

        GLint getBufferParameter(GLenum target, GLenum pname) {
            GLint params = 0;
            glGetBufferParameteriv(target, pname, &params);
            return params;
        }

        GLint getFramebufferAttachmentParameter(GLenum target, GLenum attachment, GLenum pname) {
            GLint params = 0;
            glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
            return params;
        }

        std::string getProgramInfoLog(GLuint program) {
            GLint size;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
            std::string programInfoLog(size, 0);
            glGetProgramInfoLog(program, size, &size, (char*)(programInfoLog.c_str()));
            return programInfoLog;
        }

        GLint getProgramParameter(GLuint program, GLenum pname) {
            GLint params = 0;
            glGetProgramiv(program, pname, &params);
            return params;
        }

        GLint getRenderbufferParameter(GLuint program, GLenum pname) {
            GLint params = 0;
            glGetRenderbufferParameteriv(program, pname, &params);
            return params;
        }

        std::string getShaderInfoLog(GLuint shader) {
            GLint size;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

            std::string shaderInfoLog(size, 0);
            glGetShaderInfoLog(shader, size, &size, (char*)(shaderInfoLog.c_str()));
            return shaderInfoLog;
        }

        GLint getShaderParameter(GLuint shader, GLenum pname) {
            GLint params = 0;
            glGetShaderiv(shader, pname, &params);
            return params;
        }
    
        GLuint createVertexArrayOES(){
            GLuint v = 0;
            glGenVertexArraysOES(1, &v);
            return v;
        }


        void drawArraysInstancedEXT(GLenum mode,GLint first,GLsizei count,GLsizei instanceCount){
#ifdef ANDROID
        if (drawArraysInstancedEXTFunc == nullptr)
            drawArraysInstancedEXTFunc = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)eglGetProcAddress("glDrawArraysInstancedEXT");
        if (drawArraysInstancedEXTFunc != nullptr) {
            drawArraysInstancedEXTFunc(mode, first, count, instanceCount);
        }
#else
        glDrawArraysInstancedEXT(mode, first, count, instanceCount);
#endif
        }

        void drawElementsInstancedEXT(GLenum mode,GLint  count,GLsizei type, GLint indices, GLsizei instanceCount){
#ifdef ANDROID
        if(drawElementsInstancedEXTFunc == nullptr)
            drawElementsInstancedEXTFunc =(PFNGLDRAWELEMENTSINSTANCEDEXTPROC)eglGetProcAddress("glDrawElementsInstancedEXT");
        if(drawElementsInstancedEXTFunc!= nullptr) {
            drawElementsInstancedEXTFunc(mode, count, type, (char *) NULL + indices, instanceCount);
        }
#else
        glDrawElementsInstancedEXT(mode, count, type, (char*)NULL+indices, instanceCount);
#endif
        }

        void vertexAttribDivisorEXT(GLint index,GLint divisor){
#ifdef ANDROID
        if (vertexAttribDivisorEXTFunc == nullptr)
            vertexAttribDivisorEXTFunc = (PFNGLVERTEXATTRIBDIVISOREXTPROC) eglGetProcAddress(
                    "glVertexAttribDivisorEXT");
        if (vertexAttribDivisorEXTFunc != nullptr){
            vertexAttribDivisorEXTFunc(index, divisor);
        }
#else
        glVertexAttribDivisorEXT(index, divisor);
#endif
        }


#ifdef DEBUG
    const char *webglConstStr(GLenum v)
    {
        switch (v) {
            case 0x0: return "GL_NONE or GL_ZERO or GL_POINTS";
            case 0x1: return "GL_ONE or GL_LINES";
            case 0x2: return "GL_LINE_LOOP";
            case 0x3: return "GL_LINE_STRIP";
            case 0x4: return "GL_TRIANGLES";
            case 0x5: return "GL_TRIANGLE_STRIP";
            case 0x6: return "GL_TRIANGLE_FAN";
            case 0x100: return "GL_DEPTH_BUFFER_BIT";
            case 0x200: return "GL_NEVER";
            case 0x201: return "GL_LESS";
            case 0x202: return "GL_EQUAL";
            case 0x203: return "GL_LEQUAL";
            case 0x204: return "GL_GREATER";
            case 0x205: return "GL_NOTEQUAL";
            case 0x206: return "GL_GEQUAL";
            case 0x207: return "GL_ALWAYS";
            case 0x300: return "GL_SRC_COLOR";
            case 0x301: return "GL_ONE_MINUS_SRC_COLOR";
            case 0x302: return "GL_SRC_ALPHA";
            case 0x303: return "GL_ONE_MINUS_SRC_ALPHA";
            case 0x304: return "GL_DST_ALPHA";
            case 0x305: return "GL_ONE_MINUS_DST_ALPHA";
            case 0x306: return "GL_DST_COLOR";
            case 0x307: return "GL_ONE_MINUS_DST_COLOR";
            case 0x308: return "GL_SRC_ALPHA_SATURATE";
            case 0x400: return "GL_STENCIL_BUFFER_BIT";
            case 0x404: return "GL_FRONT";
            case 0x405: return "GL_BACK";
            case 0x408: return "GL_FRONT_AND_BACK";
            case 0x500: return "GL_INVALID_ENUM";
            case 0x501: return "GL_INVALID_VALUE";
            case 0x502: return "GL_INVALID_OPERATION";
            case 0x505: return "GL_OUT_OF_MEMORY";
            case 0x506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
            case 0x900: return "GL_CW";
            case 0x901: return "GL_CCW";
            case 0xB21: return "GL_LINE_WIDTH";
            case 0xB44: return "GL_CULL_FACE";
            case 0xB45: return "GL_CULL_FACE_MODE";
            case 0xB46: return "GL_FRONT_FACE";
            case 0xB70: return "GL_DEPTH_RANGE";
            case 0xB71: return "GL_DEPTH_TEST";
            case 0xB72: return "GL_DEPTH_WRITEMASK";
            case 0xB73: return "GL_DEPTH_CLEAR_VALUE";
            case 0xB74: return "GL_DEPTH_FUNC";
            case 0xB90: return "GL_STENCIL_TEST";
            case 0xB91: return "GL_STENCIL_CLEAR_VALUE";
            case 0xB92: return "GL_STENCIL_FUNC";
            case 0xB93: return "GL_STENCIL_VALUE_MASK";
            case 0xB94: return "GL_STENCIL_FAIL";
            case 0xB95: return "GL_STENCIL_PASS_DEPTH_FAIL";
            case 0xB96: return "GL_STENCIL_PASS_DEPTH_PASS";
            case 0xB97: return "GL_STENCIL_REF";
            case 0xB98: return "GL_STENCIL_WRITEMASK";
            case 0xBA2: return "GL_VIEWPORT";
            case 0xBD0: return "GL_DITHER";
            case 0xBE2: return "GL_BLEND";
            case 0xC10: return "GL_SCISSOR_BOX";
            case 0xC11: return "GL_SCISSOR_TEST";
            case 0xC22: return "GL_COLOR_CLEAR_VALUE";
            case 0xC23: return "GL_COLOR_WRITEMASK";
            case 0xCF5: return "GL_UNPACK_ALIGNMENT";
            case 0xD05: return "GL_PACK_ALIGNMENT";
            case 0xD33: return "GL_MAX_TEXTURE_SIZE";
            case 0xD3A: return "GL_MAX_VIEWPORT_DIMS";
            case 0xD50: return "GL_SUBPIXEL_BITS";
            case 0xD52: return "GL_RED_BITS";
            case 0xD53: return "GL_GREEN_BITS";
            case 0xD54: return "GL_BLUE_BITS";
            case 0xD55: return "GL_ALPHA_BITS";
            case 0xD56: return "GL_DEPTH_BITS";
            case 0xD57: return "GL_STENCIL_BITS";
            case 0xDE1: return "GL_TEXTURE_2D";
            case 0x1100: return "GL_DONT_CARE";
            case 0x1101: return "GL_FASTEST";
            case 0x1102: return "GL_NICEST";
            case 0x1400: return "GL_BYTE";
            case 0x1401: return "GL_UNSIGNED_BYTE";
            case 0x1402: return "GL_SHORT";
            case 0x1403: return "GL_UNSIGNED_SHORT";
            case 0x1404: return "GL_INT";
            case 0x1405: return "GL_UNSIGNED_INT";
            case 0x1406: return "GL_FLOAT";
            case 0x150A: return "GL_INVERT";
            case 0x1702: return "GL_TEXTURE";
            case 0x1901: return "GL_STENCIL_INDEX";
            case 0x1902: return "GL_DEPTH_COMPONENT";
            case 0x1906: return "GL_ALPHA";
            case 0x1907: return "GL_RGB";
            case 0x1908: return "GL_RGBA";
            case 0x1909: return "GL_LUMINANCE";
            case 0x190A: return "GL_LUMINANCE_ALPHA";
            case 0x1E00: return "GL_KEEP";
            case 0x1E01: return "GL_REPLACE";
            case 0x1E02: return "GL_INCR";
            case 0x1E03: return "GL_DECR";
            case 0x1F00: return "GL_VENDOR";
            case 0x1F01: return "GL_RENDERER";
            case 0x1F02: return "GL_VERSION";
            case 0x2600: return "GL_NEAREST";
            case 0x2601: return "GL_LINEAR";
            case 0x2700: return "GL_NEAREST_MIPMAP_NEAREST";
            case 0x2701: return "GL_LINEAR_MIPMAP_NEAREST";
            case 0x2702: return "GL_NEAREST_MIPMAP_LINEAR";
            case 0x2703: return "GL_LINEAR_MIPMAP_LINEAR";
            case 0x2800: return "GL_TEXTURE_MAG_FILTER";
            case 0x2801: return "GL_TEXTURE_MIN_FILTER";
            case 0x2802: return "GL_TEXTURE_WRAP_S";
            case 0x2803: return "GL_TEXTURE_WRAP_T";
            case 0x2901: return "GL_REPEAT";
            case 0x2A00: return "GL_POLYGON_OFFSET_UNITS";
            case 0x4000: return "GL_COLOR_BUFFER_BIT";
            case 0x8001: return "GL_CONSTANT_COLOR";
            case 0x8002: return "GL_ONE_MINUS_CONSTANT_COLOR";
            case 0x8003: return "GL_CONSTANT_ALPHA";
            case 0x8004: return "GL_ONE_MINUS_CONSTANT_ALPHA";
            case 0x8005: return "GL_BLEND_COLOR";
            case 0x8006: return "GL_FUNC_ADD";
            case 0x8009: return "GL_BLEND_EQUATION_RGB";
            case 0x800A: return "GL_FUNC_SUBTRACT";
            case 0x800B: return "GL_FUNC_REVERSE_SUBTRACT";
            case 0x8033: return "GL_UNSIGNED_SHORT_4_4_4_4";
            case 0x8034: return "GL_UNSIGNED_SHORT_5_5_5_1";
            case 0x8037: return "GL_POLYGON_OFFSET_FILL";
            case 0x8038: return "GL_POLYGON_OFFSET_FACTOR";
            case 0x8056: return "GL_RGBA4";
            case 0x8057: return "GL_RGB5_A1";
            case 0x8069: return "GL_TEXTURE_BINDING_2D";
            case 0x809E: return "GL_SAMPLE_ALPHA_TO_COVERAGE";
            case 0x80A0: return "GL_SAMPLE_COVERAGE";
            case 0x80A8: return "GL_SAMPLE_BUFFERS";
            case 0x80A9: return "GL_SAMPLES";
            case 0x80AA: return "GL_SAMPLE_COVERAGE_VALUE";
            case 0x80AB: return "GL_SAMPLE_COVERAGE_INVERT";
            case 0x80C8: return "GL_BLEND_DST_RGB";
            case 0x80C9: return "GL_BLEND_SRC_RGB";
            case 0x80CA: return "GL_BLEND_DST_ALPHA";
            case 0x80CB: return "GL_BLEND_SRC_ALPHA";
            case 0x812F: return "GL_CLAMP_TO_EDGE";
            case 0x8192: return "GL_GENERATE_MIPMAP_HINT";
            case 0x81A5: return "GL_DEPTH_COMPONENT16";
            case 0x821A: return "GL_DEPTH_STENCIL_ATTACHMENT";
            case 0x8363: return "GL_UNSIGNED_SHORT_5_6_5";
            case 0x8370: return "GL_MIRRORED_REPEAT";
            case 0x846D: return "GL_ALIASED_POINT_SIZE_RANGE";
            case 0x846E: return "GL_ALIASED_LINE_WIDTH_RANGE";
            case 0x84C0: return "GL_TEXTURE0";
            case 0x84C1: return "GL_TEXTURE1";
            case 0x84C2: return "GL_TEXTURE2";
            case 0x84C3: return "GL_TEXTURE3";
            case 0x84C4: return "GL_TEXTURE4";
            case 0x84C5: return "GL_TEXTURE5";
            case 0x84C6: return "GL_TEXTURE6";
            case 0x84C7: return "GL_TEXTURE7";
            case 0x84C8: return "GL_TEXTURE8";
            case 0x84C9: return "GL_TEXTURE9";
            case 0x84CA: return "GL_TEXTURE10";
            case 0x84CB: return "GL_TEXTURE11";
            case 0x84CC: return "GL_TEXTURE12";
            case 0x84CD: return "GL_TEXTURE13";
            case 0x84CE: return "GL_TEXTURE14";
            case 0x84CF: return "GL_TEXTURE15";
            case 0x84D0: return "GL_TEXTURE16";
            case 0x84D1: return "GL_TEXTURE17";
            case 0x84D2: return "GL_TEXTURE18";
            case 0x84D3: return "GL_TEXTURE19";
            case 0x84D4: return "GL_TEXTURE20";
            case 0x84D5: return "GL_TEXTURE21";
            case 0x84D6: return "GL_TEXTURE22";
            case 0x84D7: return "GL_TEXTURE23";
            case 0x84D8: return "GL_TEXTURE24";
            case 0x84D9: return "GL_TEXTURE25";
            case 0x84DA: return "GL_TEXTURE26";
            case 0x84DB: return "GL_TEXTURE27";
            case 0x84DC: return "GL_TEXTURE28";
            case 0x84DD: return "GL_TEXTURE29";
            case 0x84DE: return "GL_TEXTURE30";
            case 0x84DF: return "GL_TEXTURE31";
            case 0x84E0: return "GL_ACTIVE_TEXTURE";
            case 0x84E8: return "GL_MAX_RENDERBUFFER_SIZE";
            case 0x84F9: return "GL_DEPTH_STENCIL";
            case 0x8507: return "GL_INCR_WRAP";
            case 0x8508: return "GL_DECR_WRAP";
            case 0x8513: return "GL_TEXTURE_CUBE_MAP";
            case 0x8514: return "GL_TEXTURE_BINDING_CUBE_MAP";
            case 0x8515: return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
            case 0x8516: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
            case 0x8517: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
            case 0x8518: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
            case 0x8519: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
            case 0x851A: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
            case 0x851C: return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
            case 0x8622: return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
            case 0x8623: return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
            case 0x8624: return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
            case 0x8625: return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
            case 0x8626: return "GL_CURRENT_VERTEX_ATTRIB";
            case 0x8645: return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
            case 0x86A2: return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
            case 0x86A3: return "GL_COMPRESSED_TEXTURE_FORMATS";
            case 0x8764: return "GL_BUFFER_SIZE";
            case 0x8765: return "GL_BUFFER_USAGE";
            case 0x8800: return "GL_STENCIL_BACK_FUNC";
            case 0x8801: return "GL_STENCIL_BACK_FAIL";
            case 0x8802: return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
            case 0x8803: return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
            case 0x883D: return "GL_BLEND_EQUATION_ALPHA";
            case 0x8869: return "GL_MAX_VERTEX_ATTRIBS";
            case 0x886A: return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
            case 0x8872: return "GL_MAX_TEXTURE_IMAGE_UNITS";
            case 0x8892: return "GL_ARRAY_BUFFER";
            case 0x8893: return "GL_ELEMENT_ARRAY_BUFFER";
            case 0x8894: return "GL_ARRAY_BUFFER_BINDING";
            case 0x8895: return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
            case 0x889F: return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
            case 0x88E0: return "GL_STREAM_DRAW";
            case 0x88E4: return "GL_STATIC_DRAW";
            case 0x88E8: return "GL_DYNAMIC_DRAW";
            case 0x8B30: return "GL_FRAGMENT_SHADER";
            case 0x8B31: return "GL_VERTEX_SHADER";
            case 0x8B4C: return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
            case 0x8B4D: return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
            case 0x8B4F: return "GL_SHADER_TYPE";
            case 0x8B50: return "GL_FLOAT_VEC2";
            case 0x8B51: return "GL_FLOAT_VEC3";
            case 0x8B52: return "GL_FLOAT_VEC4";
            case 0x8B53: return "GL_INT_VEC2";
            case 0x8B54: return "GL_INT_VEC3";
            case 0x8B55: return "GL_INT_VEC4";
            case 0x8B56: return "GL_BOOL";
            case 0x8B57: return "GL_BOOL_VEC2";
            case 0x8B58: return "GL_BOOL_VEC3";
            case 0x8B59: return "GL_BOOL_VEC4";
            case 0x8B5A: return "GL_FLOAT_MAT2";
            case 0x8B5B: return "GL_FLOAT_MAT3";
            case 0x8B5C: return "GL_FLOAT_MAT4";
            case 0x8B5E: return "GL_SAMPLER_2D";
            case 0x8B60: return "GL_SAMPLER_CUBE";
            case 0x8B80: return "GL_DELETE_STATUS";
            case 0x8B81: return "GL_COMPILE_STATUS";
            case 0x8B82: return "GL_LINK_STATUS";
            case 0x8B83: return "GL_VALIDATE_STATUS";
            case 0x8B84: return "GL_INFO_LOG_LENGTH";
            case 0x8B85: return "GL_ATTACHED_SHADERS";
            case 0x8B86: return "GL_ACTIVE_UNIFORMS";
            case 0x8B87: return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
            case 0x8B88: return "GL_SHADER_SOURCE_LENGTH";
            case 0x8B89: return "GL_ACTIVE_ATTRIBUTES";
            case 0x8B8A: return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
            case 0x8B8C: return "GL_SHADING_LANGUAGE_VERSION";
            case 0x8B8D: return "GL_CURRENT_PROGRAM";
            case 0x8CA3: return "GL_STENCIL_BACK_REF";
            case 0x8CA4: return "GL_STENCIL_BACK_VALUE_MASK";
            case 0x8CA5: return "GL_STENCIL_BACK_WRITEMASK";
            case 0x8CA6: return "GL_FRAMEBUFFER_BINDING";
            case 0x8CA7: return "GL_RENDERBUFFER_BINDING";
            case 0x8CD0: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE";
            case 0x8CD1: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
            case 0x8CD2: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
            case 0x8CD3: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
            case 0x8CD5: return "GL_FRAMEBUFFER_COMPLETE";
            case 0x8CD6: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            case 0x8CD7: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            case 0x8CD9: return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
            case 0x8CDD: return "GL_FRAMEBUFFER_UNSUPPORTED";
            case 0x8CE0: return "GL_COLOR_ATTACHMENT0";
            case 0x8D00: return "GL_DEPTH_ATTACHMENT";
            case 0x8D20: return "GL_STENCIL_ATTACHMENT";
            case 0x8D40: return "GL_FRAMEBUFFER";
            case 0x8D41: return "GL_RENDERBUFFER";
            case 0x8D42: return "GL_RENDERBUFFER_WIDTH";
            case 0x8D43: return "GL_RENDERBUFFER_HEIGHT";
            case 0x8D44: return "GL_RENDERBUFFER_INTERNAL_FORMAT";
            case 0x8D48: return "GL_STENCIL_INDEX8";
            case 0x8D50: return "GL_RENDERBUFFER_RED_SIZE";
            case 0x8D51: return "GL_RENDERBUFFER_GREEN_SIZE";
            case 0x8D52: return "GL_RENDERBUFFER_BLUE_SIZE";
            case 0x8D53: return "GL_RENDERBUFFER_ALPHA_SIZE";
            case 0x8D54: return "GL_RENDERBUFFER_DEPTH_SIZE";
            case 0x8D55: return "GL_RENDERBUFFER_STENCIL_SIZE";
            case 0x8D62: return "GL_RGB565";
            case 0x8DF0: return "GL_LOW_FLOAT";
            case 0x8DF1: return "GL_MEDIUM_FLOAT";
            case 0x8DF2: return "GL_HIGH_FLOAT";
            case 0x8DF3: return "GL_LOW_INT";
            case 0x8DF4: return "GL_MEDIUM_INT";
            case 0x8DF5: return "GL_HIGH_INT";
            case 0x8DFA: return "GL_SHADER_COMPILER";
            case 0x8DFB: return "GL_MAX_VERTEX_UNIFORM_VECTORS";
            case 0x8DFC: return "GL_MAX_VARYING_VECTORS";
            case 0x8DFD: return "GL_MAX_FRAGMENT_UNIFORM_VECTORS";
            case 0x9240: return "GL_UNPACK_FLIP_Y_WEBGL";
            case 0x9241: return "GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL";
            case 0x9242: return "GL_CONTEXT_LOST_WEBGL";
            case 0x9243: return "GL_UNPACK_COLORSPACE_CONVERSION_WEBGL";
            case 0x9244: return "GL_BROWSER_DEFAULT_WEBGL";
            default: return "Unkonw Macro";
        }
    }

    const char *webglAPIName(uint32_t ID)
    {
        switch (ID)
        {
            case 1: return "activeTexture";
            case 2: return "attachShader";
            case 3: return "bindAttribLocation";
            case 4: return "bindBuffer";
            case 5: return "bindFramebuffer";
            case 6: return "bindRenderbuffer";
            case 7: return "bindTexture";
            case 8: return "blendColor";
            case 9: return "blendEquation";
            case 10: return "blendEquationSeparate";
            case 11: return "blendFunc";
            case 12: return "blendFuncSeparate";
            case 13: return "bufferData_size";
            case 14: return "bufferData_buffer";
            case 15: return "bufferSubData";
            case 16: return "checkFramebufferStatus";
            case 17: return "clear";
            case 18: return "clearColor";
            case 19: return "clearDepth";
            case 20: return "clearStencil";
            case 21: return "colorMask";
            case 22: return "commit";
            case 23: return "compileShader";
            case 24: return "compressedTexImage2D";
            case 25: return "compressedTexSubImage2D";
            case 26: return "copyTexImage2D";
            case 27: return "copyTexSubImage2D";
            case 28: return "createBuffer";
            case 29: return "createFramebuffer";
            case 30: return "createProgram";
            case 31: return "createRenderbuffer";
            case 32: return "createShader";
            case 33: return "createTexture";
            case 34: return "cullFace";
            case 35: return "deleteBuffer";
            case 36: return "deleteFramebuffer";
            case 37: return "deleteProgram";
            case 38: return "deleteRenderbuffer";
            case 39: return "deleteShader";
            case 40: return "deleteTexture";
            case 41: return "depthFunc";
            case 42: return "depthMask";
            case 43: return "depthRange";
            case 44: return "detachShader";
            case 45: return "disable";
            case 46: return "disableVertexAttribArray";
            case 47: return "drawArrays";
            case 48: return "drawElements";
            case 49: return "enable";
            case 50: return "enableVertexAttribArray";
            case 51: return "finish";
            case 52: return "flush";
            case 53: return "framebufferRenderbuffer";
            case 54: return "framebufferTexture2D";
            case 55: return "frontFace";
            case 56: return "generateMipmap";
            case 57: return "getActiveAttrib";
            case 58: return "getActiveUniform";
            case 59: return "getAttachedShaders";
            case 60: return "getAttribLocation";
            case 61: return "getBufferParameter";
            case 62: return "getContextAttributes";
            case 63: return "getError";
            case 64: return "getExtension";
            case 65: return "getFramebufferAttachmentParameter";
            case 66: return "getParameter";
            case 67: return "getProgramInfoLog";
            case 68: return "getProgramParameter";
            case 69: return "getRenderbufferParameter";
            case 70: return "getShaderInfoLog";
            case 71: return "getShaderParameter";
            case 72: return "getShaderPrecisionFormat";
            case 73: return "getShaderSource";
            case 74: return "getSupportedExtensions";
            case 75: return "getTexParameter";
            case 76: return "getUniform";
            case 77: return "getUniformLocation";
            case 78: return "getVertexAttrib";
            case 79: return "getVertexAttribOffset";
            case 80: return "hint";
            case 81: return "isBuffer";
            case 82: return "isContextLost";
            case 83: return "isEnabled";
            case 84: return "isFramebuffer";
            case 85: return "isProgram";
            case 86: return "isRenderbuffer";
            case 87: return "isShader";
            case 88: return "isTexture";
            case 89: return "lineWidth";
            case 90: return "linkProgram";
            case 91: return "pixelStorei";
            case 92: return "polygonOffset";
            case 93: return "readPixels";
            case 94: return "renderbufferStorage";
            case 95: return "sampleCoverage";
            case 96: return "scissor";
            case 97: return "shaderSource";
            case 98: return "stencilFunc";
            case 99: return "stencilFuncSeparate";
            case 100: return "stencilMask";
            case 101: return "stencilMaskSeparate";
            case 102: return "stencilOp";
            case 103: return "stencilOpSeparate";
            case 104: return "texImage2D";
            case 105: return "texImage2D_null";
            case 106: return "texImage2D_pixels";
            case 107: return "texParameterf";
            case 108: return "texParameteri";
            case 109: return "texSubImage2D";
            case 110: return "texSubImage2D_null";
            case 111: return "texSubImage2D_pixels";
            case 112: return "uniform1f";
            case 113: return "uniform2f";
            case 114: return "uniform3f";
            case 115: return "uniform4f";
            case 116: return "uniform1i";
            case 117: return "uniform2i";
            case 118: return "uniform3i";
            case 119: return "uniform4i";
            case 120: return "uniform1fv";
            case 121: return "uniform2fv";
            case 122: return "uniform3fv";
            case 123: return "uniform4fv";
            case 124: return "uniform1iv";
            case 125: return "uniform2iv";
            case 126: return "uniform3iv";
            case 127: return "uniform4iv";
            case 128: return "uniformMatrix2fv";
            case 129: return "uniformMatrix3fv";
            case 130: return "uniformMatrix4fv";
            case 131: return "useProgram";
            case 132: return "validateProgram";
            case 133: return "vertexAttrib1f";
            case 134: return "vertexAttrib2f";
            case 135: return "vertexAttrib3f";
            case 136: return "vertexAttrib4f";
            case 137: return "vertexAttrib1fv";
            case 138: return "vertexAttrib2fv";
            case 139: return "vertexAttrib3fv";
            case 140: return "vertexAttrib4fv";
            case 141: return "vertexAttribPointer";
            case 142: return "viewport";
                
            case 200: return "drawArraysInstancedANGLE";
            case 201: return "drawElementsInstancedANGLE";
            case 202: return "vertexAttribDivisorANGLE";
            case 203: return "createVertexArrayOES";
            case 204: return "deleteVertexArrayOES";
            case 205: return "isVertexArrayOES";
            case 206: return "bindVertexArrayOES";
            
            default:  return "Unkonw API ID";
        }
    }
#endif
        
    } //namespace WebGL
} //namespace gcanvas
