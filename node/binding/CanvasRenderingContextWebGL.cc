#include "CanvasRenderingContextWebGL.h"
#include "./webgl/WebGLShader.h"
#include "./webgl/WebGLProgram.h"
#include "./webgl/WebGLBuffer.h"
#include "./webgl/WebGLTexture.h"
#include "./webgl/WebGLFrameBuffer.h"
#include "./webgl/WebGLRenderBuffer.h"
#include "./webgl/WebGLActiveInfo.h"
#include "./webgl/WebGLUniformLocation.h"
#include "Image.h"
//测量单个函数耗时的debug开关
// #define DUMP_RUNNING_TIME 1

#ifdef DUMP_RUNNING_TIME
#define RECORD_TIME_BEGIN  \
    clock_t start, finish; \
    start = clock();

#define RECORD_TIME_END \
    finish = clock();   \
    printf("cost time %f ms\n", (double)(finish - start) * 1000.0f / CLOCKS_PER_SEC);
#else
#define RECORD_TIME_BEGIN
#define RECORD_TIME_END
#endif

#define DEFINE_VOID_METHOD(methodName)                       \
    \ 
     void                                                    \
    ContextWebGL::methodName(const Napi::CallbackInfo &info) \
    {                                                        \
        RECORD_TIME_BEGIN                                    \
        mRenderContext->makeCurrent();                       \
        // printf("the function :  " #methodName " is  called \n"); \
        // GLenum error = glGetError();                             \
        // printf("the error is %d \n", error);

#define DEFINE_RETURN_VALUE_METHOD(methodName)               \
    \ 
     Napi::Value                                             \
    ContextWebGL::methodName(const Napi::CallbackInfo &info) \
    {                                                        \
        RECORD_TIME_BEGIN                                    \
        mRenderContext->makeCurrent();                       \
        // printf("the function : " #methodName " is  called \n"); \
        // GLenum error = glGetError();                            \
        // printf("the error is %d \n", error);     \

namespace NodeBinding
{

    static float parseFloat(const Napi::Value& value)
    {
        float ret = 0.0f;
        if (value.IsNumber())
        {
            ret = value.As<Napi::Number>().FloatValue();
        }
        else if (value.IsBoolean())
        {
            ret = value.As<Napi::Boolean>().Value();
        }
        return ret;
    }

    static int parseInt(const Napi::Value &value)
    {
        int ret = 0;
        if (value.IsNumber())
        {
            ret = value.As<Napi::Number>().Int32Value();
        }
        else if (value.IsBoolean())
        {
            ret = value.As<Napi::Boolean>().Value();
        }
        return ret;
    }

    static GLuint parseUInt(const Napi::Value& value)
    {
        GLuint ret = 0;
        if (value.IsNumber())
        {
            ret = value.As<Napi::Number>().Uint32Value();
        }
        else if (value.IsBoolean())
        {
            ret = value.As<Napi::Boolean>().Value();
        }
        return ret;
    }
    Napi::FunctionReference ContextWebGL::constructor;
    void ContextWebGL::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);
        Napi::Function func =
            DefineClass(env,
                        "ContextWebGL",
                        {
                            BINDING_OBJECT_METHOD(createBuffer),
                            BINDING_OBJECT_METHOD(createProgram),
                            BINDING_OBJECT_METHOD(createTexture),
                            BINDING_OBJECT_METHOD(createShader),
                            BINDING_OBJECT_METHOD(createFrameBuffer),
                            BINDING_OBJECT_METHOD(createRenderBuffer),

                            BINDING_OBJECT_METHOD(deleteShader),
                            BINDING_OBJECT_METHOD(deleteProgram),
                            BINDING_OBJECT_METHOD(deleteFrameBuffer),
                            BINDING_OBJECT_METHOD(deleteRenderBuffer),
                            BINDING_OBJECT_METHOD(deleteTexture),
                            BINDING_OBJECT_METHOD(deleteBuffer),

                            BINDING_OBJECT_METHOD(isBuffer),
                            BINDING_OBJECT_METHOD(isFramebuffer),
                            BINDING_OBJECT_METHOD(isTexture),
                            BINDING_OBJECT_METHOD(isShader),
                            BINDING_OBJECT_METHOD(isProgram),
                            BINDING_OBJECT_METHOD(isRenderBuffer),

                            BINDING_OBJECT_METHOD(getShaderParameter),
                            BINDING_OBJECT_METHOD(getProgramParameter),
                            BINDING_OBJECT_METHOD(getBufferParameter),
                            BINDING_OBJECT_METHOD(getTexParameter),
                            BINDING_OBJECT_METHOD(getFramebufferAttachmentParameter),

                            BINDING_OBJECT_METHOD(viewport),
                            BINDING_OBJECT_METHOD(scissor),
                            BINDING_OBJECT_METHOD(clearColor),
                            BINDING_OBJECT_METHOD(colorMask),
                            BINDING_OBJECT_METHOD(clearDepth),
                            BINDING_OBJECT_METHOD(clearStencil),

                            BINDING_OBJECT_METHOD(bindBuffer),
                            BINDING_OBJECT_METHOD(bufferData),
                            BINDING_OBJECT_METHOD(bufferSubData),

                            BINDING_OBJECT_METHOD(bindTexture),
                            BINDING_OBJECT_METHOD(renderbufferStorage),

                            BINDING_OBJECT_METHOD(shaderSource),
                            BINDING_OBJECT_METHOD(getShaderSource),
                            BINDING_OBJECT_METHOD(compileShader),
                            BINDING_OBJECT_METHOD(attachShader),
                            BINDING_OBJECT_METHOD(detachShader),
                            BINDING_OBJECT_METHOD(getAttachedShaders),
                            BINDING_OBJECT_METHOD(linkProgram),
                            BINDING_OBJECT_METHOD(useProgram),
                            BINDING_OBJECT_METHOD(validateProgram),

                            BINDING_OBJECT_METHOD(vertexAttribPointer),
                            BINDING_OBJECT_METHOD(enableVertexAttribArray),
                            BINDING_OBJECT_METHOD(disableVertexAttribArray),

                            BINDING_OBJECT_METHOD(bindFramebuffer),
                            BINDING_OBJECT_METHOD(checkFramebufferStatus),
                            BINDING_OBJECT_METHOD(framebufferRenderbuffer),
                            BINDING_OBJECT_METHOD(framebufferTexture2D),

                            BINDING_OBJECT_METHOD(drawElements),
                            BINDING_OBJECT_METHOD(drawArrays),
                            BINDING_OBJECT_METHOD(flush),
                            BINDING_OBJECT_METHOD(finish),
                            BINDING_OBJECT_METHOD(clear),

                            BINDING_OBJECT_METHOD(getAttribLocation),
                            BINDING_OBJECT_METHOD(getShaderInfoLog),
                            BINDING_OBJECT_METHOD(bindAttribLocation),
                            BINDING_OBJECT_METHOD(getUniformLocation),
                            BINDING_OBJECT_METHOD(getActiveAttrib),
                            BINDING_OBJECT_METHOD(getActiveUniform),
                            BINDING_OBJECT_METHOD(getUniform),
                            BINDING_OBJECT_METHOD(getVertexAttrib),
                            BINDING_OBJECT_METHOD(getVertexAttribOffset),

                            BINDING_OBJECT_METHOD(uniform1f),
                            BINDING_OBJECT_METHOD(uniform2f),
                            BINDING_OBJECT_METHOD(uniform3f),
                            BINDING_OBJECT_METHOD(uniform4f),

                            BINDING_OBJECT_METHOD(uniform1i),
                            BINDING_OBJECT_METHOD(uniform2i),
                            BINDING_OBJECT_METHOD(uniform3i),
                            BINDING_OBJECT_METHOD(uniform4i),

                            BINDING_OBJECT_METHOD(uniform1fv),
                            BINDING_OBJECT_METHOD(uniform2fv),
                            BINDING_OBJECT_METHOD(uniform3fv),
                            BINDING_OBJECT_METHOD(uniform4fv),

                            BINDING_OBJECT_METHOD(uniform1iv),
                            BINDING_OBJECT_METHOD(uniform2iv),
                            BINDING_OBJECT_METHOD(uniform3iv),
                            BINDING_OBJECT_METHOD(uniform4iv),

                            BINDING_OBJECT_METHOD(uniformMatrix2fv),
                            BINDING_OBJECT_METHOD(uniformMatrix3fv),
                            BINDING_OBJECT_METHOD(uniformMatrix4fv),

                            BINDING_OBJECT_METHOD(vertexAttrib1f),
                            BINDING_OBJECT_METHOD(vertexAttrib2f),
                            BINDING_OBJECT_METHOD(vertexAttrib3f),
                            BINDING_OBJECT_METHOD(vertexAttrib4f),

                            BINDING_OBJECT_METHOD(vertexAttrib1fv),
                            BINDING_OBJECT_METHOD(vertexAttrib2fv),
                            BINDING_OBJECT_METHOD(vertexAttrib3fv),
                            BINDING_OBJECT_METHOD(vertexAttrib4fv),

                            BINDING_OBJECT_METHOD(activeTexture),
                            BINDING_OBJECT_METHOD(pixelStorei),
                            BINDING_OBJECT_METHOD(texParameteri),
                            BINDING_OBJECT_METHOD(texParameterf),
                            BINDING_OBJECT_METHOD(texImage2D),

                            BINDING_OBJECT_METHOD(depthFunc),
                            BINDING_OBJECT_METHOD(depthMask),
                            BINDING_OBJECT_METHOD(depthRange),

                            BINDING_OBJECT_METHOD(enable),
                            BINDING_OBJECT_METHOD(disable),
                            BINDING_OBJECT_METHOD(isEnabled),

                            BINDING_OBJECT_METHOD(stencilFunc),
                            BINDING_OBJECT_METHOD(stencilOp),
                            BINDING_OBJECT_METHOD(stencilMask),
                            BINDING_OBJECT_METHOD(stencilOpSeparate),
                            BINDING_OBJECT_METHOD(stencilMaskSeparate),
                            BINDING_OBJECT_METHOD(stencilFuncSeparate),

                            BINDING_OBJECT_METHOD(blendColor),
                            BINDING_OBJECT_METHOD(blendFunc),
                            BINDING_OBJECT_METHOD(blendFuncSeparate),
                            BINDING_OBJECT_METHOD(blendEquation),
                            BINDING_OBJECT_METHOD(blendEquationSeparate),
                            BINDING_OBJECT_METHOD(generateMipmap),
                            BINDING_OBJECT_METHOD(cullFace),
                            BINDING_OBJECT_METHOD(frontFace),
                            BINDING_OBJECT_METHOD(getError),

                            BINDING_OBJECT_METHOD(polygonOffset),
                            BINDING_OBJECT_METHOD(lineWidth),
                            BINDING_OBJECT_METHOD(sampleCoverage),
                            BINDING_OBJECT_METHOD(hint),
                            BINDING_OBJECT_METHOD(isContextLost),
                            BINDING_OBJECT_METHOD(readPixels),

                            BINDING_CONST_PROPERY(LINK_STATUS),
                            BINDING_CONST_PROPERY(COLOR_BUFFER_BIT),
                            BINDING_CONST_PROPERY(DEPTH_BUFFER_BIT),
                            BINDING_CONST_PROPERY(STENCIL_BUFFER_BIT),
                            BINDING_CONST_PROPERY(TRIANGLES),
                            BINDING_CONST_PROPERY(POINTS),
                            BINDING_CONST_PROPERY(LINE_STRIP),
                            BINDING_CONST_PROPERY(LINES),
                            BINDING_CONST_PROPERY(LINE_LOOP),
                            BINDING_CONST_PROPERY(TRIANGLE_FAN),
                            BINDING_CONST_PROPERY(TRIANGLE_STRIP),

                            BINDING_CONST_PROPERY(DEPTH_TEST),
                            BINDING_CONST_PROPERY(SCISSOR_TEST),
                            BINDING_CONST_PROPERY(STENCIL_TEST),

                            BINDING_CONST_PROPERY(COMPILE_STATUS),
                            BINDING_CONST_PROPERY(ARRAY_BUFFER),
                            BINDING_CONST_PROPERY(STATIC_DRAW),
                            BINDING_CONST_PROPERY(ELEMENT_ARRAY_BUFFER),
                            BINDING_CONST_PROPERY(VERTEX_SHADER),
                            BINDING_CONST_PROPERY(FRAGMENT_SHADER),

                            BINDING_CONST_PROPERY(RGBA),
                            BINDING_CONST_PROPERY(RGBA4),
                            BINDING_CONST_PROPERY(DEPTH_COMPONENT),
                            BINDING_CONST_PROPERY(ALPHA),
                            BINDING_CONST_PROPERY(RGB5_A1),
                            BINDING_CONST_PROPERY(RGB565),
                            BINDING_CONST_PROPERY(LUMINANCE),
                            BINDING_CONST_PROPERY(LUMINANCE_ALPHA),
                            BINDING_CONST_PROPERY(DEPTH_COMPONENT16),
                            BINDING_CONST_PROPERY(STENCIL_INDEX8),

                            BINDING_CONST_PROPERY(FLOAT),
                            BINDING_CONST_PROPERY(BYTE),
                            BINDING_CONST_PROPERY(UNSIGNED_BYTE),
                            BINDING_CONST_PROPERY(SHORT),
                            BINDING_CONST_PROPERY(UNSIGNED_SHORT),
                            BINDING_CONST_PROPERY(INT),
                            BINDING_CONST_PROPERY(UNSIGNED_INT),
                            BINDING_CONST_PROPERY(FLOAT),

                            BINDING_CONST_PROPERY(FALSE),
                            BINDING_CONST_PROPERY(TRUE),
                            BINDING_CONST_PROPERY(ZERO),
                            BINDING_CONST_PROPERY(ONE),

                            BINDING_CONST_PROPERY(CURRENT_VERTEX_ATTRIB),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_ENABLED),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_SIZE),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_STRIDE),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_TYPE),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_NORMALIZED),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_POINTER),
                            BINDING_CONST_PROPERY(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING),

                            BINDING_CONST_PROPERY(CULL_FACE),
                            BINDING_CONST_PROPERY(FRONT),
                            BINDING_CONST_PROPERY(BACK),
                            BINDING_CONST_PROPERY(FRONT_AND_BACK),

                            // Enabling and disabling
                            BINDING_CONST_PROPERY(BLEND),
                            BINDING_CONST_PROPERY(DITHER),
                            BINDING_CONST_PROPERY(POLYGON_OFFSET_FILL),
                            BINDING_CONST_PROPERY(SAMPLE_ALPHA_TO_COVERAGE),
                            BINDING_CONST_PROPERY(SAMPLE_COVERAGE),

                            // errors
                            BINDING_CONST_PROPERY(NO_ERROR),
                            BINDING_CONST_PROPERY(INVALID_ENUM),
                            BINDING_CONST_PROPERY(INVALID_VALUE),
                            BINDING_CONST_PROPERY(INVALID_OPERATION),
                            BINDING_CONST_PROPERY(OUT_OF_MEMORY),
                            // BINDING_CONST_PROPERY(CONTEXT_LOST_WEBGL),

                            // Front face directions
                            BINDING_CONST_PROPERY(CW),
                            BINDING_CONST_PROPERY(CCW),

                            BINDING_CONST_PROPERY(DONT_CARE),
                            BINDING_CONST_PROPERY(FASTEST),
                            BINDING_CONST_PROPERY(NICEST),
                            BINDING_CONST_PROPERY(GENERATE_MIPMAP_HINT),

                            // Pixel types
                            // BIND_CONSTANT(UNSIGNED_BYTE, 0x1401),
                            BINDING_CONST_PROPERY(UNSIGNED_SHORT_4_4_4_4),
                            BINDING_CONST_PROPERY(UNSIGNED_SHORT_5_5_5_1),
                            BINDING_CONST_PROPERY(UNSIGNED_SHORT_5_6_5),

                            BINDING_CONST_PROPERY(FLOAT_VEC2),
                            BINDING_CONST_PROPERY(FLOAT_VEC3),
                            BINDING_CONST_PROPERY(FLOAT_VEC4),
                            BINDING_CONST_PROPERY(INT_VEC2),
                            BINDING_CONST_PROPERY(INT_VEC3),
                            BINDING_CONST_PROPERY(INT_VEC4),
                            BINDING_CONST_PROPERY(BOOL),
                            BINDING_CONST_PROPERY(BOOL_VEC2),
                            BINDING_CONST_PROPERY(BOOL_VEC3),
                            BINDING_CONST_PROPERY(BOOL_VEC4),
                            BINDING_CONST_PROPERY(FLOAT_MAT2),
                            BINDING_CONST_PROPERY(FLOAT_MAT3),
                            BINDING_CONST_PROPERY(FLOAT_MAT4),
                            BINDING_CONST_PROPERY(SAMPLER_2D),
                            BINDING_CONST_PROPERY(SAMPLER_CUBE),

                            BINDING_CONST_PROPERY(LOW_FLOAT),
                            BINDING_CONST_PROPERY(MEDIUM_FLOAT),
                            BINDING_CONST_PROPERY(HIGH_FLOAT),
                            BINDING_CONST_PROPERY(LOW_INT),
                            BINDING_CONST_PROPERY(MEDIUM_INT),
                            BINDING_CONST_PROPERY(HIGH_INT),

                            BINDING_CONST_PROPERY(SRC_COLOR),
                            BINDING_CONST_PROPERY(ONE_MINUS_SRC_COLOR),
                            BINDING_CONST_PROPERY(SRC_ALPHA),
                            BINDING_CONST_PROPERY(ONE_MINUS_SRC_ALPHA),
                            BINDING_CONST_PROPERY(DST_ALPHA),
                            BINDING_CONST_PROPERY(ONE_MINUS_DST_ALPHA),
                            BINDING_CONST_PROPERY(DST_COLOR),
                            BINDING_CONST_PROPERY(ONE_MINUS_DST_COLOR),
                            BINDING_CONST_PROPERY(SRC_ALPHA_SATURATE),
                            BINDING_CONST_PROPERY(CONSTANT_COLOR),
                            BINDING_CONST_PROPERY(ONE_MINUS_CONSTANT_COLOR),
                            BINDING_CONST_PROPERY(CONSTANT_ALPHA),
                            BINDING_CONST_PROPERY(ONE_MINUS_CONSTANT_ALPHA),

                            BINDING_CONST_PROPERY(BLEND_EQUATION),
                            BINDING_CONST_PROPERY(BLEND_EQUATION_RGB),
                            BINDING_CONST_PROPERY(BLEND_EQUATION_ALPHA),
                            BINDING_CONST_PROPERY(BLEND_DST_RGB),
                            BINDING_CONST_PROPERY(BLEND_SRC_RGB),
                            BINDING_CONST_PROPERY(BLEND_DST_ALPHA),
                            BINDING_CONST_PROPERY(BLEND_SRC_ALPHA),
                            BINDING_CONST_PROPERY(BLEND_COLOR),
                            BINDING_CONST_PROPERY(ARRAY_BUFFER_BINDING),
                            BINDING_CONST_PROPERY(ELEMENT_ARRAY_BUFFER_BINDING),
                            BINDING_CONST_PROPERY(LINE_WIDTH),
                            BINDING_CONST_PROPERY(ALIASED_POINT_SIZE_RANGE),
                            BINDING_CONST_PROPERY(ALIASED_LINE_WIDTH_RANGE),
                            BINDING_CONST_PROPERY(CULL_FACE_MODE),
                            BINDING_CONST_PROPERY(FRONT_FACE),
                            BINDING_CONST_PROPERY(DEPTH_RANGE),
                            BINDING_CONST_PROPERY(DEPTH_WRITEMASK),
                            BINDING_CONST_PROPERY(DEPTH_CLEAR_VALUE),
                            BINDING_CONST_PROPERY(DEPTH_FUNC),
                            BINDING_CONST_PROPERY(STENCIL_CLEAR_VALUE),
                            BINDING_CONST_PROPERY(STENCIL_FUNC),
                            BINDING_CONST_PROPERY(STENCIL_FAIL),
                            BINDING_CONST_PROPERY(STENCIL_PASS_DEPTH_FAIL),
                            BINDING_CONST_PROPERY(STENCIL_PASS_DEPTH_PASS),
                            BINDING_CONST_PROPERY(STENCIL_REF),
                            BINDING_CONST_PROPERY(STENCIL_VALUE_MASK),
                            BINDING_CONST_PROPERY(STENCIL_WRITEMASK),
                            BINDING_CONST_PROPERY(STENCIL_BACK_FUNC),
                            BINDING_CONST_PROPERY(STENCIL_BACK_FAIL),
                            BINDING_CONST_PROPERY(STENCIL_BACK_PASS_DEPTH_FAIL),
                            BINDING_CONST_PROPERY(STENCIL_BACK_PASS_DEPTH_PASS),
                            BINDING_CONST_PROPERY(STENCIL_BACK_REF),
                            BINDING_CONST_PROPERY(STENCIL_BACK_VALUE_MASK),
                            BINDING_CONST_PROPERY(STENCIL_BACK_WRITEMASK),
                            BINDING_CONST_PROPERY(VIEWPORT),
                            BINDING_CONST_PROPERY(SCISSOR_BOX),
                            BINDING_CONST_PROPERY(COLOR_CLEAR_VALUE),
                            BINDING_CONST_PROPERY(COLOR_WRITEMASK),
                            BINDING_CONST_PROPERY(UNPACK_ALIGNMENT),
                            BINDING_CONST_PROPERY(PACK_ALIGNMENT),
                            BINDING_CONST_PROPERY(MAX_TEXTURE_SIZE),
                            BINDING_CONST_PROPERY(MAX_VIEWPORT_DIMS),
                            BINDING_CONST_PROPERY(SUBPIXEL_BITS),
                            BINDING_CONST_PROPERY(RED_BITS),
                            BINDING_CONST_PROPERY(GREEN_BITS),
                            BINDING_CONST_PROPERY(BLUE_BITS),
                            BINDING_CONST_PROPERY(ALPHA_BITS),
                            BINDING_CONST_PROPERY(DEPTH_BITS),
                            BINDING_CONST_PROPERY(STENCIL_BITS),
                            BINDING_CONST_PROPERY(POLYGON_OFFSET_UNITS),
                            BINDING_CONST_PROPERY(POLYGON_OFFSET_FACTOR),
                            BINDING_CONST_PROPERY(TEXTURE_BINDING_2D),
                            BINDING_CONST_PROPERY(SAMPLE_BUFFERS),
                            BINDING_CONST_PROPERY(SAMPLES),
                            BINDING_CONST_PROPERY(SAMPLE_COVERAGE_VALUE),
                            BINDING_CONST_PROPERY(SAMPLE_COVERAGE_INVERT),
                            BINDING_CONST_PROPERY(COMPRESSED_TEXTURE_FORMATS),
                            BINDING_CONST_PROPERY(VENDOR),
                            BINDING_CONST_PROPERY(RENDERER),
                            BINDING_CONST_PROPERY(VERSION),
                            BINDING_CONST_PROPERY(IMPLEMENTATION_COLOR_READ_TYPE),
                            BINDING_CONST_PROPERY(IMPLEMENTATION_COLOR_READ_FORMAT),

                            BINDING_CONST_PROPERY(NEVER),
                            BINDING_CONST_PROPERY(LESS),
                            BINDING_CONST_PROPERY(EQUAL),
                            BINDING_CONST_PROPERY(LEQUAL),
                            BINDING_CONST_PROPERY(GREATER),
                            BINDING_CONST_PROPERY(NOTEQUAL),
                            BINDING_CONST_PROPERY(GEQUAL),
                            BINDING_CONST_PROPERY(ALWAYS),

                            BINDING_CONST_PROPERY(KEEP),
                            BINDING_CONST_PROPERY(REPLACE),
                            BINDING_CONST_PROPERY(INCR),
                            BINDING_CONST_PROPERY(DECR),
                            BINDING_CONST_PROPERY(INVERT),
                            BINDING_CONST_PROPERY(INCR_WRAP),
                            BINDING_CONST_PROPERY(DECR_WRAP),

                            BINDING_CONST_PROPERY(NEAREST),
                            BINDING_CONST_PROPERY(LINEAR),
                            BINDING_CONST_PROPERY(NEAREST_MIPMAP_NEAREST),
                            BINDING_CONST_PROPERY(LINEAR_MIPMAP_NEAREST),
                            BINDING_CONST_PROPERY(NEAREST_MIPMAP_LINEAR),
                            BINDING_CONST_PROPERY(LINEAR_MIPMAP_LINEAR),
                            BINDING_CONST_PROPERY(TEXTURE_MAG_FILTER),
                            BINDING_CONST_PROPERY(TEXTURE_MIN_FILTER),
                            BINDING_CONST_PROPERY(TEXTURE_WRAP_S),
                            BINDING_CONST_PROPERY(TEXTURE_WRAP_T),
                            BINDING_CONST_PROPERY(TEXTURE_2D),
                            BINDING_CONST_PROPERY(TEXTURE),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP),
                            BINDING_CONST_PROPERY(TEXTURE_BINDING_CUBE_MAP),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_POSITIVE_X),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_NEGATIVE_X),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_POSITIVE_Y),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_NEGATIVE_Y),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_POSITIVE_Z),
                            BINDING_CONST_PROPERY(TEXTURE_CUBE_MAP_NEGATIVE_Z),
                            BINDING_CONST_PROPERY(MAX_CUBE_MAP_TEXTURE_SIZE),
                            BINDING_CONST_PROPERY(TEXTURE0),
                            BINDING_CONST_PROPERY(TEXTURE1),
                            BINDING_CONST_PROPERY(TEXTURE2),
                            BINDING_CONST_PROPERY(TEXTURE3),
                            BINDING_CONST_PROPERY(TEXTURE4),
                            BINDING_CONST_PROPERY(TEXTURE5),
                            BINDING_CONST_PROPERY(TEXTURE6),
                            BINDING_CONST_PROPERY(TEXTURE7),
                            BINDING_CONST_PROPERY(TEXTURE8),
                            BINDING_CONST_PROPERY(TEXTURE9),
                            BINDING_CONST_PROPERY(TEXTURE10),
                            BINDING_CONST_PROPERY(TEXTURE11),
                            BINDING_CONST_PROPERY(TEXTURE12),
                            BINDING_CONST_PROPERY(TEXTURE13),
                            BINDING_CONST_PROPERY(TEXTURE14),
                            BINDING_CONST_PROPERY(TEXTURE15),
                            BINDING_CONST_PROPERY(TEXTURE16),
                            BINDING_CONST_PROPERY(TEXTURE17),
                            BINDING_CONST_PROPERY(TEXTURE18),
                            BINDING_CONST_PROPERY(TEXTURE19),
                            BINDING_CONST_PROPERY(TEXTURE20),
                            BINDING_CONST_PROPERY(TEXTURE21),
                            BINDING_CONST_PROPERY(TEXTURE22),
                            BINDING_CONST_PROPERY(TEXTURE23),
                            BINDING_CONST_PROPERY(TEXTURE24),
                            BINDING_CONST_PROPERY(TEXTURE25),
                            BINDING_CONST_PROPERY(TEXTURE26),
                            BINDING_CONST_PROPERY(TEXTURE27),
                            BINDING_CONST_PROPERY(TEXTURE28),
                            BINDING_CONST_PROPERY(TEXTURE29),
                            BINDING_CONST_PROPERY(TEXTURE30),
                            BINDING_CONST_PROPERY(TEXTURE31),

                            BINDING_CONST_PROPERY(ACTIVE_TEXTURE),
                            BINDING_CONST_PROPERY(REPEAT),
                            BINDING_CONST_PROPERY(CLAMP_TO_EDGE),
                            BINDING_CONST_PROPERY(MIRRORED_REPEAT),

                            InstanceAccessor("drawBufferWidth", &ContextWebGL::getDrawingBufferWidth, nullptr),
                            InstanceAccessor("drawBufferHeight", &ContextWebGL::getDrawingBufferHeight, nullptr),
                            InstanceAccessor("UNPACK_FLIP_Y_WEBGL", &ContextWebGL::getUNPACK_FLIP_Y_WEBGL, nullptr),
                            InstanceAccessor("UNPACK_PREMULTIPLY_ALPHA_WEBGL", &ContextWebGL::getUNPACK_PREMULTIPLY_ALPHA_WEBGL, nullptr),
                            InstanceAccessor("UNPACK", &ContextWebGL::getUNPACK, nullptr),
                        });
        constructor = Napi::Persistent(func);
    }
    ContextWebGL::ContextWebGL(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ContextWebGL>(info)
    {
    }

    Napi::Object ContextWebGL::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "context3d"));
        GL_TEXTURE0;
        return obj;
    }

    DEFINE_VOID_METHOD(clear)
    CHECK_PARAM_LEGNTH(1)
    GLbitfield mask = info[0].As<Napi::Number>().Uint32Value();
    glClear(mask);
    RECORD_TIME_END
} // namespace NodeBinding
DEFINE_VOID_METHOD(clearColor)
CHECK_PARAM_LEGNTH(4)
GLfloat red = info[0].As<Napi::Number>().FloatValue();
GLfloat green = info[1].As<Napi::Number>().FloatValue();
GLfloat blue = info[2].As<Napi::Number>().FloatValue();
GLfloat alpha = info[3].As<Napi::Number>().FloatValue();
glClearColor(red, green, blue, alpha);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(enable)
CHECK_PARAM_LEGNTH(1)
GLenum cap = info[0].As<Napi::Number>().Uint32Value();
glEnable(cap);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createBuffer)
CHECK_PARAM_LEGNTH(0)
GLuint bufferId;
glGenBuffers(1, &bufferId);
Napi::Object obj = WebGLBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), bufferId));
// this->mRenderContext->getCtxWebGL()->AddGLResource(Buffer, bufferId);
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(bindBuffer)
NodeBinding::checkArgs(info, 2);
GLenum type = info[0].As<Napi::Number>().Uint32Value();
GLuint bufferId;
if (info[1].IsNull() || info[1].IsUndefined())
{
    bufferId = 0;
}
else
{
    WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[1].As<Napi::Object>());
    bufferId = buffer->getId();
}
// printf("bindBuffer  id is %d \n", bufferId);
glBindBuffer(type, bufferId);
RECORD_TIME_END
}

