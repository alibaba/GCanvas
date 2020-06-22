#include "CanvasRenderingContextWebGL.h"
#include "./webGLRes/WebGLShader.h"
#include "./webGLRes/WebGLProgram.h"
#include "./webGLRes/WebGLBuffer.h"
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
        // printf("the function :  " #methodName " is  called \n");                            \
        \ 


#define DEFINE_RETURN_VALUE_METHOD(methodName)               \
    \ 
     Napi::Value                                             \
    ContextWebGL::methodName(const Napi::CallbackInfo &info) \
    {                                                        \
        RECORD_TIME_BEGIN                                    \
        mRenderContext->makeCurrent();                       \
        // printf("the function : " #methodName " is  called \n");                         \
            \  

namespace NodeBinding
{
    Napi::FunctionReference ContextWebGL::constructor;
    void ContextWebGL::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);
        Napi::Function func =
            DefineClass(env,
                        "Context3D",
                        {BINDING_OBJECT_METHOD(createBuffer),
                         BINDING_OBJECT_METHOD(createProgram),
                         BINDING_OBJECT_METHOD(createShader),

                         BINDING_OBJECT_METHOD(viewport),
                         BINDING_OBJECT_METHOD(scissor),
                         BINDING_OBJECT_METHOD(clearColor),
                         BINDING_OBJECT_METHOD(enable),

                         BINDING_OBJECT_METHOD(bindBuffer),
                         BINDING_OBJECT_METHOD(bufferData),

                         BINDING_OBJECT_METHOD(shaderSource),
                         BINDING_OBJECT_METHOD(compileShader),
                         BINDING_OBJECT_METHOD(attachShader),
                         BINDING_OBJECT_METHOD(linkProgram),
                         BINDING_OBJECT_METHOD(useProgram),

                         BINDING_OBJECT_METHOD(getAttribLocation),
                         BINDING_OBJECT_METHOD(vertexAttribPointer),
                         BINDING_OBJECT_METHOD(enableVertexAttribArray),

                         BINDING_OBJECT_METHOD(drawElements),
                         BINDING_OBJECT_METHOD(flush),
                         BINDING_OBJECT_METHOD(finish),
                         BINDING_OBJECT_METHOD(clear),

                         BINDING_CONST_PROPERY(ELEMENT_ARRAY_BUFFER),
                         BINDING_CONST_PROPERY(STATIC_DRAW),
                         BINDING_CONST_PROPERY(ARRAY_BUFFER),
                         BINDING_CONST_PROPERY(COLOR_BUFFER_BIT),
                         BINDING_CONST_PROPERY(VERTEX_SHADER),
                         BINDING_CONST_PROPERY(FRAGMENT_SHADER),
                         BINDING_CONST_PROPERY(DEPTH_TEST),
                         BINDING_CONST_PROPERY(SCISSOR_TEST),
                         BINDING_CONST_PROPERY(STENCIL_TEST),
                         BINDING_CONST_PROPERY(POINTS),
                         BINDING_CONST_PROPERY(TRIANGLES),
                         BINDING_CONST_PROPERY(LINE_STRIP),
                         BINDING_CONST_PROPERY(LINES),
                         BINDING_CONST_PROPERY(LINE_LOOP),
                         BINDING_CONST_PROPERY(TRIANGLE_FAN),
                         BINDING_CONST_PROPERY(TRIANGLE_STRIP),

                         BINDING_CONST_PROPERY(UNSIGNED_SHORT),
                         BINDING_CONST_PROPERY(FLOAT)});
        constructor = Napi::Persistent(func);
    }
    ContextWebGL::ContextWebGL(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ContextWebGL>(info)
    {
    }

    Napi::Object ContextWebGL::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "context3d"));
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
GLenum cap = info[0].As<Napi::Number>().Int32Value();
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
GLenum type = info[0].As<Napi::Number>().Int32Value();
GLuint bufferId = 0;
if (info[1].IsNull())
{
    bufferId = 0;
}
else
{
    WebGLBuffer *buffer = Napi::ObjectWrap<WebGLBuffer>::Unwrap(info[1].As<Napi::Object>());
    bufferId = buffer->getId();
}
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
GLenum target = info[0].As<Napi::Number>().Int32Value();
GLenum usage = info[2].As<Napi::Number>().Int32Value();
if (info[1].IsTypedArray())
{
    Napi::TypedArray array = info[1].As<Napi::TypedArray>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferData(target, array.ByteLength(), buffer, usage);
}
else if (info[1].IsArrayBuffer())
{
    Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferData(target, array.ByteLength(), buffer, usage);
}
else if (info[1].IsNumber())
{
    GLuint size = info[1].As<Napi::Number>().Int32Value();
    // glBufferData(target, size, usage);
}
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(createShader)
NodeBinding::checkArgs(info, 1);
Napi::Env env = info.Env();
GLuint shaderType = info[0].As<Napi::Number>().Int32Value();
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
glShaderSource(shader->getId(), 1, &shaderContent, nullptr);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(compileShader)
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[0].As<Napi::Object>());
glCompileShader(shader->getId());
RECORD_TIME_END
}

