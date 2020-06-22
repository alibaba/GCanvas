#ifndef WEBGLBUFFER_H
#define WEBGLBUFFER_H
#include <napi.h>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLBuffer : public Napi::ObjectWrap<WebGLBuffer>
    {
    public:
        WebGLBuffer(const Napi::CallbackInfo &info);
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