/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef  WebGLUniformLocation_H
#define  WebGLUniformLocation_H
#include <napi.h>
#include <string>
#include <GL/gl.h>
namespace NodeBinding
{
    class WebGLUniformLocation : public Napi::ObjectWrap<WebGLUniformLocation>
    {
    public:
        WebGLUniformLocation(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, GLuint index);
        inline GLuint getIndex(){ return mIndex; }
    private:
        static Napi::FunctionReference constructor;
        GLuint mIndex;
    };
} // namespace NodeBinding
#endif