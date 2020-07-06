/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "TextMetrics.h"

namespace NodeBinding
{
Napi::FunctionReference TextMetrics::constructor;
TextMetrics::TextMetrics(const Napi::CallbackInfo &info) : Napi::ObjectWrap<TextMetrics>(info)
{
    this->textWidth = info[0].As<Napi::Number>().FloatValue();
}

void TextMetrics::Init(Napi::Env env)
{
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "TextMetric",
                    {
                        InstanceAccessor("width", &TextMetrics::getWidth, nullptr),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object TextMetrics::NewInstance(Napi::Env env, const Napi::Value arg)
{
    Napi::Object obj = constructor.New({arg});
    return obj;
}

Napi::Value TextMetrics::getWidth(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->textWidth);
}
void TextMetrics::setWidth(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}
} // namespace NodeBinding