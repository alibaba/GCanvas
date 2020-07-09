/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef IMAGE_WORKER_H
#define IMAGE_WORKER_H
#include <napi.h>
#include "NodeBindingUtil.h"
#include "ImageCahced.h"
namespace NodeBinding
{
//使用asyncWorker来进行node中的异步调用
void cachedImage(const std::string url,std::shared_ptr<ImageCached> imageCached);
class ImageWorker : public Napi::AsyncWorker
{
public:
    
    ImageWorker(Napi::Env env,  std::shared_ptr<ImageCached>& image, unsigned int &width, unsigned int &height) : Napi::AsyncWorker(env), mImageMemCached(image),
                                                                                                             _height(height),
                                                                                                             _width(width)
    {
    }

    void Execute();
    void OnOK();
    void OnError(const Napi::Error &e);
    std::string url;
    void setOnErrorCallback(Napi::Function func);
    void setOnLoadCallback(Napi::Function func);
private:
    Napi::FunctionReference onErrorCallback;
    Napi::FunctionReference onLoadCallback;
    std::shared_ptr<ImageCached> &mImageMemCached;
    unsigned int &_width;
    unsigned int &_height;
    ImageContent content;
};
}
#endif