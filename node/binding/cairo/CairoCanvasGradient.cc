#include "CairoCanvasGradient.h"

#include "CairoCanvas.h"
#include "color.h"
#include "NodeBindingUtil.h"


namespace cairocanvas
{

Napi::FunctionReference Gradient::constructor;


void Gradient::Init(Napi::Env env)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "CanvasGradient", {
    InstanceMethod("addColorStop", &Gradient::addColorStop),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

 Napi::Object Gradient::NewInstance(const Napi::CallbackInfo &info,  Napi::Value x0, Napi::Value y0, Napi::Value x1, Napi::Value y1 )
 {
    Napi::Object obj = constructor.New({x0,y0,x1,y1});
    obj.Set("name",  Napi::String::New(info.Env(), "linearGradient"));
    return obj;
 }

Napi::Object Gradient::NewInstance(const Napi::CallbackInfo &info,  Napi::Value x0, Napi::Value y0, Napi::Value r0, Napi::Value x1, Napi::Value y1, Napi::Value r1)
{
  Napi::Object obj = constructor.New({x0,y0,r0,x1,y1,r1});
  obj.Set("name",  Napi::String::New(info.Env(), "radialGradient"));
  return obj;
}

Gradient::Gradient(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Gradient>(info)
{
  Napi::Env env = info.Env();

  if( info.Length() == 4 && 
      info[0].IsNumber() && 
      info[1].IsNumber() && 
      info[2].IsNumber() &&
      info[3].IsNumber() )
  {
    double args[4];
    args[0] = info[0].As<Napi::Number>().DoubleValue();
    args[1] = info[1].As<Napi::Number>().DoubleValue();
    args[2] = info[2].As<Napi::Number>().DoubleValue();
    args[3] = info[3].As<Napi::Number>().DoubleValue();

    setupGradient(args[0], args[1], args[2], args[3]);
  }
  else if( info.Length() == 6 )
  {
    double args[6];
    args[0] = info[0].As<Napi::Number>().DoubleValue();
    args[1] = info[1].As<Napi::Number>().DoubleValue();
    args[2] = info[2].As<Napi::Number>().DoubleValue();
    args[3] = info[3].As<Napi::Number>().DoubleValue();
    args[4] = info[4].As<Napi::Number>().DoubleValue();
    args[3] = info[5].As<Napi::Number>().DoubleValue();
    setupGradient(args[0], args[1], args[2], args[3], args[4], args[5]);
  }
}

/*
 * Add color stop.
 */
void Gradient::addColorStop(const Napi::CallbackInfo &info)
{
  if( info.Length() < 2)
  {
    return;
  }

  float offset = 0.0;
  if (info[0].IsNumber())
  {
    offset = info[0].As<Napi::Number>().FloatValue();
  }
  else if (info[0].IsString())
  {
    std::string tmp = info[0].As<Napi::String>().Utf8Value();
    offset = std::stof(tmp);
  }
  else
  {
    NodeBinding::throwError(info, "offset is invalid");
    return;
  }

  if (!info[1].IsString())
  {
    NodeBinding::throwError(info, "color string is invalid");
    return;
  }

  short ok;
  std::string str = info[1].As<Napi::String>().Utf8Value();
  uint32_t rgba = rgba_from_string(str.c_str(), &ok);
  if (ok) {
    rgba_t color = rgba_create(rgba);
    cairo_pattern_add_color_stop_rgba(
        _pattern
      , offset
      , color.r
      , color.g
      , color.b
      , color.a);
  } else {
    NodeBinding::throwError(info, "parse color failed");
  }
}

/*
 * Initialize linear gradient.
 */

void Gradient::setupGradient(double x0, double y0, double x1, double y1) 
{
  std::cout << "Gradient::LinearGradient(" << x0 <<","  << y0 <<"," << x1 <<"," << y1 <<")"<< std::endl;
  _pattern = cairo_pattern_create_linear(x0, y0, x1, y1);
}

/*
 * Initialize radial gradient.
 */

void Gradient::setupGradient(double x0, double y0, double r0, double x1, double y1, double r1) 
{
    std::cout << "Gradient::RadialGradient(" << x0 <<","  << y0 <<"," << r0<<","<< x1 <<"," << y1 <<","<< r1 <<")"<< std::endl;
  _pattern = cairo_pattern_create_radial(x0, y0, r0, x1, y1, r1);
}

/*
 * Destroy the pattern.
 */

Gradient::~Gradient() 
{
  cairo_pattern_destroy(_pattern);
}


}