/**
 *WebGL1 支持多种方式
 * gl.bufferData(target, size, usage);
 * gl.bufferData(target, ArrayBuffer? srcData, usage);
 * gl.bufferData(target, ArrayBufferView srcData, usage);
 **/
DEFINE_VOID_METHOD(bufferData)
CHECK_PARAM_LEGNTH(3)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum usage = info[2].As<Napi::Number>().Uint32Value();
if (info[1].IsTypedArray())
{
    Napi::TypedArray array = info[1].As<Napi::TypedArray>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferData(target, buffer.ByteLength(), buffer.Data(), usage);
}
else if (info[1].IsArrayBuffer())
{
    Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferData(target, array.ByteLength(), buffer.Data(), usage);
}
else if (info[1].IsNumber())
{
    GLuint size = info[1].As<Napi::Number>().Int32Value();
    glBufferData(target, size, NULL, usage);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(bufferSubData)
CHECK_PARAM_LEGNTH(3)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum offset = info[1].As<Napi::Number>().Uint32Value();
if (info[2].IsTypedArray())
{
    Napi::TypedArray array = info[2].As<Napi::TypedArray>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferSubData(target, offset, buffer.ByteLength(), buffer.Data());
}
else if (info[2].IsArrayBuffer())
{
    Napi::ArrayBuffer array = info[2].As<Napi::ArrayBuffer>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferSubData(target, offset, buffer.ByteLength(), buffer.Data());
}
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createShader)
CHECK_PARAM_LEGNTH(1)
Napi::Env env = info.Env();
GLenum shaderType = info[0].As<Napi::Number>().Uint32Value();
GLuint shaderId = glCreateShader(shaderType);
Napi::Object obj = WebGLShader::NewInstance(info.Env(), Napi::Number::New(env, shaderId));
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(shaderSource)
CHECK_PARAM_LEGNTH(2)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
const char *shaderContent = shaderStr.c_str();
GLint shaderContentLen = shaderStr.size();
GLuint shaderId = shader->getId();
glShaderSource(shader->getId(), 1, &shaderContent, &shaderContentLen);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(compileShader)
CHECK_PARAM_LEGNTH(1)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
glCompileShader(shader->getId());
GLint compiled = 0;
RECORD_TIME_END
}