DEFINE_VOID_METHOD(attachShader)
NodeBinding::checkArgs(info, 2);
WebGLProgram *program = Napi::ObjectWrap<WebGLProgram>::Unwrap(info[0].As<Napi::Object>());
WebGLShader *shader = Napi::ObjectWrap<WebGLShader>::Unwrap(info[1].As<Napi::Object>());
glAttachShader(program->getId(), shader->getId());
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
GLuint handle = glGetAttribLocation(program->getId(), name);
return Napi::Number::New(info.Env(), handle);
}

DEFINE_VOID_METHOD(viewport)
NodeBinding::checkArgs(info, 4);
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
// printf("the x y width height is %d %d %d %d \n",x,y,width,height);
glViewport(x, y, width, height);
RECORD_TIME_END
}
DEFINE_VOID_METHOD(drawElements)
NodeBinding::checkArgs(info, 4);
GLenum mode = info[0].As<Napi::Number>().Int32Value();
GLsizei count = info[1].As<Napi::Number>().Int32Value();
GLenum type = info[2].As<Napi::Number>().Int32Value();
GLuint offset = info[3].As<Napi::Number>().Int32Value();
glDrawElements(mode, count, type, (char *)(NULL + offset));
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
GLuint handle = info[0].As<Napi::Number>().Int32Value();
GLint size = info[1].As<Napi::Number>().Int32Value();
GLenum type = info[2].As<Napi::Number>().Int32Value();
GLboolean isNormalized = info[3].As<Napi::Boolean>().Value();
GLuint stride = info[4].As<Napi::Number>().Int32Value();
GLuint offset = info[5].As<Napi::Number>().Int32Value();
printf("vertexAttribPointer is %d ,%d ,%d ,%d ,%d ,%d  \n", handle, size, type, isNormalized, stride, offset);
glVertexAttribPointer(handle, size, type, isNormalized, stride, (char *)(NULL + offset));
RECORD_TIME_END
}

DEFINE_VOID_METHOD(enableVertexAttribArray)
NodeBinding::checkArgs(info, 1);
GLuint handle = info[0].As<Napi::Number>().Int32Value();
glEnableVertexAttribArray(handle);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(scissor)
NodeBinding::checkArgs(info, 4);
GLint x = info[0].As<Napi::Number>().Int32Value();
GLint y = info[1].As<Napi::Number>().Int32Value();
GLsizei width = info[2].As<Napi::Number>().Int32Value();
GLsizei height = info[3].As<Napi::Number>().Int32Value();
glScissor(x, y, width, height);
RECORD_TIME_END
}

ContextWebGL::~ContextWebGL()
{
    this->mRenderContext = nullptr;
}

} // namespace NodeBinding