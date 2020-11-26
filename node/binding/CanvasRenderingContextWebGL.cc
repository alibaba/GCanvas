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

#define  ENABLE_CHECK_GL_ERROR
#ifdef ENABLE_CHECK_GL_ERROR
void CheckGLError(const char* stmt, const char* fname, int line);
#define GL_CHECK(stmt)  \
        stmt;   \
        CheckGLError(#stmt, __FILE__, __LINE__);

#else
#define GL_CHECK(stmt)  \
    do{     \
        stmt;   \
    } while (0)
#endif



// #define ENABLE_RECORD_COST_TIME
#ifdef ENABLE_RECORD_COST_TIME
#define RECORD_TIME_BEGIN  \
    clock_t start, finish; \
    start = clock();
#define RECORD_TIME_END \
    finish = clock();   \
    printf("[%s]cost time %.5f ms, \n", __FUNCTION__,   (double)(finish - start) * 1000.0f / CLOCKS_PER_SEC);
#else
#define RECORD_TIME_BEGIN
#define RECORD_TIME_END
#endif

#define  EGL_MAKE_CURRENT   mRenderContext->makeCurrent();


#define DEFINE_VOID_METHOD(methodName)    void ContextWebGL::methodName(const Napi::CallbackInfo &info) 

#define DEFINE_RETURN_VALUE_METHOD(methodName)  Napi::Value ContextWebGL::methodName(const Napi::CallbackInfo &info)



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
        Napi::Function func = DefineClass(env, "ContextWebGL", {
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
        });
        constructor = Napi::Persistent(func);
    }
    ContextWebGL::ContextWebGL(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ContextWebGL>(info)
    {
        mUnpackFlipYWebGL = false;
        mUnpackPremultiplyAlphaWebGL = false;
    }

    ContextWebGL::~ContextWebGL()
    {
        mRenderContext = nullptr;
    }

    Napi::Object ContextWebGL::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "contextwebgl"));
        return obj;
    }

DEFINE_VOID_METHOD(clear)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLbitfield mask = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK( glClear(mask) );
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(clearColor)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;

    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat red = info[0].As<Napi::Number>().FloatValue();
    GLfloat green = info[1].As<Napi::Number>().FloatValue();
    GLfloat blue = info[2].As<Napi::Number>().FloatValue();
    GLfloat alpha = info[3].As<Napi::Number>().FloatValue();
    GL_CHECK( glClearColor(red, green, blue, alpha) );
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(enable)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum cap = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK( glEnable(cap) );
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createBuffer)
{
    // RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    // GL_CHECK(glGenBuffers(1, &bufferId));
    Napi::Object obj = WebGLBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), bufferId));
    // mRenderContext->getCtxWebGL()->AddGLResource(Buffer, bufferId);
    // RECORD_TIME_END
    return obj;
}

DEFINE_VOID_METHOD(bindBuffer)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;

    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
    GL_CHECK(glBindBuffer(type, bufferId));
    RECORD_TIME_END
}

/**
 *WebGL1 支持多种方式
 * gl.bufferData(target, size, usage);
 * gl.bufferData(target, ArrayBuffer? srcData, usage);
 * gl.bufferData(target, ArrayBufferView srcData, usage);
 **/
DEFINE_VOID_METHOD(bufferData)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;

    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum usage = info[2].As<Napi::Number>().Uint32Value();
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
        GL_CHECK(glBufferData(target, buffer.ByteLength(), buffer.Data(), usage));
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
        GL_CHECK(glBufferData(target, array.ByteLength(), buffer.Data(), usage));
    }
    else if( info[1].IsArray() )
    {
        //TODO
    }
    else if (info[1].IsNumber())
    {
        GLuint size = info[1].As<Napi::Number>().Int32Value();
        GL_CHECK(glBufferData(target, size, NULL, usage));
    }
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(bufferSubData)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum offset = info[1].As<Napi::Number>().Uint32Value();
    if (info[2].IsTypedArray())
    {
        Napi::TypedArray array = info[2].As<Napi::TypedArray>();
        Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
        GL_CHECK(glBufferSubData(target, offset, buffer.ByteLength(), buffer.Data()));
    }
    else if (info[2].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[2].As<Napi::ArrayBuffer>();
        Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
        GL_CHECK(glBufferSubData(target, offset, buffer.ByteLength(), buffer.Data()));
    }
    else if( info[2].IsArray() )
    {
        //TODO
    }
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createShader)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return  info.Env().Null();

    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    Napi::Env env = info.Env();
    GLenum shaderType = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK( GLuint shaderId = glCreateShader(shaderType) );
    Napi::Object obj = WebGLShader::NewInstance(env, Napi::Number::New(env, shaderId));
    RECORD_TIME_END
    return obj;
}

