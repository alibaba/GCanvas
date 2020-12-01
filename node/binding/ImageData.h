/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef IMAGEDATA_H
#define IMAGEDATA_H
#include <napi.h>
namespace NodeBinding
{
class ImageData : public Napi::ObjectWrap<ImageData>
{
public:
    ImageData(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::Value width, const Napi::Value height);
    std::vector<uint8_t> &getPixels();
    int getWidth();
    int getHeight();

private:
    static Napi::FunctionReference constructor;
    Napi::ObjectReference mImageDataRef;
    int mWidth = 0;
    int mHeight = 0;
    bool hasImageDataWrite = false;

    std::vector<uint8_t> pixels;
    Napi::Value getData(const Napi::CallbackInfo &info);
    void setData(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
};
} // namespace NodeBinding

#endif