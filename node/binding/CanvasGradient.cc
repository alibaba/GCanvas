/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "CanvasGradient.h"
#include "CanvasRenderingContext2D.h"
#include "NodeBindingUtil.h"
#include <algorithm> 
namespace NodeBinding
{
Napi::FunctionReference Gradient::constructor;
Gradient::Gradient(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Gradient>(info)
{
    if (info.Length() == 4)
    {
        this->mLinearGradientInfo = std::make_shared<LinearGradientInfo>();
        this->mLinearGradientInfo->startX = info[0].As<Napi::Number>().FloatValue();
        this->mLinearGradientInfo->startY = info[1].As<Napi::Number>().FloatValue();
        this->mLinearGradientInfo->endX = info[2].As<Napi::Number>().FloatValue();
        this->mLinearGradientInfo->endY = info[3].As<Napi::Number>().FloatValue();
    }
    else if (info.Length() == 6)
    {
        this->mRadialGradientInfo = std::make_shared<RadialGradientInfo>();
        this->mRadialGradientInfo->startX = info[0].As<Napi::Number>().FloatValue();
        this->mRadialGradientInfo->startY = info[1].As<Napi::Number>().FloatValue();
        this->mRadialGradientInfo->startR = info[2].As<Napi::Number>().FloatValue();
        this->mRadialGradientInfo->endX = info[3].As<Napi::Number>().FloatValue();
        this->mRadialGradientInfo->endY = info[4].As<Napi::Number>().FloatValue();
        this->mRadialGradientInfo->endR = info[5].As<Napi::Number>().FloatValue();
    }
    else
    {
        throwError(info, "wrong argment number");
    }
}
void Gradient::Init(Napi::Env env)
{
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "CanvasGradient",
                    {
                        InstanceMethod("addColorStop", &Gradient::addColorStop),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Gradient::NewInstance(Napi::Env env, const Napi::CallbackInfo &info)
{
    if (info.Length() == 4)
    {
        Napi::Object obj = constructor.New({info[0], info[1], info[2], info[3]});
        obj.Set("name", Napi::String::New(env, "linearGradient"));
        return obj;
    }
    else if (info.Length() == 6)
    {
        Napi::Object obj = constructor.New({info[0], info[1], info[2], info[3], info[4], info[5]});
        obj.Set("name", Napi::String::New(env, "radialGradient"));
        return obj;
    }
    else
    {
        throwError(info, "wrong argument number");
        return Napi::Object::New(env);
    }
}

void Gradient::addColorStop(const Napi::CallbackInfo &info)
{
    checkArgs(info, 2);
    float offset = 0.0;
    if (info[0].IsNumber())
    {
        offset = info[0].As<Napi::Number>().FloatValue();
    }
    else if (info[0].IsString())
    {
        std::string tmp = info[0].As<Napi::String>().Utf8Value();
        offset = std::stof(tmp);
    }
    else
    {
        throwError(info, "the arg type is invaild");
    }
    std::string color = info[1].As<Napi::String>().Utf8Value();
    offsets.push_back(ColorStop(offset,color));
    std::sort(offsets.begin(),offsets.end(),less_than_key());
    return;
}

const   std::vector<ColorStop> &  Gradient::getColorStops(){
        return this->offsets;
}
} // namespace NodeBinding