DEFINE_VOID_METHOD(shaderSource)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
    const char *shaderContent = shaderStr.c_str();
    GLint shaderContentLen = shaderStr.size();
    GLuint shaderId = shader->getId();
    GL_CHECK(glShaderSource(shader->getId(), 1, &shaderContent, &shaderContentLen));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(compileShader)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glCompileShader(shader->getId()));
    GLint compiled = 0;
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(attachShader)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
    GL_CHECK(glAttachShader(program->getId(), shader->getId()));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(detachShader)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
    GL_CHECK(glDetachShader(program->getId(), shader->getId()));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createProgram)
{    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint programId = glCreateProgram();
    RECORD_TIME_END
    return WebGLProgram::NewInstance(info.Env(), Napi::Number::New(info.Env(), programId));
}

DEFINE_VOID_METHOD(linkProgram)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glLinkProgram(program->getId()));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(useProgram)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glUseProgram(program->getId()));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getAttribLocation)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return  info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string name = info[1].As<Napi::String>().Utf8Value();
    GL_CHECK(GLuint pos = glGetAttribLocation(program->getId(), name.c_str()));
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), pos);
}

// DEFINE_VOID_METHOD(viewport)
void ContextWebGL::viewport(const Napi::CallbackInfo &info) 
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLint x = info[0].As<Napi::Number>().Int32Value();
    GLint y = info[1].As<Napi::Number>().Int32Value();
    GLsizei width = info[2].As<Napi::Number>().Int32Value();
    GLsizei height = info[3].As<Napi::Number>().Int32Value();
    int dpi = mRenderContext->getDpi();
    printf("dpi=%d\n", dpi);
    GL_CHECK(glViewport(x *dpi, y *dpi, width *dpi, height *dpi));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(drawElements)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum mode = info[0].As<Napi::Number>().Uint32Value();
    GLsizei count = info[1].As<Napi::Number>().Int32Value();
    GLenum type = info[2].As<Napi::Number>().Uint32Value();
    GLuint offset = info[3].As<Napi::Number>().Uint32Value();
    GL_CHECK(glDrawElements(mode, count, type, (GLvoid *)(intptr_t)offset));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(flush)
{
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(glFlush());
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(finish)
{
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(glFinish());
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttribPointer)
{
    if( !CHECK_PARAM_LEGNTH(6) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint index = info[0].As<Napi::Number>().Int32Value();
    GLint size = info[1].As<Napi::Number>().Int32Value();
    GLenum type = info[2].As<Napi::Number>().Uint32Value();
    GLboolean isNormalized = info[3].As<Napi::Boolean>().Value();
    GLuint stride = info[4].As<Napi::Number>().Int32Value();
    GLuint offset = info[5].As<Napi::Number>().Int32Value();
    GL_CHECK(glVertexAttribPointer(index, size, type, isNormalized, stride, (GLvoid *)(intptr_t)offset));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(enableVertexAttribArray)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint index = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glEnableVertexAttribArray(index));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(scissor)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLint x = info[0].As<Napi::Number>().Int32Value();
    GLint y = info[1].As<Napi::Number>().Int32Value();
    GLsizei width = info[2].As<Napi::Number>().Int32Value();
    GLsizei height = info[3].As<Napi::Number>().Int32Value();
    int dpi = mRenderContext->getDpi();
    GL_CHECK(glScissor(x *dpi, y *dpi, width *dpi, height *dpi));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(drawArrays)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum mode = info[0].As<Napi::Number>().Uint32Value();
    GLint first = info[1].As<Napi::Number>().Int32Value();
    GLint count = info[2].As<Napi::Number>().Int32Value();
    GL_CHECK(glDrawArrays(mode, first, count));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getShaderParameter)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    GLint ret = 0;
    GL_CHECK(glGetShaderiv(shader->getId(), pname, &ret));
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(getShaderInfoLog)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::String::New(info.Env(), "");
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
        GL_CHECK(glGetShaderInfoLog(shader->getId(), length, &real_size, src));
        RECORD_TIME_END
        return Napi::String::New(info.Env(), src);
    }
}

