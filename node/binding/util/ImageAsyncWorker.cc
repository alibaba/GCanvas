/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "ImageAsyncWorker.h"
#include <iostream>

ImageAsyncWorker::ImageAsyncWorker(Napi::Env env, std::string url, std::shared_ptr<ImagePixelInfo>& image, ImageDownloadCallback callback) 
    : Napi::AsyncWorker(env), 
    url(url),
    mImageMemCached(image),
    cb(callback)
{

}

ImageAsyncWorker::~ImageAsyncWorker()
{
    std::cout << "ImageAsyncWorker destructor .." << std::endl;
}

void ImageAsyncWorker::OnOK()
{
    NodeBinding::cachedImage(this->url,this->mImageMemCached);
    if (cb != nullptr)
    {
        cb(Env(), (uint8_t *)this->mImageMemCached->buffer, this->mImageMemCached->bufferSize, "");
    }
}

void ImageAsyncWorker::OnError(const Napi::Error &e)
{
    if (cb != nullptr)
    {
        cb(Env(), nullptr, 0, e.Message());
    }
}


void ImageAsyncWorker::Execute()
{
    std::shared_ptr<ImagePixelInfo> cacheRet= NodeBinding::findCacheByUrl(url);
    // 命中缓存,直接返回
    if(cacheRet){
        this->mImageMemCached=cacheRet;
        return;
    }

    NodeBinding::ImageMemoryChunk chunk;

    bool isHttpProtocol=url.rfind("http", 0) == 0;
    bool isHttpsProtocol=url.rfind("https", 0) == 0;
    if ( isHttpProtocol|| isHttpsProtocol)
    {
        chunk.size = NodeBinding::downloadImage(url, &chunk);
        if ((int)chunk.size <= 0)
        {
            chunk.memory = nullptr;
            this->SetError(std::move("Image Download Fail"));
            return;
        }
    }
    else
    { //本地文件
        chunk.size = NodeBinding::readImageFromLocalFile(url, &chunk);
        if ((int)chunk.size <= 0)
        {
            chunk.memory = nullptr;
            this->SetError(std::move("Image Read Fail"));
            return;
        }
    }

    //callback with chunk.memory & chunk.size
    if( chunk.memory && chunk.size > 0 )
    {
        //callback & decode
        mImageMemCached->bufferSize = chunk.size;
        mImageMemCached->buffer = new char[mImageMemCached->bufferSize];
        memcpy(mImageMemCached->buffer,chunk.memory, chunk.size);
    }
    else
    {
        this->SetError(std::move("Image Content Size is 0 "));
    }

    free(chunk.memory);
    chunk.memory = nullptr;
}
