/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
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
    hasImageDataWrite = true;
    if (this->mImageDataRef.IsEmpty())
    {
        Napi::Array ret = Napi::Array::New(info.Env(), this->pixels.size());

        if (!this->pixels.empty())
        {
            for (int i = 0; i < pixels.size(); i++)
            {
                ret.Set(i, Napi::Number::New(info.Env(), pixels[i]));
            }
        }
        this->mImageDataRef = Napi::ObjectReference::New(ret);
        return ret;
    }
    else
    {
        return this->mImageDataRef.Value();
    }
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
    if (!this->mImageDataRef.IsEmpty() && hasImageDataWrite)
    {
        Napi::Array ret = this->mImageDataRef.Value().As<Napi::Array>();
        for (int i = 0; i < this->pixels.size(); i++)
        {
            this->pixels[i] = ret.Get(i).As<Napi::Number>().Int32Value();
        }
        hasImageDataWrite = false;
    }
    return this->pixels;
}
} // namespace NodeBinding