DEFINE_VOID_METHOD(deleteShader)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glDeleteShader(shader->getId()));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getProgramParameter)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    GLint ret;
    GL_CHECK(glGetProgramiv(program->getId(), pname, &ret));
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(deleteProgram)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glDeleteProgram(program->getId()));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getUniformLocation)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
    GL_CHECK(GLuint index = glGetUniformLocation(program->getId(), shaderStr.c_str()));
    Napi::Object obj = WebGLUniformLocation::NewInstance(info.Env(), index);
    RECORD_TIME_END
    return obj;
}
DEFINE_VOID_METHOD(uniform1f)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLfloat v1 = parseFloat(info[1]);
    GL_CHECK(glUniform1f(location->getIndex(), v1));
    RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2f)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLfloat v1 = parseFloat(info[1]);
    GLfloat v2 = parseFloat(info[2]);
    GL_CHECK(glUniform2f(location->getIndex(), v1, v2));
    RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3f)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLfloat v1 = parseFloat(info[1]);
    GLfloat v2 = parseFloat(info[2]);
    GLfloat v3 = parseFloat(info[3]);
    GL_CHECK(glUniform3f(location->getIndex(), v1, v2, v3));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4f)
{
    if( !CHECK_PARAM_LEGNTH(5) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLfloat v1 = parseFloat(info[1]);
    GLfloat v2 = parseFloat(info[2]);
    GLfloat v3 = parseFloat(info[3]);
    GLfloat v4 = parseFloat(info[4]);
    GL_CHECK(glUniform4f(location->getIndex(), v1, v2, v3, v4));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(colorMask)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLint v1, v2, v3, v4;
    v1 = parseInt(info[0]);
    v2 = parseInt(info[1]);
    v3 = parseInt(info[2]);
    v4 = parseInt(info[3]);
    GL_CHECK(glColorMask(v1, v2, v3, v4));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1i)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLint v1 = info[1].As<Napi::Number>().Int32Value();
    GL_CHECK(glUniform1i(location->getIndex(), v1));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2i)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLint v1 = info[1].As<Napi::Number>().Int32Value();
    GLint v2 = info[2].As<Napi::Number>().Int32Value();
    GL_CHECK(glUniform2i(location->getIndex(), v1, v2));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3i)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLint v1 = info[1].As<Napi::Number>().Int32Value();
    GLint v2 = info[2].As<Napi::Number>().Int32Value();
    GLint v3 = info[3].As<Napi::Number>().Int32Value();
    GL_CHECK(glUniform3i(location->getIndex(), v1, v2, v3));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4i)
{
    if( !CHECK_PARAM_LEGNTH(5) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLint v1 = info[1].As<Napi::Number>().Int32Value();
    GLint v2 = info[2].As<Napi::Number>().Int32Value();
    GLint v3 = info[3].As<Napi::Number>().Int32Value();
    GLint v4 = info[4].As<Napi::Number>().Int32Value();
    GL_CHECK(glUniform4i(location->getIndex(), v1, v2, v3, v4));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1fv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK( parseTypeArrayAndCallUniformFloatFunc(info, glUniform1fv) );
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2fv)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformFloatFunc(info, glUniform2fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3fv)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformFloatFunc(info, glUniform3fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4fv)
{
    if( !CHECK_PARAM_LEGNTH(5) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformFloatFunc(info, glUniform4fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1iv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformIntFunc(info, glUniform1iv));
    RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2iv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformIntFunc(info, glUniform2iv));
    RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3iv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformIntFunc(info, glUniform3iv));
    RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform4iv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformIntFunc(info, glUniform4iv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix2fv)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformMatrixFunc(info, 4, glUniformMatrix2fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix3fv)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformMatrixFunc(info, 9,  glUniformMatrix3fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix4fv)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallUniformMatrixFunc(info, 16, glUniformMatrix4fv));
    RECORD_TIME_END
}

