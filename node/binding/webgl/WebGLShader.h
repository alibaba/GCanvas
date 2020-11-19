#ifndef WEBGLSHADER_H
#define WEBGLSHADER_H
#include <napi.h>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLShader : public Napi::ObjectWrap<WebGLShader>
    {
    public:
        WebGLShader(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, const Napi::Value arg);
        inline GLuint getId() const
        {
            return this->mId;
        }

    private:
        GLuint mId = 0;
        static Napi::FunctionReference constructor;
    };
} // namespace NodeBinding
#endif