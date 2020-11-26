/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "WebGLRenderBuffer.h"

namespace NodeBinding
{
Napi::FunctionReference WebGLRenderBuffer::constructor;
WebGLRenderBuffer::WebGLRenderBuffer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLRenderBuffer>(info)
{
    mId = info[0].As<Napi::Number>().Uint32Value();
}

void WebGLRenderBuffer::Init(Napi::Env env)
{
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "WebGLRenderBuffer", {});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object WebGLRenderBuffer::NewInstance(Napi::Env env, const Napi::Value arg)
{
    Napi::Object obj = constructor.New({arg});
    return obj;
}
} // namespace NodeBinding