#ifndef CONTEXT3D_H
#define CONTEXT3D_H
#include <napi.h>
#include "GRenderContext.h"

#define DEFINE_CONST_PROPERY_GET_FUNCTION(propertyName)           \
    Napi::Value get##propertyName(const Napi::CallbackInfo &info) \
    {                                                             \
        return Napi::Number::New(info.Env(), GL_##propertyName);  \
    }

#define BINDING_CONST_PROPERY(propertyName) \
    InstanceAccessor(#propertyName, &ContextWebGL::get##propertyName, nullptr)

#define BINDING_OBJECT_METHOD(methodName) \
    InstanceMethod(#methodName, &ContextWebGL::methodName)

namespace NodeBinding
{
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
        void flush(const Napi::CallbackInfo &info);
        void finish(const Napi::CallbackInfo &info);
        void vertexAttribPointer(const Napi::CallbackInfo &info);
        void enableVertexAttribArray(const Napi::CallbackInfo &info);
        void scissor(const Napi::CallbackInfo &info);
        static Napi::FunctionReference constructor;
        DEFINE_CONST_PROPERY_GET_FUNCTION(COLOR_BUFFER_BIT)
        DEFINE_CONST_PROPERY_GET_FUNCTION( DEPTH_BUFFER_BIT)
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

        DEFINE_CONST_PROPERY_GET_FUNCTION(ARRAY_BUFFER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(STATIC_DRAW)
        DEFINE_CONST_PROPERY_GET_FUNCTION(ELEMENT_ARRAY_BUFFER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(VERTEX_SHADER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(FRAGMENT_SHADER)
        DEFINE_CONST_PROPERY_GET_FUNCTION(UNSIGNED_SHORT)
        DEFINE_CONST_PROPERY_GET_FUNCTION(FLOAT)

        std::shared_ptr<GRenderContext> mRenderContext = nullptr;
    };
} // namespace NodeBinding
#endif