/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
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
        mWorker = new ImageWorker(info.Env(), pixels, width, height);
    }
    mWorker->setOnLoadCallback(this->onLoadCallback);
}

void Image::setOnError(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    checkArgs(info, 1);
    this->onErrorCallback = value.As<Napi::Function>();
    if (!mWorker)
    {
        mWorker = new ImageWorker(info.Env(), pixels, width, height);
    }
    mWorker->setOnErrorCallback(onErrorCallback);
}

Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->width);
}
Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->height);
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

}; // namespace NodeBinding