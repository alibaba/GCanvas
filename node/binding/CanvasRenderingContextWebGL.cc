#include "CanvasRenderingContextWebGL.h"
#include "./webGLRes/WebGLShader.h"
#include "./webGLRes/WebGLProgram.h"
#include "./webGLRes/WebGLBuffer.h"
#include "./webGLRes/WebGLTexture.h"
#include "./webGLRes/WebGLFrameBuffer.h"
#include "./webGLRes/WebGLRenderBuffer.h"
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
                            BINDING_OBJECT_METHOD(enable),

                            BINDING_OBJECT_METHOD(bindBuffer),
                            BINDING_OBJECT_METHOD(bufferData),

                            BINDING_OBJECT_METHOD(bindTexture),
                            BINDING_OBJECT_METHOD(renderbufferStorage),

                            BINDING_OBJECT_METHOD(shaderSource),
                            BINDING_OBJECT_METHOD(getShaderSource),
                            BINDING_OBJECT_METHOD(compileShader),
                            BINDING_OBJECT_METHOD(attachShader),
                            BINDING_OBJECT_METHOD(linkProgram),
                            BINDING_OBJECT_METHOD(useProgram),

                            BINDING_OBJECT_METHOD(getAttribLocation),
                            BINDING_OBJECT_METHOD(vertexAttribPointer),
                            BINDING_OBJECT_METHOD(enableVertexAttribArray),

                            BINDING_OBJECT_METHOD(bindFramebuffer),
                            BINDING_OBJECT_METHOD(checkFramebufferStatus),
                            BINDING_OBJECT_METHOD(framebufferRenderbuffer),
                            BINDING_OBJECT_METHOD(framebufferTexture2D),

                            BINDING_OBJECT_METHOD(drawElements),
                            BINDING_OBJECT_METHOD(drawArrays),
                            BINDING_OBJECT_METHOD(flush),
                            BINDING_OBJECT_METHOD(finish),
                            BINDING_OBJECT_METHOD(clear),

                            BINDING_OBJECT_METHOD(getShaderInfoLog),
                            BINDING_OBJECT_METHOD(getUniformLocation),

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
                            BINDING_OBJECT_METHOD(texImage2D),

                            BINDING_OBJECT_METHOD(stencilFunc),
                            BINDING_OBJECT_METHOD(stencilOp),
                            BINDING_OBJECT_METHOD(stencilMask),

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
                            BINDING_CONST_PROPERY(UNSIGNED_SHORT),
                            BINDING_CONST_PROPERY(FLOAT),

                            BINDING_CONST_PROPERY(FALSE),
                            BINDING_CONST_PROPERY(TRUE),
                            BINDING_CONST_PROPERY(ZERO),
                            BINDING_CONST_PROPERY(ONE),

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
    NodeBinding::checkArgs(info, 1);
    GLbitfield mask = info[0].As<Napi::Number>().Uint32Value();
    glClear(mask);
    RECORD_TIME_END
} // namespace NodeBinding
DEFINE_VOID_METHOD(clearColor)
NodeBinding::checkArgs(info, 4);
GLfloat red = info[0].As<Napi::Number>().FloatValue();
GLfloat green = info[1].As<Napi::Number>().FloatValue();
GLfloat blue = info[2].As<Napi::Number>().FloatValue();
GLfloat alpha = info[3].As<Napi::Number>().FloatValue();
glClearColor(red, green, blue, alpha);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(enable)
NodeBinding::checkArgs(info, 1);
GLenum cap = info[0].As<Napi::Number>().Uint32Value();
glEnable(cap);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createBuffer)
NodeBinding::checkArgs(info, 0);
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
NodeBinding::checkArgs(info, 3);
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

DEFINE_RETURN_VALUE_METHOD(createShader)
NodeBinding::checkArgs(info, 1);
Napi::Env env = info.Env();
GLenum shaderType = info[0].As<Napi::Number>().Uint32Value();
GLuint shaderId = glCreateShader(shaderType);
Napi::Object obj = WebGLShader::NewInstance(info.Env(), Napi::Number::New(env, shaderId));
RECORD_TIME_END
return obj;
}

