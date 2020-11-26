/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "WebGLActiveInfo.h"

namespace NodeBinding
{
    Napi::FunctionReference WebGLActiveInfo::constructor;
    WebGLActiveInfo::WebGLActiveInfo(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLActiveInfo>(info)
    {
        mSize = info[0].As<Napi::Number>().Int32Value();
        mType = info[1].As<Napi::Number>().Int32Value();
        mName = info[2].As<Napi::String>().Utf8Value();
    }

    void WebGLActiveInfo::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);

        Napi::Function func = DefineClass(env, "WebGLActiveInfo", {
            InstanceAccessor("name", &WebGLActiveInfo::getName, nullptr),
            InstanceAccessor("size", &WebGLActiveInfo::getSize, nullptr),
            InstanceAccessor("type", &WebGLActiveInfo::getType, nullptr)
        });
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
    }

    Napi::Object WebGLActiveInfo::NewInstance(Napi::Env env, GLuint size, GLuint type, GLchar *buffer)
    {
        Napi::Object obj = constructor.New({Napi::Number::New(env, size), Napi::Number::New(env, type), Napi::String::New(env, buffer)});
        return obj;
    }
} // namespace NodeBinding