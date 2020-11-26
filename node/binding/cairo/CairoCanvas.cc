#include "CairoCanvas.h"

#include <algorithm> // std::min
#include <assert.h>
#include "CairoCanvasRenderingContext2d.h"
#include <cstring>
#include <cctype>
#include <ctime>
#include <glib.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>
#include "CairoImageBackend.h"
#include "NodeBindingUtil.h"
#include "closure.h"
#include "PNG.h"

using namespace std;

namespace cairocanvas
{
Napi::FunctionReference Canvas::constructor;

std::vector<FontFace> font_face_list;

void Canvas::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Canvas", {
    // InstanceAccessor("type", &Canvas::getType, nullptr),
    // InstanceAccessor("stride", &Canvas::getStride, nullptr),
    InstanceAccessor("width", &Canvas::getWidth, &Canvas::setWidth),
    InstanceAccessor("height", &Canvas::getHeight, &Canvas::setHeight),
    InstanceMethod("getContext", &Canvas::getContext),
    InstanceMethod("createPNGStreamSync", &Canvas::createPNGStreamSync),
    InstanceMethod("createJPGStreamSync", &Canvas::createJPGStreamSync),
    InstanceMethod("toBuffer", &Canvas::toBuffer),
    InstanceMethod("toDataURL", &Canvas::toDataURL),

  });
  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
  return;
}

Napi::Object Canvas::NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2)
{
  Napi::Object obj = constructor.New({arg, arg2});
  obj.Set("name", Napi::String::New(env, "canvas"));
  Canvas *canvas = Napi::ObjectWrap<Canvas>::Unwrap(obj);
  canvas->mCanvasRef = Napi::ObjectReference::New(obj);
  return obj;
}


Canvas::Canvas(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Canvas>(info), mDataRaw(nullptr)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  NodeBinding::checkArgs(info, 2);
  mWidth = info[0].As<Napi::Number>().Int32Value();
  mHeight = info[1].As<Napi::Number>().Int32Value();

  mBackend = new CairoImageBackend(mWidth, mHeight);

  if( !mBackend->isSurfaceValid() )
  {
    delete mBackend;
    return;
  }

  mBackend->setCanvas(this);
}

Canvas::~Canvas()
{

}

cairo_t* Canvas::createCairoContext()
{
  cairo_t* ret = cairo_create(surface());
  cairo_set_line_width(ret, 1); // Cairo defaults to 2
  int stride = cairo_image_surface_get_stride(surface());
  return ret;
}

Napi::Value Canvas::getWidth(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), mWidth);
}

void Canvas::setWidth(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  if( info.Length() > 0 && info[0].IsNumber() )
  {
    int w = backend()->getWidth();

    int newWidth = info[0].As<Napi::Number>().Int32Value();
    if( newWidth > 0 && newWidth != w )
    {
      backend()->setWidth(newWidth);
      backend()->recreateSurface();

      mContext2dRef.Reset();
    }
  }
}

Napi::Value Canvas::getHeight(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), mHeight);
}

void Canvas::setHeight(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  if( info.Length() > 0 && info[0].IsNumber() )
  {
    int h = backend()->getHeight();

    int newHeight = info[0].As<Napi::Number>().Int32Value();
    if( newHeight > 0 && newHeight != h )
    {
      backend()->setHeight(newHeight);
      backend()->recreateSurface();
      
      mContext2dRef.Reset();
    }
  }
}

