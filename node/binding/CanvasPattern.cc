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

Pattern::Pattern(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Pattern>(info) {
    if (info[0].IsNull())
    {
        this->repetition = "repeat";
        return;
    }
    this->repetition = info[0].As<Napi::String>().Utf8Value();
    if (this->repetition != "" &&
        this->repetition != "repeat" &&
        this->repetition != "repeat-x" && this->repetition != "repeat-y" &&
        this->repetition != "no-repeat")
    {
        throwError(info, "repetition value wrong");
    }
    if (this->repetition == "")
    {
        this->repetition = "repeat";
    }
}

Napi::Object Pattern::NewInstance(Napi::Env env, const Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    obj.Set("name", Napi::String::New(env, "pattern"));
    return obj;
}

void Pattern::Init(Napi::Env env) {
    Napi::HandleScope scope(env);

    Napi::Function func =
        DefineClass(env,
                    "CanvasPattern",
                    {

                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

const std::string &Pattern::getRepetition() { return this->repetition; }
} // namespace NodeBinding