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
    }

    Image::~Image()
    {
        mImageMemCached = nullptr;
    }

    void Image::Init(Napi::Env env, Napi::Object exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function func = DefineClass(env, "Image",{
            InstanceAccessor("src", &Image::getSource, &Image::setSource),
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

    Napi::Value Image::getSource(const Napi::CallbackInfo &info)
    {
        return Napi::String::New(info.Env(), mSrc);
    }

    void Image::DownloadCallback(Napi::Env env, uint8_t *data, size_t size, std::string errMsg )
    {
        if( data != nullptr && size > 0 )
        {
            //decode 
            PIC_FORMAT format = parseFormat((char *)data, size);
            if (format == PNG_FORAMT)
            {
                decodeImagePNG(this->mImageMemCached->getPixels(), mImageMemCached->width, mImageMemCached->height, (const unsigned char *)data, size);
            }
            else if (format == JPEG_FORMAT)
            {
                decodeImageJPEG(this->mImageMemCached->getPixels(),  mImageMemCached->width, mImageMemCached->height, (const unsigned char *)data, size);
            }
            //callback
            if( mImageMemCached->width > 0 &&  mImageMemCached->height > 0 )
            {
                if( mOnLoadCallback )
                {
                    mOnLoadCallback.Call({env.Undefined()});
                    return;
                }
            }
        }
        
         if( mOnErrorCallback )
        {
            mOnErrorCallback.Call({Napi::String::New(env, "load image error")});
        }   
    }
    void Image::setSource(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        NodeBinding::checkArgs(info, 1);
        mSrc = value.As<Napi::String>().Utf8Value();
        mImageMemCached=std::make_shared<ImagePixelInfo>();
        if (!mDownloadImageWorker)
        {
            mDownloadImageWorker = new ImageAsyncWorker(info.Env(), mSrc, mImageMemCached, std::bind(&Image::DownloadCallback, this,  std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3,  std::placeholders::_4) );
        }
        mDownloadImageWorker->Queue();
    }

    Napi::Value Image::getOnLoadCallback(const Napi::CallbackInfo &info)
    {
        return mOnLoadCallback ? mOnLoadCallback.Value() : info.Env().Undefined();
    }
    Napi::Value Image::getOnErrorCallback(const Napi::CallbackInfo &info)
    {
        return mOnErrorCallback ? mOnErrorCallback.Value() : info.Env().Undefined();
    }

    void Image::setOnLoadCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        NodeBinding::checkArgs(info, 1);
        mOnLoadCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    void Image::setOnErrorCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        NodeBinding::checkArgs(info, 1);
        mOnErrorCallback = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
    {
        if (mImageMemCached)
        {
            return Napi::Number::New(info.Env(), mImageMemCached->width);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }
    Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
    {
        if (mImageMemCached)
        {
            return Napi::Number::New(info.Env(), mImageMemCached->height);
        }
        else
        {
            return Napi::Number::New(info.Env(), 0);
        }
    }

    int Image::getWidth()
    {
        return mImageMemCached != nullptr ? mImageMemCached->width : 0;
    }
    int Image::getHeight()
    {
        return mImageMemCached != nullptr ? mImageMemCached->height : 0;
    }

    std::vector<unsigned char> &Image::getPixels()
    {
        if (mImageMemCached)
        {
            return mImageMemCached->getPixels();
        }
        else
        {
            //引用没办法,只能给一个非局部的vector,稍微浪费一点内存
            return emptyPixels;
        }
    }

    void Image::setTextureId(int textureId)
    {
        mTextureId = textureId;
    }
    int Image::getTextureId()
    {
        return mTextureId;
    }

}; // namespace NodeBinding