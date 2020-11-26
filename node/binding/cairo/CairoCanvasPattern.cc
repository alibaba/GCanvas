
#include "CairoCanvasPattern.h"

#include "CairoCanvas.h"
#include "CairoImage.h"


namespace cairocanvas
{
const cairo_user_data_key_t *pattern_repeat_key;
Napi::FunctionReference Pattern::constructor;

void Pattern::Init(Napi::Env env)
{
  Napi::Function func = DefineClass(env, "CanvasPattern", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}


Napi::Object Pattern::NewInstance(const Napi::CallbackInfo &info,  Napi::Value image, Napi::Value repetition)
{
  Napi::Object obj = constructor.New({image, repetition});
  obj.Set("name",  Napi::String::New(info.Env(), "pattern"));
  return obj;
}

Pattern::Pattern(const Napi::CallbackInfo &info)  : Napi::ObjectWrap<Pattern>(info)
{
  Napi::Env env = info.Env();

  if (!info[0].IsObject() || !info[1].IsString())
  {
    napi_throw_error(env, "", "Pattern parameter invalid");
    return;
  }
  
  //TODO image or canvas type check
  cairo_surface_t *surface;
  Napi::Object object = info[0].As<Napi::Object>();
  Napi::Value name = object.Get("name");
  if (!name.IsString())
  {
    return;
  }

  std::string namePropetry = name.As<Napi::String>().Utf8Value();

  // Image
  if (namePropetry == "image")
  {
    Image *img = Napi::ObjectWrap<Image>::Unwrap(object);
    // if( !img->isComplete() )
    // {
    //   napi_throw_error(env, "", "Image given has not completed loading");
    //   return;
    // }
    surface = img->getSurface();
  }
  else if( namePropetry == "canvas" )
  {
    Canvas *canvas = Napi::ObjectWrap<Canvas>::Unwrap(object);
    surface = canvas->surface();
  }
  else
  {
    napi_throw_error(env, "", "Image or Canvas expected");
    return;
  }

  std::string repeatStr = info[1].As<Napi::String>().Utf8Value();
  repeat_type_t repeat = REPEAT;
  if (0 == strcmp("no-repeat", repeatStr.c_str())) {
    repeat = NO_REPEAT;
  } else if (0 == strcmp("repeat-x", repeatStr.c_str())) {
    repeat = REPEAT_X;
  } else if (0 == strcmp("repeat-y", repeatStr.c_str())) {
    repeat = REPEAT_Y;
  }
  setupPattern(surface, repeat);
}

void Pattern::setupPattern(cairo_surface_t *surface, repeat_type_t repeat) 
{
  _pattern = cairo_pattern_create_for_surface(surface);
  _repeat = repeat;
  cairo_pattern_set_user_data(_pattern, pattern_repeat_key, &_repeat, NULL);
}

repeat_type_t Pattern::get_repeat_type_for_cairo_pattern(cairo_pattern_t *pattern) {
  void *ud = cairo_pattern_get_user_data(pattern, pattern_repeat_key);
  return *reinterpret_cast<repeat_type_t*>(ud);
}

/*
 * Destroy the pattern.
 */
Pattern::~Pattern() 
{
  cairo_pattern_destroy(_pattern);
}

}