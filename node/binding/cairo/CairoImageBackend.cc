#include "CairoImageBackend.h"
#include <exception>
#include <assert.h>

namespace cairocanvas
{

  CairoImageBackend::CairoImageBackend(int w, int h) : width(w), height(h)
  {
  }
  
  CairoImageBackend::~CairoImageBackend()
  {
    destroySurface();
  }

  cairo_format_t CairoImageBackend::getFormat()
  {
    return format;
  }

  void CairoImageBackend::setFormat(cairo_format_t _format)
  {
    format = _format;
  }

  // This returns an approximate value only, suitable for Nan::AdjustExternalMemory.
  // The formats that don't map to intrinsic types (RGB30, A1) round up.
  int32_t CairoImageBackend::approxBytesPerPixel()
  {
    switch (format)
    {
    case CAIRO_FORMAT_ARGB32:
    case CAIRO_FORMAT_RGB24:
      return 4;
#ifdef CAIRO_FORMAT_RGB30
    case CAIRO_FORMAT_RGB30:
      return 3;
#endif
    case CAIRO_FORMAT_RGB16_565:
      return 2;
    case CAIRO_FORMAT_A8:
    case CAIRO_FORMAT_A1:
      return 1;
    default:
      return 0;
    }
  }

  void CairoImageBackend::setCanvas(Canvas *_canvas)
  {
    canvas = _canvas;
  }

  cairo_surface_t *CairoImageBackend::createSurface()
  {
    assert(!surface);
    surface = cairo_image_surface_create(format, width, height);
    assert(surface);

    // TODO
    // Napi::MemoryManagement::AdjustExternalMemory(env, approxBytesPerPixel() * width * height);
    return surface;
  }

  cairo_surface_t *CairoImageBackend::recreateSurface()
  {
    destroySurface();
    return createSurface();
  }

  cairo_surface_t *CairoImageBackend::getSurface()
  {
    if (!surface)
      createSurface();
    return surface;
  }

  void CairoImageBackend::destroySurface()
  {
    if (surface)
    {
      cairo_surface_destroy(surface);
      surface = NULL;
      //TODO
      // Napi::MemoryManagement::AdjustExternalMemory(env, -approxBytesPerPixel() * width * height);
    }
  }

  int CairoImageBackend::getWidth()
  {
    return width;
  }
  void CairoImageBackend::setWidth(int width_)
  {
    width = width_;
    recreateSurface();
  }

  int CairoImageBackend::getHeight()
  {
    return height;
  }
  void CairoImageBackend::setHeight(int height_)
  {
    height = height_;
    recreateSurface();
  }
  
  bool CairoImageBackend::isSurfaceValid()
  {
    bool hadSurface = surface != NULL;
    bool isValid = true;

    cairo_status_t status = cairo_surface_status(getSurface());

    if (status != CAIRO_STATUS_SUCCESS)
    {
      error = cairo_status_to_string(status);
      isValid = false;
    }

    if (!hadSurface)
      destroySurface();
    return isValid;
  }

} // namespace cairocanvas