DEFINE_VOID_METHOD(shaderSource)
NodeBinding::checkArgs(info, 2);
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
const char *shaderContent = shaderStr.c_str();
GLint shaderContentLen = shaderStr.size();
GLuint shaderId = shader->getId();
glShaderSource(shader->getId(), 1, &shaderContent, &shaderContentLen);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(compileShader)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
glCompileShader(shader->getId());
GLint compiled = 0;
RECORD_TIME_END
}

DEFINE_VOID_METHOD(attachShader)
NodeBinding::checkArgs(info, 2);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
glAttachShader(program->getId(), shader->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createProgram)
NodeBinding::checkArgs(info, 0);
GLuint programId = glCreateProgram();
RECORD_TIME_END
return WebGLProgram::NewInstance(info.Env(), Napi::Number::New(info.Env(), programId));
}

DEFINE_VOID_METHOD(linkProgram)
NodeBinding::checkArgs(info, 1);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glLinkProgram(program->getId());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(useProgram)
NodeBinding::checkArgs(info, 1);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glUseProgram(program->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getAttribLocation)
NodeBinding::checkArgs(info, 2);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
const char *name = info[1].As<Napi::String>().Utf8Value().data();
GLuint pos = glGetAttribLocation(program->getId(), name);
return Napi::Number::New(info.Env(), pos);
}

DEFINE_VOID_METHOD(viewport)
NodeBinding::checkArgs(info, 4);
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
int dpi = mRenderContext->getDpi();
glViewport(x *dpi, y *dpi, width *dpi, height *dpi);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(drawElements)
NodeBinding::checkArgs(info, 4);
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
GLsizei count = info[1].As<Napi::Number>().Int32Value();
GLenum type = info[2].As<Napi::Number>().Uint32Value();
GLuint offset = info[3].As<Napi::Number>().Uint32Value();
glDrawElements(mode, count, type, (GLvoid *)(intptr_t)offset);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(flush)
glFlush();
RECORD_TIME_END
}

DEFINE_VOID_METHOD(finish)
glFinish();
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttribPointer)
NodeBinding::checkArgs(info, 6);
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
NodeBinding::checkArgs(info, 1);
GLuint index = info[0].As<Napi::Number>().Uint32Value();
glEnableVertexAttribArray(index);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(scissor)
NodeBinding::checkArgs(info, 4);
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
int dpi = mRenderContext->getDpi();
glScissor(x *dpi, y *dpi, width *dpi, height *dpi);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(drawArrays)
NodeBinding::checkArgs(info, 3);
GLenum mode = info[0].As<Napi::Number>().Uint32Value();
GLint first = info[1].As<Napi::Number>().Int32Value();
GLint count = info[2].As<Napi::Number>().Int32Value();
glDrawArrays(mode, first, count);
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getShaderParameter)
NodeBinding::checkArgs(info, 2);
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLint ret = 0;
glGetShaderiv(shader->getId(), pname, &ret);
RECORD_TIME_END
return Napi::Number::New(info.Env(), ret);
}

DEFINE_RETURN_VALUE_METHOD(getShaderInfoLog)
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
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
glDeleteShader(shader->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getProgramParameter)
NodeBinding::checkArgs(info, 2);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
GLenum pname = info[1].As<Napi::Number>().Uint32Value();
GLint ret;
glGetProgramiv(program->getId(), pname, &ret);
RECORD_TIME_END
return Napi::Number::New(info.Env(), ret);
}

