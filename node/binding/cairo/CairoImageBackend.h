#pragma once

#include <cairo.h>
#include <exception>
#include <napi.h>
#include <string>
#include <iostream>

namespace cairocanvas
{

  class Canvas;

  class CairoImageBackend
  {
  public:
    CairoImageBackend(int width, int height);
    ~CairoImageBackend();

    cairo_format_t getFormat();
    void setFormat(cairo_format_t format);

    void setCanvas(Canvas *canvas);
    cairo_surface_t *createSurface();
    cairo_surface_t *recreateSurface();

    cairo_surface_t *getSurface();
    void destroySurface();

    int getWidth();
    void setWidth(int width);

    int getHeight();
    virtual void setHeight(int height);

    bool isSurfaceValid();

    int32_t approxBytesPerPixel();
    const static cairo_format_t DEFAULT_FORMAT = CAIRO_FORMAT_ARGB32;

  private:
    cairo_format_t format = DEFAULT_FORMAT;
    const char *error = NULL;
    int width;
    int height;
    cairo_surface_t *surface = nullptr;
    Canvas *canvas = nullptr;
  };

} // namespace cairocanvas
