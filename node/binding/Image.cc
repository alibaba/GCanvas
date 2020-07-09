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
        this->mCallbackSet = new ImageCallbackSet();
    }

    Image::~Image()
    {
        delete this->mDownloadImageWorker;
        this->mDownloadImageWorker = nullptr;
        delete this->mCallbackSet;
        this->mCallbackSet = nullptr;
        this->mImageMemCached = nullptr;
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
                            InstanceAccessor("onload", &Image::getOnLoadCallback, &Image::setOnLoadCallback),
                            InstanceAccessor("onerror", &Image::getOnErrorCallback, &Image::setOnErrorCallback),
                        });
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("Image", func);
    }

    Napi::Object Image::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "image"));
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
        this->mImageMemCached=std::make_shared<ImageCached>();
        if (!mDownloadImageWorker)
        {
            mDownloadImageWorker = new ImageWorker(info.Env(), this->mImageMemCached,
                                      this->mImageMemCached->width,
                                      this->mImageMemCached->height);
        }
        if (mDownloadImageWorker)
        {
            mDownloadImageWorker->url = this->src;
            mDownloadImageWorker->setOnErrorCallback(this->mCallbackSet->mOnErrorCallback.Value());
            mDownloadImageWorker->setOnLoadCallback(this->mCallbackSet->mOnLoadCallback.Value());
            mDownloadImageWorker->Queue();
        }
    }
    Napi::Value Image::getOnLoadCallback(const Napi::CallbackInfo &info)
    {
        return this->mCallbackSet->mOnLoadCallback.Value();
    }
    Napi::Value Image::getOnErrorCallback(const Napi::CallbackInfo &info)
    {
        return this->mCallbackSet->mOnErrorCallback.Value();
    }

    void Image::setOnLoadCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        checkArgs(info, 1);
        this->mCallbackSet->mOnLoadCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    void Image::setOnErrorCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        checkArgs(info, 1);
        this->mCallbackSet->mOnErrorCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
    {
        if (this->mImageMemCached)
        {
            return Napi::Number::New(info.Env(), this->mImageMemCached->width);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }
    Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
    {
        if (this->mImageMemCached)
        {
            return Napi::Number::New(info.Env(), this->mImageMemCached->height);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }

    int Image::getWidth()
    {
        return this->mImageMemCached != nullptr ? mImageMemCached->width : 0;
    }
    int Image::getHeight()
    {
        return this->mImageMemCached != nullptr ? mImageMemCached->height : 0;
    }

    std::vector<unsigned char> &Image::getPixels()
    {
        if (this->mImageMemCached)
        {
            return this->mImageMemCached->getPixels();
        }
        else
        {
            //引用没办法,只能给一个非局部的vector,稍微浪费一点内存
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