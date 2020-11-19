/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <napi.h>
#include <iostream>
#include "Canvas.h"
#include "Image.h"
#include "TextMetrics.h"
#include "./webgl/WebGLShader.h"
#include "./webgl/WebGLBuffer.h"
#include "./webgl/WebGLProgram.h"
#include "./webgl/WebGLTexture.h"
#include "./webgl/WebGLFrameBuffer.h"
#include "./webgl/WebGLActiveInfo.h"
#include "./webgl/WebGLUniformLocation.h"
#include "./webgl/WebGLRenderBuffer.h"

Napi::Object createCanvas(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() < 2)
  {
    Napi::TypeError::New(env, "canvas need width & height")
        .ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }
  return NodeBinding::Canvas::NewInstance(env, info[0], info[1]);
}

Napi::Object createImage(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return NodeBinding::Image::NewInstance(env);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  //所有binding对象的初始化入口
  NodeBinding::Canvas::Init(env, exports);
  NodeBinding::Image::Init(env, exports);
  NodeBinding::Context2D::Init(env);
  NodeBinding::ContextWebGL::Init(env);
  NodeBinding::Gradient::Init(env);
  NodeBinding::ImageData::Init(env);
  NodeBinding::TextMetrics::Init(env);
  NodeBinding::Pattern::Init(env);
  //webl reousce binding
  NodeBinding::WebGLShader::Init(env);
  NodeBinding::WebGLProgram::Init(env);
  NodeBinding::WebGLBuffer ::Init(env);
  NodeBinding::WebGLTexture::Init(env);
  NodeBinding::WebGLFrameBuffer::Init(env);
  NodeBinding::WebGLRenderBuffer::Init(env);
  NodeBinding::WebGLActiveInfo::Init(env);
  NodeBinding::WebGLUniformLocation::Init(env);
  exports.Set(Napi::String::New(env, "createCanvas"),
              Napi::Function::New(env, createCanvas));
  exports.Set(Napi::String::New(env, "createImage"),
              Napi::Function::New(env, createImage));
  return exports;
}

NODE_API_MODULE(canvas, Init)
