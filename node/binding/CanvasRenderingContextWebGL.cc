#include "CanvasRenderingContextWebGL.h"
namespace NodeBinding
{
    Napi::FunctionReference ContextWebGL::constructor;
    void ContextWebGL::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);
        Napi::Function func =
            DefineClass(env,
                        "Context3D",
                        {InstanceMethod("clear", &ContextWebGL::clear),
                         InstanceMethod("clearColor", &ContextWebGL::clearColor),
                         InstanceMethod("enable", &ContextWebGL::enable)});
        constructor = Napi::Persistent(func);
    }
    ContextWebGL::ContextWebGL(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ContextWebGL>(info)
    {
    }

    Napi::Object ContextWebGL::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "context3d"));
        return obj;
    }

    void ContextWebGL::clear(const Napi::CallbackInfo &info)
    {
    }
    void ContextWebGL::clearColor(const Napi::CallbackInfo &info)
    {
    }
    void ContextWebGL::enable(const Napi::CallbackInfo &info)
    {
    }

    ContextWebGL::~ContextWebGL()
    {
    }

} // namespace NodeBinding