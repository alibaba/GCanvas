#ifndef CONTEXT3D_H
#define CONTEXT3D_H
#include <napi.h>
#include "GRenderContext.h"

#define DEFINE_CONST_PROPERY_GET_FUNCTION(propertyName)           \
    Napi::Value get##propertyName(const Napi::CallbackInfo &info) \
    {                                                             \
        return Napi::Number::New(info.Env(), GL_##propertyName);  \
    }

#define CHECK_PARAM_LEGNTH(length) \
    NodeBinding::checkArgs(info, length);

#define BINDING_CONST_PROPERY(propertyName) \
    InstanceAccessor(#propertyName, &ContextWebGL::get##propertyName, nullptr)

#define BINDING_OBJECT_METHOD(methodName) \
    InstanceMethod(#methodName, &ContextWebGL::methodName)

namespace NodeBinding
{
    typedef void (*glUniformFloatPtr)(GLint location, GLsizei count, const GLfloat *value);
    typedef void (*glUniformIntPtr)(GLint location, GLsizei count, const GLint *value);
    typedef void (*glUniformMatrixPtr)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (*glVeterxFloatPtr)(GLuint index, const GLfloat *value);
    class ContextWebGL : public Napi::ObjectWrap<ContextWebGL>
    {
    public:
        static void Init(Napi::Env env);
        ContextWebGL(const Napi::CallbackInfo &info);
        static Napi::Object NewInstance(Napi::Env env);
        virtual ~ContextWebGL();
        void inline setRenderContext(std::shared_ptr<GRenderContext> renderContext)
        {
            this->mRenderContext = renderContext;
        }

    private:
        void clear(const Napi::CallbackInfo &info);
        void clearColor(const Napi::CallbackInfo &info);
        void enable(const Napi::CallbackInfo &info);
        void bindBuffer(const Napi::CallbackInfo &info);
        void bufferData(const Napi::CallbackInfo &info);
        Napi::Value createBuffer(const Napi::CallbackInfo &info);
        Napi::Value createShader(const Napi::CallbackInfo &info);
        void shaderSource(const Napi::CallbackInfo &info);
        void compileShader(const Napi::CallbackInfo &info);
        Napi::Value createProgram(const Napi::CallbackInfo &info);
        void attachShader(const Napi::CallbackInfo &info);
        void linkProgram(const Napi::CallbackInfo &info);
        void useProgram(const Napi::CallbackInfo &info);
        Napi::Value getAttribLocation(const Napi::CallbackInfo &info);
        void viewport(const Napi::CallbackInfo &info);
        void drawElements(const Napi::CallbackInfo &info);
        void drawArrays(const Napi::CallbackInfo &info);
        void flush(const Napi::CallbackInfo &info);
        void finish(const Napi::CallbackInfo &info);
        void vertexAttribPointer(const Napi::CallbackInfo &info);
        void enableVertexAttribArray(const Napi::CallbackInfo &info);
        void scissor(const Napi::CallbackInfo &info);
        Napi::Value getShaderParameter(const Napi::CallbackInfo &info);
        Napi::Value getShaderInfoLog(const Napi::CallbackInfo &info);
        void deleteShader(const Napi::CallbackInfo &info);
        Napi::Value getProgramParameter(const Napi::CallbackInfo &info);
        void deleteProgram(const Napi::CallbackInfo &info);
        Napi::Value getUniformLocation(const Napi::CallbackInfo &info);

        void uniform1f(const Napi::CallbackInfo &info);
        void uniform2f(const Napi::CallbackInfo &info);
        void uniform3f(const Napi::CallbackInfo &info);
        void uniform4f(const Napi::CallbackInfo &info);

        void uniform1i(const Napi::CallbackInfo &info);
        void uniform2i(const Napi::CallbackInfo &info);
        void uniform3i(const Napi::CallbackInfo &info);
        void uniform4i(const Napi::CallbackInfo &info);

        void uniform1fv(const Napi::CallbackInfo &info);
        void uniform2fv(const Napi::CallbackInfo &info);
        void uniform3fv(const Napi::CallbackInfo &info);
        void uniform4fv(const Napi::CallbackInfo &info);

        void uniform1iv(const Napi::CallbackInfo &info);
        void uniform2iv(const Napi::CallbackInfo &info);
        void uniform3iv(const Napi::CallbackInfo &info);
        void uniform4iv(const Napi::CallbackInfo &info);

        void uniformMatrix2fv(const Napi::CallbackInfo &info);
        void uniformMatrix3fv(const Napi::CallbackInfo &info);
        void uniformMatrix4fv(const Napi::CallbackInfo &info);

        void vertexAttrib1f(const Napi::CallbackInfo &info);
        void vertexAttrib2f(const Napi::CallbackInfo &info);
        void vertexAttrib3f(const Napi::CallbackInfo &info);
        void vertexAttrib4f(const Napi::CallbackInfo &info);

        void vertexAttrib1fv(const Napi::CallbackInfo &info);
        void vertexAttrib2fv(const Napi::CallbackInfo &info);
        void vertexAttrib3fv(const Napi::CallbackInfo &info);
        void vertexAttrib4fv(const Napi::CallbackInfo &info);

        void colorMask(const Napi::CallbackInfo &info);
        Napi::Value getShaderSource(const Napi::CallbackInfo &info);
        void parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformFloatPtr func);
        void parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformIntPtr func);
        void parseTypeArrayAndCallUniformFunc(const Napi::CallbackInfo &info, glUniformMatrixPtr func);
        void parseTypeArrayAndCallVertexFunc(const Napi::CallbackInfo &info, glVeterxFloatPtr func);
        static Napi::FunctionReference constructor;
        DEFINE_CONST_PROPERY_GET_FUNCTION(COLOR_BUFFER_BIT)
        DEFINE_CONST_PROPERY_GET_FUNCTION(DEPTH_BUFFER_BIT)
        DEFINE_CONST_PROPERY_GET_FUNCTION(TRIANGLES)
        DEFINE_CONST_PROPERY_GET_FUNCTION(POINTS)
        DEFINE_CONST_PROPERY_GET_FUNCTION(LINE_STRIP)
        DEFINE_CONST_PROPERY_GET_FUNCTION(LINES)
        DEFINE_CONST_PROPERY_GET_FUNCTION(LINE_LOOP)
        DEFINE_CONST_PROPERY_GET_FUNCTION(TRIANGLE_FAN)
        DEFINE_CONST_PROPERY_GET_FUNCTION(TRIANGLE_STRIP)

        DEFINE_CONST_PROPERY_GET_FUNCTION(DEPTH_TEST)
        DEFINE_CONST_PROPERY_GET_FUNCTION(SCISSOR_TEST)
        DEFINE_CONST_PROPERY_GET_FUNCTION(STENCIL_TEST)
        DEFINE_CONST_PROPERY_GET_FUNCTION(COMPILE_STATUS)
        DEFINE_CONST_PROPERY_GET_FUNCTION(ARRAY_BUFFER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(STATIC_DRAW)
        DEFINE_CONST_PROPERY_GET_FUNCTION(ELEMENT_ARRAY_BUFFER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(VERTEX_SHADER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(FRAGMENT_SHADER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(UNSIGNED_SHORT)
        DEFINE_CONST_PROPERY_GET_FUNCTION(FLOAT)

        DEFINE_CONST_PROPERY_GET_FUNCTION(FALSE)
        DEFINE_CONST_PROPERY_GET_FUNCTION(TRUE)
        DEFINE_CONST_PROPERY_GET_FUNCTION(LINK_STATUS)

        std::shared_ptr<GRenderContext> mRenderContext = nullptr;
    };
} // namespace NodeBinding
#endif