Napi::Value Canvas::getContext(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  NodeBinding::checkArgs(info, 1);
  std::string type = info[0].As<Napi::String>().Utf8Value();

  std::cout << "Canvas::getContext('" <<  type << "')" <<  std::endl;

  if (type == "2d")
  {
    if (mContext2dRef.IsEmpty())
    {
      Napi::Object obj = Context2d::NewInstance(info);
      Context2d *context2d = Napi::ObjectWrap<Context2d>::Unwrap(obj);
      context2d->setupContext2d( this );
      //save reference
      mContext2dRef = Napi::ObjectReference::New(obj);
      return obj;
    }
    else
    {
      return mContext2dRef.Value();
    }
  }
  else
  {
    napi_throw_error(env, "", "type is invalid");
    return Napi::Object::New(env);
  }
}
Napi::Value Canvas::createJPGStreamSync(const Napi::CallbackInfo &info)
{
  NodeBinding::checkArgs(info, 2);
  unsigned long size = 0;
  Napi::Buffer<unsigned char> buffer = getJPGBuffer(info, size);
  if (size > 0)
  {
    Napi::Function callback = info[0].As<Napi::Function>();
    //handlescope 表示作用域,一般调用callback函数时使用
    Napi::HandleScope scope(info.Env());
    callback.Call({info.Env().Null(), buffer, Napi::Number::New(info.Env(), size)});
  }
  else
  {
    Napi::Function callback = info[0].As<Napi::Function>();
    Napi::HandleScope scope(info.Env());
    callback.Call({Napi::String::New(Env(), "createJPGStreamFail"), info.Env().Null(), info.Env().Null()});
  }
  return info.Env().Undefined();
}

Napi::Value Canvas::createPNGStreamSync(const Napi::CallbackInfo &info)
{
  NodeBinding::checkArgs(info, 2);
  unsigned long size = 0;
  Napi::Buffer<unsigned char> buffer = getPNGBuffer(info, size);

  if (size > 0)
  {
    Napi::Function callback = info[0].As<Napi::Function>();
    //handlescope 表示作用域,一般调用callback函数时使用
    Napi::HandleScope scope(info.Env());
    callback.Call({info.Env().Null(), buffer, Napi::Number::New(info.Env(), size)});
  }
  else
  {
    Napi::Function callback = info[0].As<Napi::Function>();
    Napi::HandleScope scope(info.Env());
    callback.Call({Napi::String::New(Env(), "createPNGStreamFail"), info.Env().Null(), info.Env().Null()});
  }
  return info.Env().Undefined();
}