DEFINE_VOID_METHOD(attachShader)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
glAttachShader(program->getId(), shader->getId());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(detachShader)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
glDetachShader(program->getId(), shader->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createProgram)
NodeBinding::checkArgs(info, 0);
GLuint programId = glCreateProgram();
RECORD_TIME_END
return WebGLProgram::NewInstance(info.Env(), Napi::Number::New(info.Env(), programId));
}

DEFINE_VOID_METHOD(linkProgram)
CHECK_PARAM_LEGNTH(1)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glLinkProgram(program->getId());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(useProgram)
CHECK_PARAM_LEGNTH(1)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glUseProgram(program->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getAttribLocation)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
const char *name = info[1].As<Napi::String>().Utf8Value().data();
GLuint pos = glGetAttribLocation(program->getId(), name);
RECORD_TIME_END
return Napi::Number::New(info.Env(), pos);
}

DEFINE_VOID_METHOD(viewport)
CHECK_PARAM_LEGNTH(4)
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
int dpi = mRenderContext->getDpi();
glViewport(x *dpi, y *dpi, width *dpi, height *dpi);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(drawElements)
CHECK_PARAM_LEGNTH(4)
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
GLsizei count = info[1].As<Napi::Number>().Int32Value();
GLenum type = info[2].As<Napi::Number>().Uint32Value();
GLuint offset = info[3].As<Napi::Number>().Uint32Value();
glDrawElements(mode, count, type, (GLvoid *)(intptr_t)offset);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(flush)
CHECK_PARAM_LEGNTH(0)
glFlush();
RECORD_TIME_END
}

