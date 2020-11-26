#ifndef CAIRO_CANVAS_H
#define CAIRO_CANVAS_H

#pragma once

#include "CairoImageBackend.h"

#include <cairo.h>
#include <napi.h>
#include <pango/pangocairo.h>
#include <vector>

#ifndef CANVAS_MAX_STATES
#define CANVAS_MAX_STATES 64
#endif


namespace cairocanvas
{
class FontFace {
  public:
    PangoFontDescription *sys_desc = nullptr;
    PangoFontDescription *user_desc = nullptr;
};

class Canvas: public Napi::ObjectWrap<Canvas> {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2);
    static PangoWeight GetWeightFromCSSString(const char *weight);
    static PangoStyle GetStyleFromCSSString(const char *style);
    static PangoFontDescription *ResolveFontDescription(const PangoFontDescription *desc);

    Canvas(const Napi::CallbackInfo &info);
    virtual ~Canvas();

    inline CairoImageBackend* backend() { return mBackend; }
    inline cairo_surface_t* surface(){ return backend()->getSurface(); }
    cairo_t* createCairoContext();

    inline uint8_t *data(){ return cairo_image_surface_get_data(surface()); }
    inline int stride(){ return cairo_image_surface_get_stride(surface()); }
    inline int nBytes(){ return getHeight() * stride(); }

    inline int getWidth() { return backend()->getWidth(); }
    inline int getHeight() { return backend()->getHeight(); }

    Napi::ObjectReference mCanvasRef;
    
 private:
    static Napi::FunctionReference constructor;
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
    void setHeight(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getContext(const Napi::CallbackInfo &info);
    Napi::Value createPNGStreamSync(const Napi::CallbackInfo &info);
    Napi::Value createJPGStreamSync(const Napi::CallbackInfo &info);
    Napi::Value toBuffer(const Napi::CallbackInfo &info);
    Napi::Value toDataURL(const Napi::CallbackInfo &info);
    Napi::Buffer<unsigned char> getPNGBuffer(const Napi::CallbackInfo &info, unsigned long &size);
    Napi::Buffer<unsigned char> getJPGBuffer(const Napi::CallbackInfo &info, unsigned long &size);
    Napi::Buffer<unsigned char> getRawDataBuffer(const Napi::CallbackInfo &info, unsigned long &size);

    Napi::ObjectReference mContext2dRef;
    Napi::ObjectReference mContextWebGLRef;

    int mWidth = 0;
    int mHeight = 0;
    unsigned char *mDataRaw;
    CairoImageBackend* mBackend;
};

}

#endif
