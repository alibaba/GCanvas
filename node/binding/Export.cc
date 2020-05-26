#include <napi.h>
#include <iostream>
#include "Canvas.h"
#include "Image.h"
#include "TextMetrics.h"

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

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  //所有binding对象的初始化入口
  NodeBinding::Canvas::Init(env, exports);
  NodeBinding::Image::Init(env, exports);
  NodeBinding::Context2D::Init(env);
  NodeBinding::Gradient::Init(env);
  NodeBinding::ImageData::Init(env);
  NodeBinding::TextMetrics::Init(env);
  NodeBinding::Pattern::Init(env);
  exports.Set(Napi::String::New(env, "createCanvas"),
              Napi::Function::New(env, createCanvas));
  return exports;
}

NODE_API_MODULE(canvas, Init)