DEFINE_VOID_METHOD(finish)
CHECK_PARAM_LEGNTH(0)
glFinish();
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttribPointer)
CHECK_PARAM_LEGNTH(6)
GLuint index = info[0].As<Napi::Number>().Int32Value();
GLint size = info[1].As<Napi::Number>().Int32Value();
GLenum type = info[2].As<Napi::Number>().Uint32Value();
GLboolean isNormalized = info[3].As<Napi::Boolean>().Value();
GLuint stride = info[4].As<Napi::Number>().Int32Value();
GLuint offset = info[5].As<Napi::Number>().Int32Value();
// printf("vertexAttribPointer is %d ,%d ,%d ,%d ,%d ,%d  \n", index, size, type, isNormalized, stride, offset);

glVertexAttribPointer(index, size, type, isNormalized, stride, (GLvoid *)(intptr_t)offset);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(enableVertexAttribArray)
CHECK_PARAM_LEGNTH(1)
GLuint index = info[0].As<Napi::Number>().Uint32Value();
glEnableVertexAttribArray(index);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(scissor)
CHECK_PARAM_LEGNTH(4)
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
int dpi = mRenderContext->getDpi();
glScissor(x *dpi, y *dpi, width *dpi, height *dpi);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(drawArrays)
CHECK_PARAM_LEGNTH(3)
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
GLint first = info[1].As<Napi::Number>().Int32Value();
GLint count = info[2].As<Napi::Number>().Int32Value();
glDrawArrays(mode, first, count);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getShaderParameter)
CHECK_PARAM_LEGNTH(2)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLint ret = 0;
glGetShaderiv(shader->getId(), pname, &ret);
RECORD_TIME_END
return Napi::Number::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(getShaderInfoLog)
CHECK_PARAM_LEGNTH(1)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
GLsizei length;
glGetShaderiv(shader->getId(), GL_INFO_LOG_LENGTH, &length);
if (length <= 0)
{
    RECORD_TIME_END
    return Napi::String::New(info.Env(), "");
}
else
{
    GLchar *src = new GLchar[length];
    int real_size = 0;
    glGetShaderInfoLog(shader->getId(), length, &real_size, src);
    RECORD_TIME_END
    return Napi::String::New(info.Env(), src);
}
}

