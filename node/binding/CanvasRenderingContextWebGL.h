#ifndef CONTEXT3D_H
#define CONTEXT3D_H
#include <napi.h>
#include "GRenderContext.h"
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
        static Napi::FunctionReference constructor;
        std::shared_ptr<GRenderContext> mRenderContext = nullptr;
    };
} // namespace NodeBinding
#endif