Napi::Buffer<unsigned char> Canvas::getPNGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
{
  cairo_surface_t *s = surface();
  cairo_surface_flush(s);
  const unsigned char *data = cairo_image_surface_get_data(s);

  PngClosure closure(this);

  try
  {
    cairo_status_t status = canvas_write_to_png_stream(s, PngClosure::writeVec, &closure);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

  size = closure.vec.size();
  
  if( !closure.vec.empty() )
  {
    return Napi::Buffer<unsigned char>::Copy(info.Env(), &closure.vec[0], closure.vec.size());
  }
  else
  {
    return Napi::Buffer<unsigned char>::New(info.Env(), nullptr, 0);
  }
}

Napi::Buffer<unsigned char> Canvas::getJPGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
{
  cairo_surface_t *s = surface();
  cairo_surface_flush(s);
  unsigned char *data = cairo_image_surface_get_data(s);

  //TODO check argb only
  unsigned int width = cairo_image_surface_get_width(s);
  unsigned int height = cairo_image_surface_get_height(s);
  
  size = 4 * width * height;
  NodeBinding::pixelsConvertARGBToRGBA(( unsigned char *)data, width, height);

  unsigned char *jpegBuffer = nullptr;
  NodeBinding::encodeJPEGInBuffer(&jpegBuffer, size, data, width, height);
  return Napi::Buffer<unsigned char>::Copy(info.Env(), jpegBuffer, size);
}

Napi::Buffer<unsigned char> Canvas::getRawDataBuffer(const Napi::CallbackInfo &info, unsigned long &size)
{
  if (mDataRaw == nullptr)
  {
    size = 4 * mWidth * mHeight;
    mDataRaw = new unsigned char[size];
  }

  cairo_surface_t *s = surface();
  cairo_surface_flush(s);
  mDataRaw = cairo_image_surface_get_data(s);
  unsigned char *jpegBuffer = nullptr;

  NodeBinding::encodeJPEGInBuffer(&jpegBuffer, size, mDataRaw, mWidth, mHeight);
  return Napi::Buffer<unsigned char>::Copy(info.Env(), jpegBuffer, size);
}
Napi::Value Canvas::toBuffer(const Napi::CallbackInfo &info)
{
  unsigned long size = 0;
  //默认输出png 编码
  if (info.Length() == 0)
  {
    return getPNGBuffer(info, size);
  }
  else
  {
    Napi::Buffer<unsigned char> ret;
    if (info.Length() == 1)
    {
        std::string mimeType = info[0].As<Napi::String>().Utf8Value();
        if (mimeType == "image/png")
        {
            ret = getPNGBuffer(info, size);
        }
        else if (mimeType == "image/jpeg")
        {
            ret = getJPGBuffer(info, size);
        }
        else if (mimeType == "raw")
        {
            ret = getRawDataBuffer(info, size);
        }
    }
    if (size <= 0)
    {
        return info.Env().Null();
    }
    else
    {
        return ret;
    }
  }
}
Napi::Value Canvas::toDataURL(const Napi::CallbackInfo &info)
{
  if( mWidth == 0 || mHeight == 0 )
  {
    return Napi::String::New(info.Env(), "data:,");
  }

  bool isJPEG = false; //default output
  float jepgQuality = 1.0;
  unsigned long size = 0;
  if (info.Length() >= 1)
  {
    std::string mimeType = info[0].As<Napi::String>().Utf8Value();
    if (mimeType == "image/png")
    {
      isJPEG = false;
    }
    else if (mimeType == "image/jpeg")
    {
      isJPEG = true;
    }

    if( isJPEG && info.Length() >= 2 && info[1].IsNumber() )
    {
      jepgQuality = info[1].As<Napi::Number>().FloatValue();
    }
  }

  cairo_surface_t *s = surface();
  cairo_surface_flush(s);
  unsigned char *data = cairo_image_surface_get_data(s);
  //TODO check argb only
  unsigned int width = cairo_image_surface_get_width(s);
  unsigned int height = cairo_image_surface_get_height(s);
  
  if ( isJPEG )
  {
    size = 4 * width * height;
    NodeBinding::pixelsConvertARGBToRGBA(( unsigned char *)data, width, height);

    unsigned char *jpegBuffer = nullptr;
    NodeBinding::encodeJPEGInBuffer(&jpegBuffer, size, data, width, height, jepgQuality);

    //jepgbuffer & size
    std::string jpegStr((const char*)jpegBuffer, (size_t)size);

    std::string base64Str = "data:image/jpeg;base64,";

    std::string tmpStr;
    NodeBinding::toBase64(tmpStr, jpegStr);
  
    std::cout << "JPEG in: " << jpegStr.size() << ",out:" << tmpStr.size() << std::endl;

    base64Str.append(tmpStr);
    return Napi::Buffer<unsigned char>::Copy(info.Env(), (unsigned char *)base64Str.c_str(), base64Str.size());
  }
  else
  {
    PngClosure closure(this);
    try
    {
      cairo_status_t status = canvas_write_to_png_stream(s, PngClosure::writeVec, &closure);
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }

    size = closure.vec.size();
    std::string base64Str = "data:image/png;base64,";

    if( size > 0 )
    {
      // &closure.vec[0],  closure.vec.size()
      std::vector<unsigned char> base64Vec;
      NodeBinding::toBase64(base64Vec, closure.vec);

      std::cout << "PNG in: " << closure.vec.size() << ",out:" << base64Vec.size() << std::endl;
      base64Str.append((const char*)(&base64Vec[0]), base64Vec.size());
    }
    return Napi::Buffer<unsigned char>::Copy(info.Env(), (unsigned char *)base64Str.c_str(), base64Str.size());
  }
}

/*
 * Get a PangoStyle from a CSS string (like "italic")
 */

PangoStyle
Canvas::GetStyleFromCSSString(const char *style) {
  PangoStyle s = PANGO_STYLE_NORMAL;

  if (strlen(style) > 0) {
    if (0 == strcmp("italic", style)) {
      s = PANGO_STYLE_ITALIC;
    } else if (0 == strcmp("oblique", style)) {
      s = PANGO_STYLE_OBLIQUE;
    }
  }

  return s;
}

/*
 * Get a PangoWeight from a CSS string ("bold", "100", etc)
 */

PangoWeight
Canvas::GetWeightFromCSSString(const char *weight) {
  PangoWeight w = PANGO_WEIGHT_NORMAL;

  if (strlen(weight) > 0) {
    if (0 == strcmp("bold", weight)) {
      w = PANGO_WEIGHT_BOLD;
    } else if (0 == strcmp("100", weight)) {
      w = PANGO_WEIGHT_THIN;
    } else if (0 == strcmp("200", weight)) {
      w = PANGO_WEIGHT_ULTRALIGHT;
    } else if (0 == strcmp("300", weight)) {
      w = PANGO_WEIGHT_LIGHT;
    } else if (0 == strcmp("400", weight)) {
      w = PANGO_WEIGHT_NORMAL;
    } else if (0 == strcmp("500", weight)) {
      w = PANGO_WEIGHT_MEDIUM;
    } else if (0 == strcmp("600", weight)) {
      w = PANGO_WEIGHT_SEMIBOLD;
    } else if (0 == strcmp("700", weight)) {
      w = PANGO_WEIGHT_BOLD;
    } else if (0 == strcmp("800", weight)) {
      w = PANGO_WEIGHT_ULTRABOLD;
    } else if (0 == strcmp("900", weight)) {
      w = PANGO_WEIGHT_HEAVY;
    }
  }

  return w;
}

bool streq_casein(std::string& str1, std::string& str2) {
  return str1.size() == str2.size() && std::equal(str1.begin(), str1.end(), str2.begin(), [](char& c1, char& c2) {
    return c1 == c2 || std::toupper(c1) == std::toupper(c2);
  });
}

/*
 * Given a user description, return a description that will select the
 * font either from the system or @font-face
 */

PangoFontDescription *
Canvas::ResolveFontDescription(const PangoFontDescription *desc) {
  // One of the user-specified families could map to multiple SFNT family names
  // if someone registered two different fonts under the same family name.
  // https://drafts.csswg.org/css-fonts-3/#font-style-matching
  FontFace best;
  istringstream families(pango_font_description_get_family(desc));
  unordered_set<string> seen_families;
  string resolved_families;
  bool first = true;

  for (string family; getline(families, family, ','); ) {
    string renamed_families;
    for (auto& ff : font_face_list) {
      string pangofamily = string(pango_font_description_get_family(ff.user_desc));
      if (streq_casein(family, pangofamily)) {
        const char* sys_desc_family_name = pango_font_description_get_family(ff.sys_desc);
        bool unseen = seen_families.find(sys_desc_family_name) == seen_families.end();

        // Avoid sending duplicate SFNT font names due to a bug in Pango for macOS:
        // https://bugzilla.gnome.org/show_bug.cgi?id=762873
        if (unseen) {
          seen_families.insert(sys_desc_family_name);
          if (renamed_families.size()) renamed_families += ',';
          renamed_families += sys_desc_family_name;
        }

        if (first && (best.user_desc == nullptr || pango_font_description_better_match(desc, best.user_desc, ff.user_desc))) {
          best = ff;
        }
      }
    }

    if (resolved_families.size()) resolved_families += ',';
    resolved_families += renamed_families.size() ? renamed_families : family;
    first = false;
  }

  PangoFontDescription* ret = pango_font_description_copy(best.sys_desc ? best.sys_desc : desc);
  pango_font_description_set_family(ret, resolved_families.c_str());

  return ret;
}


}