DEFINE_VOID_METHOD(deleteShader)
CHECK_PARAM_LEGNTH(1)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
glDeleteShader(shader->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getProgramParameter)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLint ret;
glGetProgramiv(program->getId(), pname, &ret);
RECORD_TIME_END
return Napi::Number::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(deleteProgram)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glDeleteProgram(program->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getUniformLocation)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
GLuint index = glGetUniformLocation(program->getId(), shaderStr.c_str());
Napi::Object obj = WebGLUniformLocation::NewInstance(info.Env(), index);
RECORD_TIME_END
return obj;
}
DEFINE_VOID_METHOD(uniform1f)
CHECK_PARAM_LEGNTH(2)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLfloat v1 = parseFloat(info[1]);
glUniform1f(location->getIndex(), v1);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2f)
CHECK_PARAM_LEGNTH(3)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLfloat v1 = parseFloat(info[1]);
GLfloat v2 = parseFloat(info[2]);
glUniform2f(location->getIndex(), v1, v2);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3f)
CHECK_PARAM_LEGNTH(4)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLfloat v1 = parseFloat(info[1]);
GLfloat v2 = parseFloat(info[2]);
GLfloat v3 = parseFloat(info[3]);
glUniform3f(location->getIndex(), v1, v2, v3);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4f)
CHECK_PARAM_LEGNTH(5)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLfloat v1 = parseFloat(info[1]);
GLfloat v2 = parseFloat(info[2]);
GLfloat v3 = parseFloat(info[3]);
GLfloat v4 = parseFloat(info[4]);
glUniform4f(location->getIndex(), v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(colorMask)
CHECK_PARAM_LEGNTH(4)
GLint v1, v2, v3, v4;
v1 = parseInt(info[0]);
v2 = parseInt(info[1]);
v3 = parseInt(info[2]);
v4 = parseInt(info[3]);
glColorMask(v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1i)
CHECK_PARAM_LEGNTH(2)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLint v1 = info[1].As<Napi::Number>().Int32Value();
glUniform1i(location->getIndex(), v1);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2i)
CHECK_PARAM_LEGNTH(3)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
glUniform2i(location->getIndex(), v1, v2);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3i)
CHECK_PARAM_LEGNTH(4)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
GLint v3 = info[3].As<Napi::Number>().Int32Value();
glUniform3i(location->getIndex(), v1, v2, v3);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4i)
CHECK_PARAM_LEGNTH(5)
WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
GLint v3 = info[3].As<Napi::Number>().Int32Value();
GLint v4 = info[4].As<Napi::Number>().Int32Value();
glUniform4i(location->getIndex(), v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1fv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallUniformFunc(info, glUniform1fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2fv)
CHECK_PARAM_LEGNTH(3)
parseTypeArrayAndCallUniformFunc(info, glUniform2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3fv)
CHECK_PARAM_LEGNTH(4)
parseTypeArrayAndCallUniformFunc(info, glUniform3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4fv)
CHECK_PARAM_LEGNTH(5)
parseTypeArrayAndCallUniformFunc(info, glUniform4fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1iv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallUniformFunc(info, glUniform1iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2iv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallUniformFunc(info, glUniform2iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3iv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallUniformFunc(info, glUniform3iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform4iv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallUniformFunc(info, glUniform4iv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix2fv)
CHECK_PARAM_LEGNTH(3)
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix3fv)
CHECK_PARAM_LEGNTH(3)
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix4fv)
CHECK_PARAM_LEGNTH(3)
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix4fv);
RECORD_TIME_END
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformFloatPtr func)
{
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength(), buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength(), buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformIntPtr func)
{
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Int32Array buffer = array.As<Napi::Int32Array>();
        func(location->getIndex(), buffer.ElementLength(), buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Int32Array buffer = array.As<Napi::Int32Array>();
        func(location->getIndex(), buffer.ElementLength(), buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformMatrixPtr func)
{
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLboolean transpose = info[1].As<Napi::Boolean>().Value();
    if (info[2].IsTypedArray())
    {
        Napi::TypedArray array = info[2].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength(), transpose, buffer.Data());
    }
    else if (info[2].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[2].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength(), transpose, buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallVertexFunc(const Napi::CallbackInfo &info, glVeterxFloatPtr func)
{
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location, buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location, buffer.Data());
    }
}

DEFINE_VOID_METHOD(vertexAttrib1f)
NodeBinding::checkArgs(info, 2);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
glVertexAttrib1f(location, v1);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib2f)
NodeBinding::checkArgs(info, 3);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
glVertexAttrib2f(location, v1, v2);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib3f)
NodeBinding::checkArgs(info, 4);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
glVertexAttrib3f(location, v1, v2, v3);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib4f)
NodeBinding::checkArgs(info, 5);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
GLfloat v4 = info[4].As<Napi::Number>().FloatValue();
glVertexAttrib4f(location, v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib1fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info, glVertexAttrib1fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib2fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info, glVertexAttrib2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib3fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info, glVertexAttrib3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib4fv)
CHECK_PARAM_LEGNTH(2)
parseTypeArrayAndCallVertexFunc(info, glVertexAttrib4fv);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createTexture)
CHECK_PARAM_LEGNTH(0)
GLuint textureId;
glGenTextures(1, &textureId);
Napi::Object obj = WebGLTexture::NewInstance(info.Env(), Napi::Number::New(info.Env(), textureId));
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(bindTexture)
CHECK_PARAM_LEGNTH(2)
GLenum type = info[0].As<Napi::Number>().Uint32Value();
GLuint textureId = 0;
if (info[1].IsNull() || info[0].IsUndefined())
{
}
else
{
    WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[1].As<Napi::Object>());
    textureId = texture->getId();
}

