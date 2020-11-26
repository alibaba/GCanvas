/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "WebGLUniformLocation.h"

namespace NodeBinding
{
    Napi::FunctionReference WebGLUniformLocation::constructor;
    WebGLUniformLocation::WebGLUniformLocation(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLUniformLocation>(info)
    {
        mIndex = info[0].As<Napi::Number>().Uint32Value();
    }

    void WebGLUniformLocation::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);
        Napi::Function func = DefineClass(env, "WebGLUniformLocation", {});
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
    }

    Napi::Object WebGLUniformLocation::NewInstance(Napi::Env env, GLuint index)
    {

        Napi::Object obj = constructor.New({Napi::Number::New(env, index)});
        return obj;
    }
} // namespace NodeBinding