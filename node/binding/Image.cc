#include "Image.h"
#include "NodeBindingUtil.h"
#include <iostream>
#include <stdlib.h>
namespace NodeBinding
{
Napi::FunctionReference Image::constructor;
Image::Image(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Image>(info)
{
}

void Image::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "Image",
                    {
                        InstanceAccessor("src", &Image::getSrc, &Image::setSrc),
                        InstanceAccessor("width", &Image::getWidth, nullptr),
                        InstanceAccessor("height", &Image::getHeight, nullptr),
                        InstanceAccessor("onload", &Image::getOnLoad, &Image::setOnLoad),
                        InstanceAccessor("onerror", &Image::getOnError, &Image::setOnError),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Image", func);
}

Napi::Value Image::getSrc(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), this->src);
}

void Image::setSrc(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    checkArgs(info, 1);
    this->src = value.As<Napi::String>().Utf8Value();
    if (mWorker)
    {
        mWorker->url = this->src;
        mWorker->Queue();
    }
}
Napi::Value Image::getOnLoad(const Napi::CallbackInfo &info)
{
    return this->onLoadCallback;
}
void Image::setOnLoad(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    checkArgs(info, 1);
    this->onLoadCallback = value.As<Napi::Function>();
    if (!mWorker)
    {
        mWorker = new ImageWorker(info.Env(),pixels, width, height);
    }
    mWorker->setOnLoadCallback(this->onLoadCallback);
}

void Image::setOnError(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    checkArgs(info, 1);
    this->onErrorCallback = value.As<Napi::Function>();
    if (!mWorker)
    {
        mWorker = new ImageWorker(info.Env(),pixels, width, height);
    }
    mWorker->setOnErrorCallback(onErrorCallback);
}

Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->width);
}
void Image::setWidth(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}
Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->height);
}
void Image::setHeight(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

int Image::getWidth()
{
    return this->width;
}
int Image::getHeight()
{
    return this->height;
}
Napi::Value Image::getOnError(const Napi::CallbackInfo &info)
{
    return this->onErrorCallback;
}

std::vector<unsigned char> &Image::getPixels()
{
    return this->pixels;
}

void ImageWorker::setOnErrorCallback(Napi::Function func)
{
    this->onErrorCallback = Napi::Persistent(func);
}

void ImageWorker::setOnLoadCallback(Napi::Function func)
{
    this->onLoadCallback = Napi::Persistent(func);
}

void ImageWorker::OnOK()
{
    if (content.size <= 0) //download fail
    {
        this->onErrorCallback.Call({Napi::String::New(Env(), "Dowdload image Error")});
    }
    else //download success
    {
        this->onLoadCallback.Call({Env().Undefined()});
    }
}

void ImageWorker::Execute()
{
    if (url.rfind("http", 0) == 0 || url.rfind("https", 0) == 0)
    {
        content.size = downloadImage(url, &content);
        if (content.size == -1)
        {
            free(content.memory);
            content.memory = nullptr;
            return;
        }
    }
    else
    { //本地文件
        content.size = readLocalImage(url, &content);
        if (content.size == -1)
        {
            free(content.memory);
            content.memory = nullptr;
            return;
        }
    }
    lodepng::decode(_pixels, _width, _height, (const unsigned char *)content.memory, content.size);
    free(content.memory);
    content.memory = nullptr;
}

}; // namespace NodeBinding