glBindTexture(type, textureId);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(pixelStorei)
CHECK_PARAM_LEGNTH(2)
GLenum type = info[0].As<Napi::Number>().Uint32Value();
GLuint param = parseUInt(info[1]);
glPixelStorei(type, param);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(texParameteri)
CHECK_PARAM_LEGNTH(3)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLenum param = info[2].As<Napi::Number>().Uint32Value();
glTexParameteri(target, pname, param);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(texParameterf)
CHECK_PARAM_LEGNTH(3)
GLfloat target = info[0].As<Napi::Number>().FloatValue();
GLfloat pname = info[1].As<Napi::Number>().FloatValue();
GLfloat param = info[2].As<Napi::Number>().FloatValue();
glTexParameterf(target, pname, param);
RECORD_TIME_END
}
// support:
//gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
DEFINE_VOID_METHOD(texImage2D)
if (info.Length() == 6)
{
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLuint level = info[1].As<Napi::Number>().Uint32Value();
    GLint internalFormat = info[2].As<Napi::Number>().Uint32Value();
    GLint format = info[3].As<Napi::Number>().Uint32Value();
    GLint type = info[4].As<Napi::Number>().Int32Value();
    GLint border = 0;
    if (info[5].IsNull() || info[5].IsUndefined())
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLint width = 0;
        GLint height = 0;
        glTexImage2D(target, level, internalFormat, width, height, border, format, type, nullptr);
    }
    else if (info[5].IsObject())
    {
        Napi::Object object = info[5].As<Napi::Object>();
        Napi::Value name = object.Get("name");
        printf("the name is %d\n", name.IsNull());
        std::string namePropetry = name.As<Napi::String>().Utf8Value();
        //todo canvas
        if (namePropetry == "image")
        {
            Image *image = Napi::ObjectWrap<Image>::Unwrap(info[5].As<Napi::Object>());

            //一次处理 1 个字节
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(target, level, internalFormat,
                         image->getWidth(),
                         image->getHeight(), border, format, type, &image->getPixels()[0]);
        }
    }
}
else
{
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilFunc)
CHECK_PARAM_LEGNTH(3)
GLenum func = info[0].As<Napi::Number>().Uint32Value();
GLint ref = info[1].As<Napi::Number>().Int32Value();
GLuint mask = info[2].As<Napi::Number>().Uint32Value();
glStencilFunc(func, ref, mask);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilOp)
CHECK_PARAM_LEGNTH(3)
GLenum fail = info[0].As<Napi::Number>().Uint32Value();
GLenum zfail = info[1].As<Napi::Number>().Uint32Value();
GLenum zpass = info[2].As<Napi::Number>().Uint32Value();
glStencilOp(fail, zfail, zpass);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(activeTexture)
CHECK_PARAM_LEGNTH(1)
GLenum texture = info[0].As<Napi::Number>().Uint32Value();
glActiveTexture(texture);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilMask)
CHECK_PARAM_LEGNTH(1)
GLint mask = info[0].As<Napi::Number>().Uint32Value();
glStencilMask(mask);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(frontFace)
CHECK_PARAM_LEGNTH(1)
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
glFrontFace(mode);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createFrameBuffer)
CHECK_PARAM_LEGNTH(0)
GLuint frameBufferId;
glGenFramebuffers(1, &frameBufferId);
Napi::Object obj = WebGLFrameBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), frameBufferId));
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(bindFramebuffer)
CHECK_PARAM_LEGNTH(2)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLuint frameBufferId = 0;
if (info[1].IsNull() || info[1].IsUndefined())
{
}
else
{
    WebGLFrameBuffer *buffer = Napi::ObjectWrap<WebGLFrameBuffer>::Unwrap(info[1].As<Napi::Object>());
    frameBufferId = buffer->getId();
}
glBindFramebuffer(target, frameBufferId);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getShaderSource)
CHECK_PARAM_LEGNTH(1)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
GLsizei length;
glGetShaderiv(shader->getId(), GL_SHADER_SOURCE_LENGTH, &length);
GLchar *src = new GLchar[length];
glGetShaderSource(shader->getId(), length, nullptr, src);
RECORD_TIME_END
return Napi::String::New(info.Env(), src);
}

