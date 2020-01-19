#include "ImageData.h"

namespace NodeBinding
{
Napi::FunctionReference ImageData::constructor;

ImageData::ImageData(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ImageData>(info)
{
    this->width = info[0].As<Napi::Number>().Int32Value();
    this->height = info[1].As<Napi::Number>().Int32Value();
    this->pixels.resize(4 * width * height);
}

void ImageData::Init(Napi::Env env)
{
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "ImageData",
                    {
                        InstanceAccessor("data", &ImageData::getData, nullptr),
                        InstanceAccessor("width", &ImageData::getWidth, nullptr),
                        InstanceAccessor("height", &ImageData::getHeight, nullptr),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object ImageData::NewInstance(Napi::Env env, const Napi::Value width, const Napi::Value height)
{
    Napi::Object obj = constructor.New({width, height});
    obj.Set("name", Napi::String::New(env, "imageData"));
    return obj;
}

Napi::Value ImageData::getData(const Napi::CallbackInfo &info)
{

}

Napi::Value ImageData::getWidth(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->width);
}

Napi::Value ImageData::getHeight(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->height);
}

int ImageData::getWidth()
{
    return this->width;
}
int ImageData::getHeight()
{
    return this->height;
}

std::vector<u_int8_t> &ImageData::getPixles()
{
    return this->pixels;
}
} // namespace NodeBinding