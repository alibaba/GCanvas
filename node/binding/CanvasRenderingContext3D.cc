#include "CanvasRenderingContext3D.h"
namespace NodeBinding
{
    Napi::FunctionReference Context3D::constructor;
    void Context3D::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);
        Napi::Function func =
            DefineClass(env,
                        "Context3D",
                        {

                        });
        constructor= Napi::Persistent(func);
    }
    Context3D::Context3D(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Context3D>(info)
    {
                                     
    }

    Napi::Object Context3D::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "context3d"));
        return obj;
    }

    Context3D::~Context3D()
    {
    }

} // namespace NodeBinding