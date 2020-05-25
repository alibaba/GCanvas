#include <napi.h>
#include <iostream>
#include "Canvas.h"
#include "Image.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  NodeBinding::Canvas::Init(env,exports);
  NodeBinding::Image::Init(env, exports);
  return exports;
}

NODE_API_MODULE(canvas, Init)
