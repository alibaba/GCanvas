#ifndef CAIRO_PATTERN_H
#define CAIRO_PATTERN_H

#pragma once

#include <cairo.h>
#include <napi.h>

namespace cairocanvas
{

typedef enum {
  NO_REPEAT,  // match CAIRO_EXTEND_NONE
  REPEAT,  // match CAIRO_EXTEND_REPEAT
  REPEAT_X, // needs custom processing
  REPEAT_Y // needs custom processing
} repeat_type_t;

extern const cairo_user_data_key_t *pattern_repeat_key;

class Pattern: public Napi::ObjectWrap<Pattern> {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(const Napi::CallbackInfo &info,  Napi::Value image, Napi::Value repetition);
    static repeat_type_t get_repeat_type_for_cairo_pattern(cairo_pattern_t *pattern);

    Pattern(const Napi::CallbackInfo &info);
    ~Pattern();
    inline cairo_pattern_t *pattern(){ return _pattern; }
  private:
   static Napi::FunctionReference constructor;

   void setupPattern(cairo_surface_t *surface, repeat_type_t repeat);
   cairo_pattern_t *_pattern;
   repeat_type_t _repeat;
};

}

#endif