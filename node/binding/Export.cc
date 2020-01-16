#include <napi.h>
#include <iostream>
#include "Canvas.h"
#include "Image.h"

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
  NodeBinding::Canvas::Init(env);
  exports.Set(Napi::String::New(env, "createCanvas"),
              Napi::Function::New(env, createCanvas));
  NodeBinding::Image::Init(env, exports);
  return exports;
}

NODE_API_MODULE(canvas, Init)
