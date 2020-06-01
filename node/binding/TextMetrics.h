/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef TEXTMETRICS_H
#define TEXTMETRICS_H
#include <napi.h>
namespace NodeBinding
{
class TextMetrics : public Napi::ObjectWrap<TextMetrics>
{
public:
    TextMetrics(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::Value arg);

private:
    float textWidth = 0;
    static Napi::FunctionReference constructor;
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
};
} // namespace NodeBinding
#endif