void ContextWebGL::parseTypeArrayAndCallUniformFloatFunc(const Napi::CallbackInfo &info, glUniformFloatPtr func)
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
    else if( info[1].IsArray() )
    {
        Napi::Array array  = info[2].As<Napi::Array>();
        float floatArray[array.Length()];
        for (size_t i = 0; i < array.Length(); i++)
        {
            floatArray[i] =  array.Get(i).ToNumber();
        }
        func(location->getIndex(), array.Length(), floatArray);
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformIntFunc(const Napi::CallbackInfo &info, glUniformIntPtr func)
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
    else if( info[1].IsArray() )
    {
        Napi::Array array  = info[2].As<Napi::Array>();
        int intArray[array.Length()];
        for (size_t i = 0; i < array.Length(); i++)
        {
            intArray[i] =  array.Get(i).ToNumber();
        }
        func(location->getIndex(), array.Length(), intArray);
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformMatrixFunc(const Napi::CallbackInfo &info, int size, glUniformMatrixPtr func)
{
    WebGLUniformLocation *location = Napi::ObjectWrap<WebGLUniformLocation>::Unwrap(info[0].As<Napi::Object>());
    GLboolean transpose = info[1].As<Napi::Boolean>().Value();

    if (info[2].IsTypedArray())
    { /* constant-expression */
        Napi::TypedArray array = info[2].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength()/size, transpose, buffer.Data());
    }
    else if (info[2].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[2].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(location->getIndex(), buffer.ElementLength()/size, transpose, buffer.Data());
    }
    else if( info[2].IsArray() )
    {
        Napi::Array array  = info[2].As<Napi::Array>();
        float floatArray[array.Length()];
        for (size_t i = 0; i < array.Length(); i++)
        {
            floatArray[i] =  array.Get(i).ToNumber();
        }
    
        func(location->getIndex(), array.Length()/size, transpose, floatArray);
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
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
    GL_CHECK(glVertexAttrib1f(location, v1));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib2f)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
    GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
    GL_CHECK(glVertexAttrib2f(location, v1, v2));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib3f)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
    GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
    GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
    GL_CHECK(glVertexAttrib3f(location, v1, v2, v3));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib4f)
{
    if( !CHECK_PARAM_LEGNTH(5) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
    GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
    GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
    GLfloat v4 = info[4].As<Napi::Number>().FloatValue();
    GL_CHECK(glVertexAttrib4f(location, v1, v2, v3, v4));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib1fv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallVertexFunc(info, glVertexAttrib1fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib2fv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallVertexFunc(info, glVertexAttrib2fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib3fv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallVertexFunc(info, glVertexAttrib3fv));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib4fv)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GL_CHECK(parseTypeArrayAndCallVertexFunc(info, glVertexAttrib4fv));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createTexture)
{
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint textureId;
    GL_CHECK(glGenTextures(1, &textureId));
    Napi::Object obj = WebGLTexture::NewInstance(info.Env(), Napi::Number::New(info.Env(), textureId));
    RECORD_TIME_END
    return obj;
}

DEFINE_VOID_METHOD(bindTexture)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
    GL_CHECK(glBindTexture(type, textureId));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(pixelStorei)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    GLenum pname = info[0].As<Napi::Number>().Uint32Value();
    GLuint param = parseUInt(info[1]);
    switch (pname)
    {
    case GL_UNPACK_FLIP_Y_WEBGL:
        mUnpackFlipYWebGL = param;
        break;

    case GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
        mUnpackPremultiplyAlphaWebGL = param;
        break;

    case GL_UNPACK_COLORSPACE_CONVERSION_WEBGL:
        printf("Unsupport  GL_UNPACK_COLORSPACE_CONVERSION_WEBGL");
        break;

    default:
        EGL_MAKE_CURRENT
        GL_CHECK(glPixelStorei(pname, param));
        break;
    }
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(texParameteri)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    GLenum param = info[2].As<Napi::Number>().Uint32Value();
    GL_CHECK(glTexParameteri(target, pname, param));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(texParameterf)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat target = info[0].As<Napi::Number>().FloatValue();
    GLfloat pname = info[1].As<Napi::Number>().FloatValue();
    GLfloat param = info[2].As<Napi::Number>().FloatValue();
    GL_CHECK(glTexParameterf(target, pname, param));
    RECORD_TIME_END
}
// support:
//gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
DEFINE_VOID_METHOD(texImage2D)
{
    if( !CHECK_PARAM_LEGNTH(6) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
            GL_CHECK(glTexImage2D(target, level, internalFormat, width, height, border, format, type, nullptr));
        }
        else if (info[5].IsObject())
        {
            Napi::Object object = info[5].As<Napi::Object>();
            Napi::Value name = object.Get("name");
            std::string namePropetry = name.As<Napi::String>().Utf8Value();
            //todo canvas
            if (namePropetry == "image")
            {
                Image *image = Napi::ObjectWrap<Image>::Unwrap(info[5].As<Napi::Object>());

                //一次处理 1 个字节
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                GL_CHECK(glTexImage2D(target, level, internalFormat,image->getWidth(),image->getHeight(), 
                                                                        border, format, type, &image->getPixels()[0]));
            }
            else
            {

            }
        }
    }
    else if( info.Length() == 9 )
    {
        //TODO 9个参数
    }
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilFunc)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum func = info[0].As<Napi::Number>().Uint32Value();
    GLint ref = info[1].As<Napi::Number>().Int32Value();
    GLuint mask = info[2].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilFunc(func, ref, mask));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilOp)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum fail = info[0].As<Napi::Number>().Uint32Value();
    GLenum zfail = info[1].As<Napi::Number>().Uint32Value();
    GLenum zpass = info[2].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilOp(fail, zfail, zpass));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(activeTexture)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum texture = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glActiveTexture(texture));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilMask)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLint mask = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilMask(mask));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(frontFace)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum mode = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glFrontFace(mode));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createFrameBuffer)
{    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT  
    GLuint frameBufferId;
    GL_CHECK(glGenFramebuffers(1, &frameBufferId));
    Napi::Object obj = WebGLFrameBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), frameBufferId));
    RECORD_TIME_END
    return obj;
}

