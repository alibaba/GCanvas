/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef WebGLACTIVERINFO_H
#define WebGLACTIVERINFO_H
#include <napi.h>
#include <string>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLActiveInfo : public Napi::ObjectWrap<WebGLActiveInfo>
    {
    public:
        WebGLActiveInfo(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, GLuint size, GLuint type, GLchar *buffer);

    private:
        Napi::Value getName(const Napi::CallbackInfo &info)
        {
            return Napi::String::New(info.Env(), mName);
        }
        Napi::Value getSize(const Napi::CallbackInfo &info)
        {
            return Napi::Number::New(info.Env(), mSize);
        }
        Napi::Value getType(const Napi::CallbackInfo &info)
        {
            return Napi::Number::New(info.Env(), mType);
        }
        static Napi::FunctionReference constructor;
        GLint mType;
        GLint mSize;
        std::string mName;
    };
} // namespace NodeBinding
#endif