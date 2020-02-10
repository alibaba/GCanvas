#ifndef CANVAS_H
#define CANVAS_H
#include "GRenderContext.h"
#include "CanvasRenderingContext2D.h"
#include "CanvasGradient.h"
#include "ImageData.h"
#include "CanvasPattern.h"
#include <napi.h>
#include <memory>
namespace NodeBinding
{
class Canvas : public Napi::ObjectWrap<Canvas>
{
public:
    static void Init(Napi::Env env);
    Canvas(const Napi::CallbackInfo &info);
    virtual ~Canvas();
    static Napi::Object NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2);
    Napi::ObjectReference mRef;
private:
    static Napi::FunctionReference constructor;
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
    Napi::Value getContext(const Napi::CallbackInfo &info);
    Napi::ObjectReference context2dRef;

    void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    void setHeight(const Napi::CallbackInfo &info, const Napi::Value &value);
    void createPNG(const Napi::CallbackInfo &info);

    int mWidth = 0;
    int mHeight = 0;
    std::shared_ptr<GRenderContext> mRenderContext;
};
} // namespace NodeBinding
#endif