DEFINE_VOID_METHOD(bindFramebuffer)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
    GL_CHECK(glBindFramebuffer(target, frameBufferId));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getShaderSource)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
    GLsizei length;
    glGetShaderiv(shader->getId(), GL_SHADER_SOURCE_LENGTH, &length);
    GLchar *src = new GLchar[length];
    GL_CHECK(glGetShaderSource(shader->getId(), length, nullptr, src));
    RECORD_TIME_END
    return Napi::String::New(info.Env(), src);
}

DEFINE_RETURN_VALUE_METHOD(checkFramebufferStatus)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum status = glCheckFramebufferStatus(target);
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), status);
}

DEFINE_RETURN_VALUE_METHOD(isFramebuffer)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return info.Env().Null();
    
    bool isFrameBuffer = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT

        WebGLFrameBuffer *buffer = Napi::ObjectWrap<WebGLFrameBuffer>::Unwrap(info[1].As<Napi::Object>());
        GLuint frameBufferId = buffer->getId();
        GL_CHECK(isFrameBuffer = glIsFramebuffer(frameBufferId));
        RECORD_TIME_END
    }
    return Napi::Number::New(info.Env(), glIsFramebuffer(isFrameBuffer));
}

DEFINE_VOID_METHOD(framebufferRenderbuffer)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum attachment = info[1].As<Napi::Number>().Uint32Value();
    GLenum renderBufferTarget = info[2].As<Napi::Number>().Uint32Value();
    WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[3].As<Napi::Object>());
    GL_CHECK(glFramebufferRenderbuffer(target, attachment, renderBufferTarget, renderBuffer->getId()));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createRenderBuffer)
{
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint renderBufferId;
    glGenRenderbuffers(1, &renderBufferId);
    Napi::Object obj = WebGLFrameBuffer::NewInstance(info.Env(), Napi::Number::New(info.Env(), renderBufferId));
    RECORD_TIME_END
    return obj;
}

