/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef IMAGE_H
#define IMAGE_H
#include <napi.h>
#include <curl/curl.h>
#include <vector>
#include "NodeBindingUtil.h"
#include "lodepng.h"
#include "ImageCahced.h"
#include "ImageWorker.h"

namespace NodeBinding
{
struct ImageCallbackTuple{
    Napi::FunctionReference mOnErrorCallback;
    Napi::FunctionReference mOnLoadCallback;
};

class Image : public Napi::ObjectWrap<Image>
{
public:
    Image(const Napi::CallbackInfo &info);
    virtual ~Image();
    static void Init(Napi::Env env, Napi::Object exports);
    int getWidth();
    int getHeight();
    std::vector<unsigned char> &getPixels();
    void setTextureId(int textureId);
    int getTextureId();

private:
    static Napi::FunctionReference constructor;
    std::string src;
    ImageCallbackTuple *mCallbackSet;
    ImageWorker *mWorker = nullptr;
    std::vector<unsigned char> emptyPixels;
    std::shared_ptr<ImageCached> mImageCached;
    Napi::Value getSrc(const Napi::CallbackInfo &info);
    void setSrc(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnLoad(const Napi::CallbackInfo &info);
    void setOnLoad(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnError(const Napi::CallbackInfo &info);
    void setOnError(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
};
} // namespace NodeBinding

#endif