#ifndef CONTEXT3D_H
#define CONTEXT3D_H
#include <napi.h>
namespace NodeBinding
{
    class Context3D : public Napi::ObjectWrap<Context3D>
    {
    public:
        static void Init(Napi::Env env);
        Context3D(const Napi::CallbackInfo &info);
        static Napi::Object NewInstance(Napi::Env env);
        virtual ~Context3D();

    private:
        static Napi::FunctionReference constructor;
    };
} // namespace NodeBinding
#endif