DEFINE_VOID_METHOD(renderbufferStorage)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum format = info[1].As<Napi::Number>().Uint32Value();
    GLsizei width = info[2].As<Napi::Number>().Int32Value();
    GLsizei height = info[3].As<Napi::Number>().Int32Value();
    GL_CHECK(glRenderbufferStorage(target, format, width, height));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteBuffer)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
    GLuint id = buffer->getId();
    GL_CHECK(glDeleteBuffers(1, &id));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteTexture)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
    GLuint id = texture->getId();
    GL_CHECK(glDeleteTextures(1, &id));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteFrameBuffer)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLFrameBuffer *frameBuffer = Napi::ObjectWrap<WebGLFrameBuffer>::Unwrap(info[0].As<Napi::Object>());
    GLuint id = frameBuffer->getId();
    GL_CHECK(glDeleteTextures(1, &id));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(deleteRenderBuffer)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[0].As<Napi::Object>());
    GLuint id = renderBuffer->getId();
    GL_CHECK(glDeleteTextures(1, &id));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(framebufferTexture2D)
{
    if( !CHECK_PARAM_LEGNTH(5) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum attachment = info[1].As<Napi::Number>().Uint32Value();
    GLenum textarget = info[2].As<Napi::Number>().Uint32Value();
    WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[3].As<Napi::Object>());
    GLint level = info[4].As<Napi::Number>().Int32Value();
    GL_CHECK(glFramebufferTexture2D(target, attachment, textarget, texture->getId(), level));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getBufferParameter)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint target = info[0].As<Napi::Number>().Uint32Value();
    GLuint pname = info[1].As<Napi::Number>().Uint32Value();
    GLint result;
    GL_CHECK(glGetBufferParameteriv(target, pname, &result));
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), result);
}

DEFINE_RETURN_VALUE_METHOD(isBuffer)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::Boolean::New(info.Env(), false);

    bool ret = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[0].As<Napi::Object>());
        GL_CHECK(ret = glIsBuffer(buffer->getId()));
        RECORD_TIME_END
    }
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(isShader)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::Boolean::New(info.Env(), false);

    bool ret = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
        GL_CHECK(ret = glIsShader(shader->getId()));
        RECORD_TIME_END
    }
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(isRenderBuffer)
{
     if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::Boolean::New(info.Env(), false);

    bool ret = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        WebGLRenderBuffer *renderBuffer = Napi::ObjectWrap<WebGLRenderBuffer>::Unwrap(info[0].As<Napi::Object>());
        GL_CHECK(ret = glIsRenderbuffer(renderBuffer->getId()));
        RECORD_TIME_END
    }
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(isTexture)
{
     if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::Boolean::New(info.Env(), false);

    bool ret = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        WebGLTexture *texture = Napi::ObjectWrap<WebGLTexture>::Unwrap(info[0].As<Napi::Object>());
        GL_CHECK(ret = glIsTexture(texture->getId()));
        RECORD_TIME_END
    }
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(isProgram)
{
     if( !CHECK_PARAM_LEGNTH(1) )
        return Napi::Boolean::New(info.Env(), false);

    bool ret = false;
    if( info[0].IsObject() )
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
        GL_CHECK(ret = glIsProgram(program->getId()));
        RECORD_TIME_END
    }
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(clearDepth)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat depth = info[0].As<Napi::Number>().FloatValue();
    GL_CHECK(glClearDepthf(depth));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getTexParameter)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    GLint result;
    GL_CHECK(glGetTexParameteriv(target, pname, &result));
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), result);
}

DEFINE_RETURN_VALUE_METHOD(getFramebufferAttachmentParameter)
{
    //TODO
// !CHECK_PARAM_LEGNTH(2)
// GLenum target = info[0].As<Napi::Number>().Uint32Value();
// GLenum pname = info[1].As<Napi::Number>().Uint32Value();
// GLint result;
// glGetTexParameteriv(target, pname, &result);
// RECORD_TIME_END
// return Napi::Number::New(info.Env(), result);
}

