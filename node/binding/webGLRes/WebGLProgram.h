#ifndef WEBGLPROGRAM_H
#define WEBGLPROGRAM_H
#include <napi.h>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLProgram : public Napi::ObjectWrap<WebGLProgram>
    {
    public:
        WebGLProgram(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, const Napi::Value arg);
        GLuint getId() const
        {
            return this->mId;
        }

    private:
        GLuint mId = 0;
        static Napi::FunctionReference constructor;
    };
} // namespace NodeBinding
#endif