#ifndef  WebGLUniformLocation_H
#define  WebGLUniformLocation_H
#include <napi.h>
#include <string>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLUniformLocation : public Napi::ObjectWrap<WebGLUniformLocation>
    {
    public:
        WebGLUniformLocation(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, GLuint index);
        inline GLuint getIndex(){
            return this->mIndex;
        }
    private:

        static Napi::FunctionReference constructor;
        GLuint mIndex;
    };
} // namespace NodeBinding
#endif