DEFINE_VOID_METHOD(stencilFuncSeparate)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum face = info[0].As<Napi::Number>().Uint32Value();
    GLenum sfail = info[1].As<Napi::Number>().Uint32Value();
    GLenum dpfail = info[2].As<Napi::Number>().Uint32Value();
    GLenum dpass = info[3].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilFuncSeparate(face, sfail, dpfail, dpass));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilMaskSeparate)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum face = info[0].As<Napi::Number>().Uint32Value();
    GLuint mask = info[1].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilMaskSeparate(face, mask));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(stencilOpSeparate)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum face = info[0].As<Napi::Number>().Uint32Value();
    GLenum sfail = info[1].As<Napi::Number>().Uint32Value();
    GLenum dpfail = info[2].As<Napi::Number>().Uint32Value();
    GLenum dpass = info[3].As<Napi::Number>().Uint32Value();
    GL_CHECK(glStencilOpSeparate(face, sfail, dpfail, dpass));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(clearStencil)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLint s = info[0].As<Napi::Number>().Int32Value();
    GL_CHECK(glClearStencil(s));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(validateProgram)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GL_CHECK(glValidateProgram(program->getId()));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthFunc)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum func = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glDepthMask(func));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthMask)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLboolean flag = info[0].As<Napi::Boolean>().Value();
    GL_CHECK(glDepthMask(flag));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(disable)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum cap = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glDisable(cap));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendColor)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLclampf red = info[0].As<Napi::Number>().DoubleValue();
    GLclampf green = info[1].As<Napi::Number>().DoubleValue();
    GLclampf blue = info[2].As<Napi::Number>().DoubleValue();
    GLclampf alpha = info[3].As<Napi::Number>().DoubleValue();
    GL_CHECK(glBlendColor(red, green, blue, alpha));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendFunc)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum sfactor = info[0].As<Napi::Number>().Uint32Value();
    GLenum dfactor = info[1].As<Napi::Number>().Uint32Value();
    GL_CHECK(glBlendFunc(sfactor, dfactor));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendFuncSeparate)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum one = info[0].As<Napi::Number>().Uint32Value();
    GLenum two = info[1].As<Napi::Number>().Uint32Value();
    GLenum three = info[2].As<Napi::Number>().Uint32Value();
    GLenum four = info[3].As<Napi::Number>().Uint32Value();
    GL_CHECK(glBlendFuncSeparate(one, two, three, four));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendEquation)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum one = info[0].As<Napi::Number>().Uint32Value();
    GLenum two = info[1].As<Napi::Number>().Uint32Value();
    GL_CHECK(glBlendEquationSeparate(one, two));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(blendEquationSeparate)
{
    if( !CHECK_PARAM_LEGNTH(4) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum one = info[0].As<Napi::Number>().Uint32Value();
    GLenum two = info[1].As<Napi::Number>().Uint32Value();
    GLenum three = info[2].As<Napi::Number>().Uint32Value();
    GLenum four = info[4].As<Napi::Number>().Uint32Value();
    GL_CHECK(glBlendFuncSeparate(one, two, three, four));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(cullFace)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum mode = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glCullFace(mode));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getError)
{
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum err= glGetError();
    RECORD_TIME_END
    return Napi::Number::New(info.Env(), err);
    
}

DEFINE_VOID_METHOD(bindAttribLocation)
{
    if( !CHECK_PARAM_LEGNTH(3) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT

    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GLuint index = info[1].As<Napi::Number>().Uint32Value();
    std::string name = info[2].As<Napi::String>().Utf8Value();
    GL_CHECK(glBindAttribLocation(program->getId(), index, name.c_str()));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(polygonOffset)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat factor = info[0].As<Napi::Number>().FloatValue();
    GLfloat untis = info[1].As<Napi::Number>().FloatValue();
    GL_CHECK(glPolygonOffset(factor, untis));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(lineWidth)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat width = info[0].As<Napi::Number>().FloatValue();
    GL_CHECK(glLineWidth(width));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(sampleCoverage)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat value = info[0].As<Napi::Number>().FloatValue();
    GLboolean invert = info[1].As<Napi::Boolean>().Value();
    GL_CHECK(glSampleCoverage(value, invert));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(hint)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GLenum mode = info[1].As<Napi::Number>().Uint32Value();
    GL_CHECK(glHint(target, mode));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(isEnabled)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return  Napi::Boolean::New(info.Env(), false);
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum cap = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(GLboolean ret = glIsEnabled(cap));
    RECORD_TIME_END
    return Napi::Boolean::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(readPixels)
{
    if( !CHECK_PARAM_LEGNTH(7) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
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
    else
    {
        //TODO IsArray
    }
    GL_CHECK(glReadPixels(x, y, width, height, format, type, pixels));
    RECORD_TIME_END
}

DEFINE_VOID_METHOD(depthRange)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLfloat near = info[0].As<Napi::Number>().FloatValue();
    GLfloat far = info[1].As<Napi::Number>().FloatValue();
    GL_CHECK(glDepthRange(near, far));
    RECORD_TIME_END
    }

DEFINE_VOID_METHOD(generateMipmap)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLenum target = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glGenerateMipmap(target));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getAttachedShaders)
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    int maxCount = 32;
    int shaderCount = 0;
    GLuint shaders[maxCount];
    GL_CHECK(glGetAttachedShaders(program->getId(), maxCount, &shaderCount, shaders));
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
{
    if( !CHECK_PARAM_LEGNTH(1) )
        return;
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    GLuint index = info[0].As<Napi::Number>().Uint32Value();
    GL_CHECK(glDisableVertexAttribArray(index));
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getActiveAttrib)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GLuint programId = program->getId();
    GLint index = info[1].As<Napi::Number>().Int32Value();
    GLsizei length;
    glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
    GLchar *buffer = new (std::nothrow) GLchar[length];
    GLint size = 0;
    GLenum type = 0;
    GL_CHECK(glGetActiveAttrib(programId, index, length, NULL, &size, &type, buffer));
    Napi::Object activeInfoObj = WebGLActiveInfo::NewInstance(info.Env(), size, type, buffer);
    RECORD_TIME_END
    return activeInfoObj;
}

