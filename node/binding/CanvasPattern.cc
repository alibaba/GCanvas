/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "CanvasPattern.h"
#include "NodeBindingUtil.h"
namespace NodeBinding
{
Napi::FunctionReference Pattern::constructor;

Pattern::Pattern(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Pattern>(info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsObject() || !info[1].IsString())
    {
        napi_throw_error(env, "", "Pattern parameter invalid");
        return;
    }

    content  = Napi::ObjectWrap<Image>::Unwrap(info[0].As<Napi::Object>());

    repetition = info[1].As<Napi::String>().Utf8Value();
    if (repetition != "" &&
        repetition != "repeat" &&
        repetition != "repeat-x" && repetition != "repeat-y" &&
        repetition != "no-repeat")
    {
        throwError(info, "repetition value wrong");
    }

    if (repetition == "")
    {
        repetition = "repeat";
    }
}

Napi::Object Pattern::NewInstance(const Napi::CallbackInfo &info,  Napi::Value image, Napi::Value repetition)
{
  Napi::Object obj = constructor.New( {image, repetition} );
  obj.Set("name",  Napi::String::New(info.Env(), "pattern"));
  return obj;
}

void Pattern::Init(Napi::Env env) {
    Napi::HandleScope scope(env);

    Napi::Function func =  DefineClass(env, "CanvasPattern", {});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

const std::string &Pattern::getRepetition() { return repetition; }
} // namespace NodeBinding