DEFINE_VOID_METHOD(deleteProgram)
NodeBinding::checkArgs(info, 1);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
glDeleteProgram(program->getId());
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getUniformLocation)
NodeBinding::checkArgs(info, 2);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
std::string shaderStr = info[1].As<Napi::String>().Utf8Value();
GLuint index = glGetUniformLocation(program->getId(), shaderStr.c_str());
RECORD_TIME_END
return Napi::Number::New(info.Env(), index);
}
DEFINE_VOID_METHOD(uniform1f)
NodeBinding::checkArgs(info, 2);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
glUniform1f(location, v1);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2f)
NodeBinding::checkArgs(info, 3);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
glUniform2f(location, v1, v2);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3f)
NodeBinding::checkArgs(info, 4);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
glUniform3f(location, v1, v2, v3);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4f)
NodeBinding::checkArgs(info, 5);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLfloat v1 = info[1].As<Napi::Number>().FloatValue();
GLfloat v2 = info[2].As<Napi::Number>().FloatValue();
GLfloat v3 = info[3].As<Napi::Number>().FloatValue();
GLfloat v4 = info[4].As<Napi::Number>().FloatValue();
glUniform4f(location, v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(colorMask)
NodeBinding::checkArgs(info, 4);
GLint v1, v2, v3, v4;
if (info[0].IsNumber())
{
    v1 = info[0].As<Napi::Number>().Int32Value();
}
else
{
    v1 = info[0].As<Napi::Boolean>().Value();
}
if (info[1].IsNumber())
{
    v2 = info[1].As<Napi::Number>().Int32Value();
}
else
{
    v2 = info[1].As<Napi::Boolean>().Value();
}
if (info[2].IsNumber())
{
    v3 = info[2].As<Napi::Number>().Int32Value();
}
else
{
    v3 = info[2].As<Napi::Boolean>().Value();
}
if (info[3].IsNumber())
{
    v4 = info[3].As<Napi::Number>().Int32Value();
}
else
{
    v4 = info[3].As<Napi::Boolean>().Value();
}
glColorMask(v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1i)
NodeBinding::checkArgs(info, 2);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLint v1 = info[1].As<Napi::Number>().Int32Value();
glUniform1i(location, v1);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2i)
NodeBinding::checkArgs(info, 3);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
glUniform2i(location, v1, v2);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3i)
NodeBinding::checkArgs(info, 4);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
GLint v3 = info[3].As<Napi::Number>().Int32Value();
glUniform3i(location, v1, v2, v3);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4i)
NodeBinding::checkArgs(info, 5);
GLuint location = info[0].As<Napi::Number>().Uint32Value();
GLint v1 = info[1].As<Napi::Number>().Int32Value();
GLint v2 = info[2].As<Napi::Number>().Int32Value();
GLint v3 = info[3].As<Napi::Number>().Int32Value();
GLint v4 = info[4].As<Napi::Number>().Int32Value();
glUniform4i(location, v1, v2, v3, v4);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform1fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform2fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform3fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform4fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform4fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniform1iv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform1iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform2iv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform2iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform3iv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform3iv);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(uniform4iv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallUniformFunc(info, glUniform4iv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix2fv)
NodeBinding::checkArgs(info, 3);
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix3fv)
NodeBinding::checkArgs(info, 3);
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(uniformMatrix4fv)
NodeBinding::checkArgs(info, 3);
parseTypeArrayAndCallUniformFunc(info, glUniformMatrix4fv);
RECORD_TIME_END
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformFloatPtr func)
{
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.ElementLength(), buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.ElementLength(), buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformIntPtr func)
{
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Int32Array buffer = array.As<Napi::Int32Array>();
        func(0, buffer.ElementLength(), buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Int32Array buffer = array.As<Napi::Int32Array>();
        func(0, buffer.ElementLength(), buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformMatrixPtr func)
{
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    GLboolean transpose = info[1].As<Napi::Boolean>().Value();
    if (info[2].IsTypedArray())
    {
        Napi::TypedArray array = info[2].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.ElementLength(), transpose, buffer.Data());
    }
    else if (info[2].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[2].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.ElementLength(), transpose, buffer.Data());
    }
}

void ContextWebGL::parseTypeArrayAndCallVertexFunc(const Napi::CallbackInfo &info, glVeterxFloatPtr func)
{
    GLuint location = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsTypedArray())
    {
        Napi::TypedArray array = info[1].As<Napi::TypedArray>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.Data());
    }
    else if (info[1].IsArrayBuffer())
    {
        Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
        Napi::Float32Array buffer = array.As<Napi::Float32Array>();
        func(0, buffer.Data());
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
NodeBinding::checkArgs(info, 2);
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
NodeBinding::checkArgs(info, 2);
GLenum type = info[0].As<Napi::Number>().Uint32Value();
GLuint param = 0;
if (info[1].IsBoolean())
{
    param = info[1].As<Napi::Boolean>().Value();
}
else
{
    param = info[1].As<Napi::Number>().Int32Value();
}
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

//TODO
DEFINE_VOID_METHOD(texImage2D)
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

DEFINE_VOID_METHOD(clearStencil)
CHECK_PARAM_LEGNTH(1)
GLint s = info[0].As<Napi::Number>().Int32Value();
glClearStencil(s);
RECORD_TIME_END
}

ContextWebGL::~ContextWebGL()
{
    this->mRenderContext = nullptr;
}

} // namespace NodeBinding