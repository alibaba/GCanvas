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

GLuint simpleTriangleProgram = 0;
GLuint vPosition = 0;
static const char glVertexShader[] =
    "attribute vec4 vPosition;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char glFragmentShader[] =
    "void main()\n"
    "{\n"
    "  gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
    "}\n";

GLuint loadShader(GLenum shaderType, const char *shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char *buf = (char *)malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char *vertexSource, const char *fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
    {
        return 0;
    }
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        return 0;
    }
    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char *buf = (char *)malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

bool setupGraphics(int w, int h)
{
    simpleTriangleProgram = createProgram(glVertexShader, glFragmentShader);
    if (!simpleTriangleProgram)
    {
        return false;
    }
    vPosition = glGetAttribLocation(simpleTriangleProgram, "vPosition");
    glViewport(0, 0, w, h);
    return true;
}

const GLfloat triangleVertices[] = {
    0.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, -1.0f};
void renderFrame()
{
    printf("render frame called  \n");
    setupGraphics(800, 800);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(simpleTriangleProgram);
    GLuint vertexBuffer = -1;
    glGenBuffers(1, &vertexBuffer);
    printf("the vertexBuffer is %d \n", vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 6, triangleVertices, GL_STATIC_DRAW);
    GLint a_Position = glGetAttribLocation(simpleTriangleProgram, "vPosition");
    printf("the a_Position value is %d \n", a_Position);
    glVertexAttribPointer(a_Position, 2, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(a_Position);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
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

                         BINDING_OBJECT_METHOD(uniform4f),

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
    Napi::Float32Array buffer = array.As<Napi::Float32Array>();
    glBufferData(target, buffer.ByteLength(), buffer.Data(), usage);
}
else if (info[1].IsArrayBuffer())
{
    Napi::ArrayBuffer array = info[1].As<Napi::ArrayBuffer>();
    Napi::Uint8Array buffer = array.As<Napi::Uint8Array>();
    glBufferData(target, array.ByteLength(), buffer.data(), usage);
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
int  dpi=mRenderContext->getDpi();
glViewport(x * dpi, y * dpi, width * dpi, height * dpi);
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
GLsizei height = info[3].As<Napi::Number>().Int32Value();\
int dpi=mRenderContext->getDpi();
glScissor(x * dpi, y * dpi, width * dpi, height * dpi);
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