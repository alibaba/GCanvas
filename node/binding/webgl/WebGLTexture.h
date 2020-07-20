#ifndef WEBGLTEXTURE_H
#define WEBGLTEXTURE_H
#include <napi.h>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLTexture : public Napi::ObjectWrap<WebGLTexture>
    {
    public:
        WebGLTexture(const Napi::CallbackInfo &info);
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