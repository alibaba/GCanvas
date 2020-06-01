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
    if (this->onLoadCallback)
    {
        this->onLoadCallback.Call({Env().Undefined()});
    }
}

void ImageWorker::OnError(const Napi::Error &e)
{
    if (this->onErrorCallback)
    {
        this->onErrorCallback.Call({Napi::String::New(Env(), e.Message())});
    }
}

void ImageWorker::Execute()
{
    if (url.rfind("http", 0) == 0 || url.rfind("https", 0) == 0)
    {
        content.size = downloadImage(url, &content);
        if ((int)content.size <= 0)
        {
            free(content.memory);
            content.memory = nullptr;
            this->SetError(std::move("Image Download Fail"));
            return;
        }
    }
    else
    { //本地文件
        content.size = readLocalImage(url, &content);
        if ((int)content.size <= 0)
        {
            free(content.memory);
            content.memory = nullptr;
            this->SetError(std::move("Image Read Fail"));
            return;
        }
    }

    PIC_FORMAT format = getPicFormatFromContent(content.memory, content.size);
    if (format == PNG_FORAMT)
    {
        decodeFromPNGImage(_pixels, _width, _height, (const unsigned char *)content.memory, content.size);
    }
    else if (format == JPEG_FORMAT)
    {
        decodeFromJEPGImage(_pixels, _width, _height, (const unsigned char *)content.memory, (unsigned int)content.size);
    }
    else if (format == UNKOWN_PIC_FORMAT)
    {
        this->SetError(std::move("Image Format Unspported"));
    }
    free(content.memory);
    content.memory = nullptr;
}

}; // namespace NodeBinding