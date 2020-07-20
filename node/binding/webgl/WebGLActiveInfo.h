#ifndef WebGLACTIVERINFO_H
#define WebGLACTIVERINFO_H
#include <napi.h>
#include <string>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLActiveInfo : public Napi::ObjectWrap<WebGLActiveInfo>
    {
    public:
        WebGLActiveInfo(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, GLuint size, GLuint type, GLchar *buffer);

    private:
        Napi::Value getName(const Napi::CallbackInfo &info)
        {
            return Napi::String::New(info.Env(), this->mName);
        }
        Napi::Value getSize(const Napi::CallbackInfo &info)
        {
            return Napi::Number::New(info.Env(), this->mSize);
        }
        Napi::Value getType(const Napi::CallbackInfo &info)
        {
            return Napi::Number::New(info.Env(), this->mType);
        }
        static Napi::FunctionReference constructor;
        GLint mType;
        GLint mSize;
        std::string mName;
    };
} // namespace NodeBinding
#endif