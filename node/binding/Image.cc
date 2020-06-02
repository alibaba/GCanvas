/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Image.h"
#include "Canvas.h"
#include "NodeBindingUtil.h"
#include <iostream>
#include <stdlib.h>
namespace NodeBinding
{
    Napi::FunctionReference Image::constructor;
    Image::Image(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Image>(info)
    {
        this->mCallbackSet = new ImageCallbackTuple();
    }

    Image::~Image()
    {
        delete this->mCallbackSet;
        this->mCallbackSet = nullptr;
        this->mImageCached = nullptr;
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

    Napi::Object Image::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "gImage"));
        return obj;
    }

    Napi::Value Image::getSrc(const Napi::CallbackInfo &info)
    {
        return Napi::String::New(info.Env(), this->src);
    }

    void Image::setSrc(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        checkArgs(info, 1);
        this->src = value.As<Napi::String>().Utf8Value();
        this->mImageCached=std::make_shared<ImageCached>();
        if (!mWorker)
        {
            mWorker = new ImageWorker(info.Env(), this->mImageCached,
                                      this->mImageCached->width,
                                      this->mImageCached->height);
        }
        if (mWorker)
        {
            mWorker->url = this->src;
            mWorker->setOnErrorCallback(this->mCallbackSet->mOnErrorCallback.Value());
            mWorker->setOnLoadCallback(this->mCallbackSet->mOnLoadCallback.Value());
            mWorker->Queue();
        }
    }
    Napi::Value Image::getOnLoad(const Napi::CallbackInfo &info)
    {
        return this->mCallbackSet->mOnLoadCallback.Value();
    }
    Napi::Value Image::getOnError(const Napi::CallbackInfo &info)
    {
        return this->mCallbackSet->mOnErrorCallback.Value();
    }

    void Image::setOnLoad(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        checkArgs(info, 1);
        this->mCallbackSet->mOnLoadCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    void Image::setOnError(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        checkArgs(info, 1);
        this->mCallbackSet->mOnErrorCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
    {
        if (this->mImageCached)
        {
            return Napi::Number::New(info.Env(), this->mImageCached->width);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }
    Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
    {
        if (this->mImageCached)
        {
            return Napi::Number::New(info.Env(), this->mImageCached->height);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }

    int Image::getWidth()
    {
        return this->mImageCached != nullptr ? mImageCached->width : 0;
    }
    int Image::getHeight()
    {
        return this->mImageCached != nullptr ? mImageCached->height : 0;
    }

    std::vector<unsigned char> &Image::getPixels()
    {
        if (this->mImageCached)
        {
            return this->mImageCached->getPixels();
        }
        else
        {
            //返回引用没办法,只能给一个非局部的vector,稍微浪费一点内存
            return this->emptyPixels;
        }
    }

    void Image::setTextureId(int textureId)
    {
        this->textureId = textureId;
    }
    int Image::getTextureId()
    {
        return this->textureId;
    }

}; // namespace NodeBinding