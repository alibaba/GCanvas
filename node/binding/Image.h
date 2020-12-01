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
#include "ImagePixelInfo.h"
#include "ImageAsyncWorker.h"


namespace NodeBinding
{

extern std::shared_ptr<ImagePixelInfo> findCacheByUrl(const std::string &url);
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
    std::string getSrc(){ return mSrc; }
    static Napi::Object NewInstance(Napi::Env env);
private:
    static Napi::FunctionReference constructor;
    std::string mSrc;
    Napi::FunctionReference mOnErrorCallback;
    Napi::FunctionReference mOnLoadCallback;
    ImageAsyncWorker *mDownloadImageWorker = nullptr;
    std::vector<unsigned char> emptyPixels;
    std::shared_ptr<ImagePixelInfo> mImageMemCached;
    Napi::Value getSource(const Napi::CallbackInfo &info);
    void setSource(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnLoadCallback(const Napi::CallbackInfo &info);
    void setOnLoadCallback(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnErrorCallback(const Napi::CallbackInfo &info);
    void setOnErrorCallback(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);

    void DownloadCallback(Napi::Env env, uint8_t *data, size_t size, std::string errMsg );

    int mTextureId=-1;
};
} // namespace NodeBinding

#endif