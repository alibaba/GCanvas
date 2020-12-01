/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef IMAGE_ASYNC_WORKER_H
#define IMAGE_ASYNC_WORKER_H
#include <napi.h>
#include "ImagePixelInfo.h"
#include "NodeBindingUtil.h"

typedef std::function<void (Napi::Env env, uint8_t *data, size_t size, std::string msg )> ImageDownloadCallback;

class ImageAsyncWorker : public Napi::AsyncWorker
{
public:
    ImageAsyncWorker(Napi::Env env, std::string url, std::shared_ptr<ImagePixelInfo>& image, ImageDownloadCallback callback); 
    ~ImageAsyncWorker();

    void Execute();
    void OnOK();
    void OnError(const Napi::Error &e);

    std::string url;

private:
    std::shared_ptr<ImagePixelInfo> &mImageMemCached;
    ImageDownloadCallback cb;
};

#endif