/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef PATTERN_H
#define PATTERN_H
#include <napi.h>
#include <iostream>
#include "Image.h"
namespace NodeBinding
{
class Pattern : public Napi::ObjectWrap<Pattern> {
   public:
    Pattern(const Napi::CallbackInfo& info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::Value repetition);
    std::shared_ptr<Image> content;
    const std::string& getRepetition();
private:
    static Napi::FunctionReference constructor;
   std::string repetition;
};
} // namespace NodeBinding
#endif