DEFINE_RETURN_VALUE_METHOD(checkFramebufferStatus)
CHECK_PARAM_LEGNTH(1)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
RECORD_TIME_END
return Napi::Number::New(info.Env(), glCheckFramebufferStatus(target));
}

DEFINE_RETURN_VALUE_METHOD(isFramebuffer)
CHECK_PARAM_LEGNTH(1)
GLuint frameBufferId = 0;
if (info[0].IsNull() || info[0].IsUndefined())
{
}
else
{
    WebGLFrameBuffer *buffer = Napi::ObjectWrap<WebGLFrameBuffer>::Unwrap(info[1].As<Napi::Object>());
    frameBufferId = buffer->getId();
}
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsFramebuffer(frameBufferId));
}

DEFINE_VOID_METHOD(framebufferRenderbuffer)
CHECK_PARAM_LEGNTH(4)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum attachment = info[1].As<Napi::Number>().Uint32Value();
GLenum renderBufferTarget = info[2].As<Napi::Number>().Uint32Value();
WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[3].As<Napi::Object>());
glFramebufferRenderbuffer(target, attachment, renderBufferTarget, renderBuffer->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createRenderBuffer)
CHECK_PARAM_LEGNTH(0)
GLuint renderBufferId;
glGenRenderbuffers(1, &renderBufferId);
Napi::Object obj = WebGLFrameBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), renderBufferId));
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(renderbufferStorage)
CHECK_PARAM_LEGNTH(4)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum format = info[1].As<Napi::Number>().Uint32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
glRenderbufferStorage(target, format, width, height);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteBuffer)
CHECK_PARAM_LEGNTH(1)
WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
GLuint id = buffer->getId();
glDeleteBuffers(1, &id);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteTexture)
CHECK_PARAM_LEGNTH(1)
WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
GLuint id = texture->getId();
glDeleteTextures(1, &id);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteFrameBuffer)
CHECK_PARAM_LEGNTH(1)
WebGLFrameBuffer *frameBuffer = Napi::ObjectWrap<WebGLFrameBuffer>::Unwrap(info[0].As<Napi::Object>());
GLuint id = frameBuffer->getId();
glDeleteTextures(1, &id);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteRenderBuffer)
CHECK_PARAM_LEGNTH(1)
WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[0].As<Napi::Object>());
GLuint id = renderBuffer->getId();
glDeleteTextures(1, &id);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(framebufferTexture2D)
CHECK_PARAM_LEGNTH(5)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum attachment = info[1].As<Napi::Number>().Uint32Value();
GLenum textarget = info[2].As<Napi::Number>().Uint32Value();
WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[3].As<Napi::Object>());
GLint level = info[4].As<Napi::Number>().Int32Value();
glFramebufferTexture2D(target, attachment, textarget, texture->getId(), level);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getBufferParameter)
CHECK_PARAM_LEGNTH(2)
GLuint target = info[0].As<Napi::Number>().Uint32Value();
GLuint pname = info[1].As<Napi::Number>().Uint32Value();
GLint result;
glGetBufferParameteriv(target, pname, &result);
RECORD_TIME_END
return Napi::Number::New(info.Env(), result);
}

DEFINE_RETURN_VALUE_METHOD(isBuffer)
CHECK_PARAM_LEGNTH(1)
WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsBuffer(buffer->getId()));
}

DEFINE_RETURN_VALUE_METHOD(isShader)
CHECK_PARAM_LEGNTH(1)
WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsShader(buffer->getId()));
}

DEFINE_RETURN_VALUE_METHOD(isRenderBuffer)
CHECK_PARAM_LEGNTH(1)
WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[0].As<Napi::Object>());
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsRenderbuffer(renderBuffer->getId()));
}

DEFINE_RETURN_VALUE_METHOD(isTexture)
CHECK_PARAM_LEGNTH(1)
WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsTexture(texture->getId()));
}

DEFINE_RETURN_VALUE_METHOD(isProgram)
CHECK_PARAM_LEGNTH(1)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
RECORD_TIME_END
return Napi::Number::New(info.Env(), glIsProgram(program->getId()));
}

DEFINE_VOID_METHOD(clearDepth)
CHECK_PARAM_LEGNTH(1)
GLclampd depth = info[0].As<Napi::Number>().DoubleValue();
glClearDepth(depth);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getTexParameter)
CHECK_PARAM_LEGNTH(2)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLint result;
glGetTexParameteriv(target, pname, &result);
RECORD_TIME_END
return Napi::Number::New(info.Env(), result);
}

DEFINE_RETURN_VALUE_METHOD(getFramebufferAttachmentParameter)
CHECK_PARAM_LEGNTH(2)
// GLenum target = info[0].As<Napi::Number>().Uint32Value();
// GLenum pname = info[1].As<Napi::Number>().Uint32Value();
// GLint result;
// glGetTexParameteriv(target, pname, &result);
// RECORD_TIME_END
// return Napi::Number::New(info.Env(), result);
}

