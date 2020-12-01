#ifndef CAIRO_CONTEXT2D_H
#define CAIRO_CONTEXT2D_H

#pragma once

#include "cairo.h"
#include "CairoCanvas.h"
#include "color.h"
#include "napi.h"
#include <pango/pangocairo.h>

namespace cairocanvas
{
typedef enum {
  TEXT_DRAW_PATHS,
  TEXT_DRAW_GLYPHS
} canvas_draw_mode_t;

/*
 * State struct.
 *
 * Used in conjunction with Save() / Restore() since
 * cairo's gstate maintains only a single source pattern at a time.
 */

typedef struct {
  rgba_t fill;
  rgba_t stroke;
  cairo_filter_t patternQuality;
  cairo_pattern_t *fillPattern;
  cairo_pattern_t *strokePattern;
  cairo_pattern_t *fillGradient;
  cairo_pattern_t *strokeGradient;
  float globalAlpha;
  short textAlignment;
  short textBaseline;
  rgba_t shadow;
  int shadowBlur;
  double shadowOffsetX;
  double shadowOffsetY;
  canvas_draw_mode_t textDrawingMode;
  PangoFontDescription *fontDescription;
  bool imageSmoothingEnabled;
} canvas_state_t;

/*
 * Equivalent to a PangoRectangle but holds floats instead of ints
 * (software pixels are stored here instead of pango units)
 *
 * Should be compatible with PANGO_ASCENT, PANGO_LBEARING, etc.
 */

typedef struct {
  float x;
  float y;
  float width;
  float height;
} float_rectangle;

void state_assign_fontFamily(canvas_state_t *state, const char *str);

class Context2d: public Napi::ObjectWrap<Context2d> {
  public:
    short stateno;
    canvas_state_t *states[CANVAS_MAX_STATES];
    canvas_state_t *state;



    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(const Napi::CallbackInfo &info);
    static void registerParseFont(Napi::Function func);

    Context2d(const Napi::CallbackInfo &info);
    ~Context2d();
    void setupContext2d(Canvas *canvas);

    inline void setContext(cairo_t *ctx) { _context = ctx; }
    inline cairo_t *context(){ return _context; }
    inline Canvas *canvas(){ return _canvas; }

    inline bool hasShadow();
    void inline setSourceRGBA(rgba_t color);
    void inline setSourceRGBA(cairo_t *ctx, rgba_t color);
    void setTextPath(double x, double y);
    void blur(cairo_surface_t *surface, int radius);
    void shadow(void (fn)(cairo_t *cr));
    void shadowStart();
    void shadowApply();
    void savePath();
    void restorePath();
    void saveState();
    void restoreState();
    void inline setFillRule(const Napi::Value &value);
    void fill(bool preserve = false);
    void stroke(bool preserve = false);
    void save();
    void restore();
    void setFontFromState();
    void resetState(bool init = false);
    inline PangoLayout *layout(){ return _layout; }

  private:

    void _resetPersistentHandles();
    void paintText(const Napi::CallbackInfo &info, bool stroke);

    Napi::Value _getFillColor(const Napi::CallbackInfo &info);
    Napi::Value _getStrokeColor(const Napi::CallbackInfo &info);
    void _setFillColor( const Napi::Value &value);
    void _setFillPattern(const Napi::CallbackInfo &info, const Napi::Value &value);
    void _setStrokeColor(const Napi::Value &value);
    void _setStrokePattern(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::ObjectReference _fillStyle;
    Napi::ObjectReference _strokeStyle;
    Napi::ObjectReference _font;    
    Napi::ObjectReference _textBaseline;
    Napi::ObjectReference _textAlign;

    Canvas *_canvas;
    cairo_t *_context;
    cairo_path_t *_path;
    PangoLayout *_layout;

  private:
    static Napi::FunctionReference constructor;
    static Napi::FunctionReference _DOMMatrix;
    static Napi::FunctionReference _parseFont;

    void fillRect(const Napi::CallbackInfo &info);
    void arc(const Napi::CallbackInfo &info);
    void arcTo(const Napi::CallbackInfo &info);
    void beginPath(const Napi::CallbackInfo &info);
    void bezierCurveTo(const Napi::CallbackInfo &info);
    void clearRect(const Napi::CallbackInfo &info);
    void clip(const Napi::CallbackInfo &info);
    void closePath(const Napi::CallbackInfo &info);
    Napi::Value createImageData(const Napi::CallbackInfo &info);
    Napi::Value createLinearGradient(const Napi::CallbackInfo &info);
    Napi::Value createPattern(const Napi::CallbackInfo &info);
    Napi::Value createRadialGradient(const Napi::CallbackInfo &info);
    void drawImage(const Napi::CallbackInfo &info);
    void fill(const Napi::CallbackInfo &info);
    void fillText(const Napi::CallbackInfo &info);
    Napi::Value getImageData(const Napi::CallbackInfo &info);
    Napi::Value getLineDash(const Napi::CallbackInfo &info);
    void lineTo(const Napi::CallbackInfo &info);
    Napi::Value measureText(const Napi::CallbackInfo &info);
    void moveTo(const Napi::CallbackInfo &info);
    void putImageData(const Napi::CallbackInfo &info);
    void quadraticCurveTo(const Napi::CallbackInfo &info);
    void rect(const Napi::CallbackInfo &info);
    void restore(const Napi::CallbackInfo &info);
    void rotate(const Napi::CallbackInfo &info);
    void save(const Napi::CallbackInfo &info);
    void scale(const Napi::CallbackInfo &info);
    void setLineDash(const Napi::CallbackInfo &info);
    void setTransform(const Napi::CallbackInfo &info);
    void stroke(const Napi::CallbackInfo &info);
    void strokeRect(const Napi::CallbackInfo &info);
    void strokeText(const Napi::CallbackInfo &info);
    void transform(const Napi::CallbackInfo &info);
    void resetTransform(const Napi::CallbackInfo &info);
    void translate(const Napi::CallbackInfo &info);
    void ellipse(const Napi::CallbackInfo &info);

    Napi::Value IsPointInPath(const Napi::CallbackInfo &info);

    Napi::Value getfillStyle(const Napi::CallbackInfo &info);
    void setfillStyle(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getfont(const Napi::CallbackInfo &info);
    void setfont(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getglobalAlpha(const Napi::CallbackInfo &info);
    void setglobalAlpha(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getglobalCompositeOperation(const Napi::CallbackInfo &info);
    void setglobalCompositeOperation(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineCap(const Napi::CallbackInfo &info);
    void setlineCap(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineDashOffset(const Napi::CallbackInfo &info);
    void setlineDashOffset(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineJoin(const Napi::CallbackInfo &info);
    void setlineJoin(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineWidth(const Napi::CallbackInfo &info);
    void setlineWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getmiterLimit(const Napi::CallbackInfo &info);
    void setmiterLimit(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowBlur(const Napi::CallbackInfo &info);
    void setshadowBlur(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowColor(const Napi::CallbackInfo &info);
    void setshadowColor(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowOffsetX(const Napi::CallbackInfo &info);
    void setshadowOffsetX(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowOffsetY(const Napi::CallbackInfo &info);
    void setshadowOffsetY(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getstrokeStyle(const Napi::CallbackInfo &info);
    void setstrokeStyle(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value gettextAlign(const Napi::CallbackInfo &info);
    void settextAlign(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value gettextBaseline(const Napi::CallbackInfo &info);
    void settextBaseline(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getCanvas(const Napi::CallbackInfo &info);
};

}

#endif