DEFINE_RETURN_VALUE_METHOD(getActiveUniform)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT
    WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
    GLuint programId = program->getId();
    GLint index = info[1].As<Napi::Number>().Int32Value();
    GLsizei length;
    glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
    GLchar *buffer = new (std::nothrow) GLchar[length];
    GLint size = 0;
    GLenum type = 0;
    GL_CHECK(glGetActiveUniform(programId, index, length, NULL, &size, &type, buffer));
    Napi::Object activeInfoObj = WebGLActiveInfo::NewInstance(info.Env(), size, type, buffer);
    RECORD_TIME_END
    return activeInfoObj;
}

DEFINE_RETURN_VALUE_METHOD(getUniform)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT

    //TODO 
    // glGetUniformiv()
    RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getVertexAttrib)
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    RECORD_TIME_BEGIN 
    EGL_MAKE_CURRENT

    GLuint index = info[0].As<Napi::Number>().Uint32Value();
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    if (pname == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)
    { // WebGLBuffer
        GLint params;
        GL_CHECK(glGetVertexAttribiv(index, pname, &params));
        return Napi::Number::New(info.Env(), params);
    }
    else if (pname == GL_VERTEX_ATTRIB_ARRAY_ENABLED ||
            pname == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED)
    { // GLBool
        GLint params;
        GL_CHECK(glGetVertexAttribiv(index, pname, &params));
        return Napi::Number::New(info.Env(), params);
    }
    else if (pname == GL_VERTEX_ATTRIB_ARRAY_SIZE ||
            pname == GL_VERTEX_ATTRIB_ARRAY_STRIDE)
    { // GLint
        GLint params;
        GL_CHECK(glGetVertexAttribiv(index, pname, &params));
        return Napi::Number::New(info.Env(), params);
    }
    else if (pname == GL_VERTEX_ATTRIB_ARRAY_TYPE)
    { // GLEnum
        GLint params;
        GL_CHECK(glGetVertexAttribiv(index, pname, &params));
        return Napi::Number::New(info.Env(), params);
    }
    else if (pname == GL_CURRENT_VERTEX_ATTRIB)
    { // 4元素 Float32Array
        GLfloat *data = new GLfloat[4];
        GL_CHECK(glGetVertexAttribfv(index, pname, data));
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
{
    if( !CHECK_PARAM_LEGNTH(2) )
        return info.Env().Null();
    
    GLuint index = info[0].As<Napi::Number>().Uint32Value();
    GLenum pname = info[1].As<Napi::Number>().Uint32Value();
    if (pname == GL_VERTEX_ATTRIB_ARRAY_POINTER)
    {
        RECORD_TIME_BEGIN 
        EGL_MAKE_CURRENT
        int *p;
        GL_CHECK(glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, (void **)&p));
        RECORD_TIME_END
        return Napi::Number::New(info.Env(), *p);
    }
    return Napi::Number::New(info.Env(), 0);
}

DEFINE_RETURN_VALUE_METHOD(isContextLost)
{
    RECORD_TIME_BEGIN 
    bool flag = false;
    if (!mRenderContext)
    {
        flag = true;
    }
    RECORD_TIME_END
    return Napi::Boolean::New(info.Env(), flag);
}

} // namespace NodeBinding