DEFINE_VOID_METHOD(stencilFuncSeparate)
CHECK_PARAM_LEGNTH(4)
GLenum face = info[0].As<Napi::Number>().Uint32Value();
GLenum sfail = info[1].As<Napi::Number>().Uint32Value();
GLenum dpfail = info[2].As<Napi::Number>().Uint32Value();
GLenum dpass = info[3].As<Napi::Number>().Uint32Value();
glStencilFuncSeparate(face, sfail, dpfail, dpass);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilMaskSeparate)
CHECK_PARAM_LEGNTH(2)
GLenum face = info[0].As<Napi::Number>().Uint32Value();
GLuint mask = info[1].As<Napi::Number>().Uint32Value();
glStencilMaskSeparate(face, mask);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilOpSeparate)
CHECK_PARAM_LEGNTH(4)
GLenum face = info[0].As<Napi::Number>().Uint32Value();
GLenum sfail = info[1].As<Napi::Number>().Uint32Value();
GLenum dpfail = info[2].As<Napi::Number>().Uint32Value();
GLenum dpass = info[3].As<Napi::Number>().Uint32Value();
glStencilOpSeparate(face, sfail, dpfail, dpass);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(clearStencil)
CHECK_PARAM_LEGNTH(1)
GLint s = info[0].As<Napi::Number>().Int32Value();
glClearStencil(s);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(validateProgram)
CHECK_PARAM_LEGNTH(1)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glValidateProgram(program->getId());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthFunc)
CHECK_PARAM_LEGNTH(1)
GLenum func = info[0].As<Napi::Number>().Uint32Value();
glDepthMask(func);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthMask)
CHECK_PARAM_LEGNTH(1)
GLboolean flag = info[0].As<Napi::Boolean>().Value();
glDepthMask(flag);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(disable)
CHECK_PARAM_LEGNTH(1)
GLenum cap = info[0].As<Napi::Number>().Uint32Value();
glDisable(cap);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendColor)
CHECK_PARAM_LEGNTH(4)
GLclampf red = info[0].As<Napi::Number>().DoubleValue();
GLclampf green = info[1].As<Napi::Number>().DoubleValue();
GLclampf blue = info[2].As<Napi::Number>().DoubleValue();
GLclampf alpha = info[3].As<Napi::Number>().DoubleValue();
glBlendColor(red, green, blue, alpha);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendFunc)
CHECK_PARAM_LEGNTH(2)
GLenum sfactor = info[0].As<Napi::Number>().Uint32Value();
GLenum dfactor = info[1].As<Napi::Number>().Uint32Value();
glBlendFunc(sfactor, dfactor);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendFuncSeparate)
CHECK_PARAM_LEGNTH(4)
GLenum one = info[0].As<Napi::Number>().Uint32Value();
GLenum two = info[1].As<Napi::Number>().Uint32Value();
GLenum three = info[2].As<Napi::Number>().Uint32Value();
GLenum four = info[3].As<Napi::Number>().Uint32Value();
glBlendFuncSeparate(one, two, three, four);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendEquation)
CHECK_PARAM_LEGNTH(2)
GLenum one = info[0].As<Napi::Number>().Uint32Value();
GLenum two = info[1].As<Napi::Number>().Uint32Value();
glBlendEquationSeparate(one, two);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendEquationSeparate)
CHECK_PARAM_LEGNTH(4)
GLenum one = info[0].As<Napi::Number>().Uint32Value();
GLenum two = info[1].As<Napi::Number>().Uint32Value();
GLenum three = info[2].As<Napi::Number>().Uint32Value();
GLenum four = info[4].As<Napi::Number>().Uint32Value();
glBlendFuncSeparate(one, two, three, four);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(cullFace)
CHECK_PARAM_LEGNTH(1)
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
glCullFace(mode);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getError)
CHECK_PARAM_LEGNTH(0)
return Napi::Number::New(info.Env(), glGetError());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(bindAttribLocation)
CHECK_PARAM_LEGNTH(3)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
GLuint index = info[1].As<Napi::Number>().Uint32Value();
std::string name = info[2].As<Napi::String>().Utf8Value();
glBindAttribLocation(program->getId(), index, name.c_str());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(polygonOffset)
CHECK_PARAM_LEGNTH(2)
GLfloat factor = info[0].As<Napi::Number>().FloatValue();
GLfloat untis = info[1].As<Napi::Number>().FloatValue();
glPolygonOffset(factor, untis);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(lineWidth)
CHECK_PARAM_LEGNTH(1)
GLfloat width = info[0].As<Napi::Number>().FloatValue();
glLineWidth(width);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(sampleCoverage)
CHECK_PARAM_LEGNTH(2)
GLfloat value = info[0].As<Napi::Number>().FloatValue();
GLboolean invert = info[1].As<Napi::Boolean>().Value();
glSampleCoverage(value, invert);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(hint)
CHECK_PARAM_LEGNTH(2)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
GLenum mode = info[1].As<Napi::Number>().Uint32Value();
glHint(target, mode);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(isEnabled)
CHECK_PARAM_LEGNTH(1)
GLenum cap = info[0].As<Napi::Number>().Uint32Value();
GLboolean ret = glIsEnabled(cap);
RECORD_TIME_END
return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(readPixels)
CHECK_PARAM_LEGNTH(7)
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
GLenum format = info[4].As<Napi::Number>().Uint32Value();
GLenum type = info[5].As<Napi::Number>().Uint32Value();
void *pixels = nullptr;
if (info[6].IsTypedArray())
{
    Napi::TypedArray array = info[1].As<Napi::TypedArray>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    pixels = buffer.Data();
}
else if (info[6].IsArrayBuffer())
{
    Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    pixels = buffer.Data();
}
glReadPixels(x, y, width, height, format, type, pixels);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthRange)
CHECK_PARAM_LEGNTH(2)
GLfloat near = info[0].As<Napi::Number>().FloatValue();
GLfloat far = info[1].As<Napi::Number>().FloatValue();
glDepthRange(near, far);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(generateMipmap)
CHECK_PARAM_LEGNTH(1)
GLenum target = info[0].As<Napi::Number>().Uint32Value();
glGenerateMipmap(target);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getAttachedShaders)
CHECK_PARAM_LEGNTH(1)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
int maxCount = 32;
int shaderCount = 0;
GLuint shaders[maxCount];
glGetAttachedShaders(program->getId(), maxCount, &shaderCount, shaders);
Napi::Array arr = Napi::Array::New(info.Env());
for (int i = 0; i < shaderCount; i++)
{
    Napi::Object shader = WebGLShader::NewInstance(info.Env(), Napi::Number::New(info.Env(), shaders[i]));
    arr.Set(i, shader);
}
RECORD_TIME_END
return arr;
}

DEFINE_VOID_METHOD(disableVertexAttribArray)
CHECK_PARAM_LEGNTH(1)
GLuint index = info[0].As<Napi::Number>().Uint32Value();
glDisableVertexAttribArray(index);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getActiveAttrib)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
GLuint programId = program->getId();
GLint index = info[1].As<Napi::Number>().Int32Value();
GLsizei length;
glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
GLchar *buffer = new (std::nothrow) GLchar[length];
GLint size = 0;
GLenum type = 0;
glGetActiveAttrib(programId, index, length, NULL, &size, &type, buffer);
Napi::Object activeInfoObj = WebGLActiveInfo::NewInstance(info.Env(), size, type, buffer);
RECORD_TIME_END
return activeInfoObj;
}

DEFINE_RETURN_VALUE_METHOD(getActiveUniform)
CHECK_PARAM_LEGNTH(2)
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
GLuint programId = program->getId();
GLint index = info[1].As<Napi::Number>().Int32Value();
GLsizei length;
glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
GLchar *buffer = new (std::nothrow) GLchar[length];
GLint size = 0;
GLenum type = 0;
glGetActiveUniform(programId, index, length, NULL, &size, &type, buffer);
Napi::Object activeInfoObj = WebGLActiveInfo::NewInstance(info.Env(), size, type, buffer);
RECORD_TIME_END
return activeInfoObj;
}

DEFINE_RETURN_VALUE_METHOD(getUniform)
// glGetUniformiv()
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getVertexAttrib)
CHECK_PARAM_LEGNTH(2)
GLuint index = info[0].As<Napi::Number>().Uint32Value();
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)
{ // WebGLBuffer
    GLint params;
    glGetVertexAttribiv(index, pname, &params);
    return Napi::Number::New(info.Env(), params);
}
else if (pname == GL_VERTEX_ATTRIB_ARRAY_ENABLED ||
         pname == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED)
{ // GLBool
    GLint params;
    glGetVertexAttribiv(index, pname, &params);
    return Napi::Number::New(info.Env(), params);
}
else if (pname == GL_VERTEX_ATTRIB_ARRAY_SIZE ||
         pname == GL_VERTEX_ATTRIB_ARRAY_STRIDE)
{ // GLint
    GLint params;
    glGetVertexAttribiv(index, pname, &params);
    return Napi::Number::New(info.Env(), params);
}
else if (pname == GL_VERTEX_ATTRIB_ARRAY_TYPE)
{ // GLEnum
    GLint params;
    glGetVertexAttribiv(index, pname, &params);
    return Napi::Number::New(info.Env(), params);
}
else if (pname == GL_CURRENT_VERTEX_ATTRIB)
{ // 4元素 Float32Array
    GLfloat *data = new GLfloat[4];
    glGetVertexAttribfv(index, pname, data);
    Napi::Array arr = Napi::Array::New(info.Env());
    for (int i = 0; i < 4; i++)
    {
        arr.Set(i, Napi::Number::New(info.Env(), data[i]));
    }
    return arr;
}
RECORD_TIME_END
}
DEFINE_RETURN_VALUE_METHOD(getVertexAttribOffset)
CHECK_PARAM_LEGNTH(2)
GLuint index = info[0].As<Napi::Number>().Uint32Value();
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
if (pname == GL_VERTEX_ATTRIB_ARRAY_POINTER)
{
    int *p;
    glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, (void **)&p);
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), *p);
}
else
{
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), 0);
}
}

DEFINE_RETURN_VALUE_METHOD(isContextLost)
bool flag = false;
if (!mRenderContext)
{
    flag = true;
}
RECORD_TIME_END
return Napi::Boolean::New(info.Env(), flag);
}

ContextWebGL::~ContextWebGL()
{
    this->mRenderContext = nullptr;
}

} // namespace NodeBindingDataConvert::AsUInt(args[0]);