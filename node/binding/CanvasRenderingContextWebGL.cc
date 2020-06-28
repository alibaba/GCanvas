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
                        {BINDING_OBJECT_METHOD(createBuffer),
                         BINDING_OBJECT_METHOD(createProgram),
                         BINDING_OBJECT_METHOD(createShader),
                         BINDING_OBJECT_METHOD(deleteShader),

                         BINDING_OBJECT_METHOD(viewport),
                         BINDING_OBJECT_METHOD(scissor),
                         BINDING_OBJECT_METHOD(clearColor),
                         BINDING_OBJECT_METHOD(colorMask),
                         BINDING_OBJECT_METHOD(enable),

                         BINDING_OBJECT_METHOD(bindBuffer),
                         BINDING_OBJECT_METHOD(bufferData),

                         BINDING_OBJECT_METHOD(shaderSource),
                         BINDING_OBJECT_METHOD(getShaderSource),
                         BINDING_OBJECT_METHOD(compileShader),
                         BINDING_OBJECT_METHOD(attachShader),
                         BINDING_OBJECT_METHOD(linkProgram),
                         BINDING_OBJECT_METHOD(useProgram),

                         BINDING_OBJECT_METHOD(getAttribLocation),
                         BINDING_OBJECT_METHOD(vertexAttribPointer),
                         BINDING_OBJECT_METHOD(enableVertexAttribArray),

                         BINDING_OBJECT_METHOD(drawElements),
                         BINDING_OBJECT_METHOD(drawArrays),
                         BINDING_OBJECT_METHOD(flush),
                         BINDING_OBJECT_METHOD(finish),
                         BINDING_OBJECT_METHOD(clear),
                         BINDING_OBJECT_METHOD(getShaderParameter),
                         BINDING_OBJECT_METHOD(getShaderInfoLog),
                         BINDING_OBJECT_METHOD(getProgramParameter),
                         BINDING_OBJECT_METHOD(deleteProgram),
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

                         BINDING_CONST_PROPERY(ELEMENT_ARRAY_BUFFER),
                         BINDING_CONST_PROPERY(STATIC_DRAW),
                         BINDING_CONST_PROPERY(ARRAY_BUFFER),
                         BINDING_CONST_PROPERY(COLOR_BUFFER_BIT),
                         BINDING_CONST_PROPERY(DEPTH_BUFFER_BIT),
                         BINDING_CONST_PROPERY(VERTEX_SHADER),
                         BINDING_CONST_PROPERY(FRAGMENT_SHADER),
                         BINDING_CONST_PROPERY(DEPTH_TEST),
                         BINDING_CONST_PROPERY(SCISSOR_TEST),
                         BINDING_CONST_PROPERY(STENCIL_TEST),
                         BINDING_CONST_PROPERY(POINTS),
                         BINDING_CONST_PROPERY(COMPILE_STATUS),
                         BINDING_CONST_PROPERY(LINK_STATUS),
                         BINDING_CONST_PROPERY(TRIANGLES),
                         BINDING_CONST_PROPERY(LINE_STRIP),
                         BINDING_CONST_PROPERY(LINES),
                         BINDING_CONST_PROPERY(LINE_LOOP),
                         BINDING_CONST_PROPERY(TRIANGLE_FAN),
                         BINDING_CONST_PROPERY(TRIANGLE_STRIP),

                         BINDING_CONST_PROPERY(FALSE),
                         BINDING_CONST_PROPERY(TRUE),
                         BINDING_CONST_PROPERY(UNSIGNED_SHORT),
                         BINDING_CONST_PROPERY(FLOAT)});
        constructor = Napi::Persistent(func);
    }
    ContextWebGL::ContextWebGL(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ContextWebGL>(info)
    {
        // renderFrame();
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
GLboolean v1 = info[0].As<Napi::Boolean>().Value();
GLboolean v2 = info[1].As<Napi::Boolean>().Value();
GLboolean v3 = info[2].As<Napi::Boolean>().Value();
GLboolean v4 = info[3].As<Napi::Boolean>().Value();
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
        func(0,buffer.Data());
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
parseTypeArrayAndCallVertexFunc(info,glVertexAttrib1fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib2fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info,glVertexAttrib2fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib3fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info,glVertexAttrib3fv);
RECORD_TIME_END
}

DEFINE_VOID_METHOD(vertexAttrib4fv)
NodeBinding::checkArgs(info, 2);
parseTypeArrayAndCallVertexFunc(info,glVertexAttrib4fv);
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

ContextWebGL::~ContextWebGL()
{
    this->mRenderContext = nullptr;
}

} // namespace NodeBinding