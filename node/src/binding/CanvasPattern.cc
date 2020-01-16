#include "CanvasPattern.h"
#include "NodeBindingUtil.h"
namespace NodeBinding
{
Napi::FunctionReference ImagePattern::constructor;

ImagePattern::ImagePattern(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ImagePattern>(info)
{
    if (info[0].IsNull())
    {
        this->repetition = "repeat";
        return;
    }
    this->repetition = info[0].As<Napi::String>().Utf8Value();
    if (this->repetition != "" &&
        this->repetition != "repeat" &&
        this->repetition != "repeat-x" && this->repetition != "repeat-y" &&
        this->repetition != "no-repeat")
    {
        throwError(info, "repetition value wrong");
    }
    if (this->repetition == "")
    {
        this->repetition = "repeat";
    }
}

Napi::Object ImagePattern::NewInstance(Napi::Env env, const Napi::Value arg)
{
    Napi::Object obj = constructor.New({arg});
    obj.Set("name", Napi::String::New(env, "imagePattern"));
    return obj;
}

void ImagePattern::Init(Napi::Env env)
{
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "canvasPattern",
                    {

                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

const std::string &ImagePattern::getRepetition()
{
    return this->repetition;
}
} // namespace NodeBinding