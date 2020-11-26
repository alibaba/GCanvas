#include "CairoImage.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <csetjmp>
#include <iostream>


namespace cairocanvas
{

typedef struct {
  unsigned len;
  uint8_t *buf;
} read_closure_t;


struct canvas_jpeg_error_mgr: jpeg_error_mgr {
  Image* image;
  jmp_buf setjmp_buffer;
};

Napi::FunctionReference Image::constructor;

void Image::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func = DefineClass(env, "Image", {
    InstanceAccessor("src", &Image::getSource, &Image::setSource),
    InstanceAccessor("width", &Image::getWidth, nullptr),
    InstanceAccessor("height", &Image::getHeight, nullptr),
    InstanceAccessor("onload", &Image::getOnLoadCallback, &Image::setOnLoadCallback),
    InstanceAccessor("onerror", &Image::getOnErrorCallback, &Image::setOnErrorCallback),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("Image", func);
}


Napi::Object Image::NewInstance(Napi::Env env)
{
  Napi::Object obj = constructor.New({});
  obj.Set("name", Napi::String::New(env, "image"));
  return obj;
}

Image::Image(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Image>(info)
{
  _data = nullptr;
  _data_len = 0;
  _surface = NULL;
  width = height = 0;
}

Image::~Image()
{
  clearData();
  mImageMemCached = nullptr;
}

Napi::Value Image::getSource(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), mSrc);
}

void Image::DownloadCallback(Napi::Env env, uint8_t *data, size_t size, std::string errMsg )
{
  if( data != nullptr && size > 0 )
  {
    //decode 
    cairo_status_t status = loadFromBuffer(data, size);
    if( CAIRO_STATUS_SUCCESS == status )
    {
      loaded();

      //callback
      if( mOnLoadCallback )
      {
        mOnLoadCallback.Call({env.Undefined()});
      }
    }
    else
    {
      if( mOnErrorCallback )
      {
        mOnErrorCallback.Call({Napi::String::New(env, "load image error")});
      }
    }
        
  }
  else
  {
    if( mOnErrorCallback )
    {
      mOnErrorCallback.Call({Napi::String::New(env, errMsg)});
    }
  }
}

void Image::setSource(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    NodeBinding::checkArgs(info, 1);
    mSrc = value.As<Napi::String>().Utf8Value();
    mImageMemCached=std::make_shared<ImagePixelInfo>();
    if (!mDownloadImageWorker)
    {
      mDownloadImageWorker = new ImageAsyncWorker(info.Env(), mSrc, mImageMemCached, std::bind(&Image::DownloadCallback, this,  std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3,  std::placeholders::_4) );
    }

    mDownloadImageWorker->Queue();
}

Napi::Value Image::getOnLoadCallback(const Napi::CallbackInfo &info)
{
  return mOnLoadCallback? mOnLoadCallback.Value() : info.Env().Undefined();
}
Napi::Value Image::getOnErrorCallback(const Napi::CallbackInfo &info)
{
  return mOnErrorCallback? mOnErrorCallback.Value() : info.Env().Undefined();
}

void Image::setOnLoadCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  NodeBinding::checkArgs(info, 1);
  mOnLoadCallback = Napi::Persistent(value.As<Napi::Function>());
}

void Image::setOnErrorCallback(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  NodeBinding::checkArgs(info, 1);
  mOnErrorCallback = Napi::Persistent(value.As<Napi::Function>());
}

Napi::Value Image::getWidth(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), width);
}
Napi::Value Image::getHeight(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), height);
}

int Image::getWidth()
{
  return width;
}
int Image::getHeight()
{
  return height;
}

std::vector<unsigned char> &Image::getPixels()
{
  if (mImageMemCached)
  {
    //Cairo ARGB TO A
    // NodeBinding::pixelsConvertRGBAToARBG(&mImageMemCached->getPixels()[0], getWidth(), getHeight());
    return mImageMemCached->getPixels();
  }
  else
  {
    return emptyPixels;
  }
}

 cairo_surface_t* Image::getSurface()
 {
  if( !_surface ) 
  {
    ImagePixelInfo *imageCached = mImageMemCached.get();
    if (imageCached) 
    {
      unsigned int len = imageCached->getPixels().size();
      unsigned char *buf = imageCached->getPixels().data();

      int width = imageCached->width;
      int height = imageCached->height;
      int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);

      //RGBA buffer
      unsigned char * buf2 = imageCached->getPixels().data();
      NodeBinding::pixelsConvertRGBAToARGB(buf2, width, height);

      _surface = cairo_image_surface_create_for_data( 
        buf2,
        CAIRO_FORMAT_ARGB32,
        width, 
        height, 
        stride);
    }
  }
  return _surface;
}

void Image::clearData()
{
  if (_surface) 
  {
    cairo_surface_destroy(_surface);
    Napi::MemoryManagement::AdjustExternalMemory(Env(), -_data_len);
    _data_len = 0;
    _surface = NULL;
  }

  delete[] _data;
  _data = nullptr;
  width = height = 0;
}

// Load image data form buffer
cairo_status_t Image::loadFromBuffer(uint8_t *buf, unsigned len) 
{
  uint8_t data[4] = {0};
  memcpy(data, buf, (len < 4 ? len : 4) * sizeof(uint8_t));

  if (isPNG(data)) 
  {
    return loadPNGFromBuffer(buf);
  }

  if (isJPEG(data)) 
  {
    return loadJPEGFromBuffer(buf, len);
  }

  std::cout << "Unsupported image type" << std::endl; 
  return CAIRO_STATUS_READ_ERROR;
}


Image::type Image::extension(const char *filename) 
{
  size_t len = strlen(filename);
  filename += len;
  if (len >= 5 && 0 == strcmp(".jpeg", filename - 5)) return Image::JPEG;
  if (len >= 4 && 0 == strcmp(".jpg", filename - 4)) return Image::JPEG;
  if (len >= 4 && 0 == strcmp(".png", filename - 4)) return Image::PNG;
  return Image::UNKNOWN;
}

int Image::isJPEG(uint8_t *data) 
{
  return 0xff == data[0] && 0xd8 == data[1];
}

int Image::isPNG(uint8_t *data) 
{
  return 'P' == data[1] && 'N' == data[2] && 'G' == data[3];
}

// Load PNG data from buffer
cairo_status_t Image::loadPNGFromBuffer(uint8_t *buf) 
{
  read_closure_t closure;
  closure.len = 0;
  closure.buf = buf;
  _surface = cairo_image_surface_create_from_png_stream(readPNG, &closure);
  cairo_status_t status = cairo_surface_status(_surface);
  if (status) return status;
  return CAIRO_STATUS_SUCCESS;
}

// Read PNG data
cairo_status_t Image::readPNG(void *c, uint8_t *data, unsigned int len) 
{
  read_closure_t *closure = (read_closure_t *) c;
  memcpy(data, closure->buf + closure->len, len);
  closure->len += len;
  return CAIRO_STATUS_SUCCESS;
}

static void canvas_jpeg_error_exit(j_common_ptr cinfo) 
{
  canvas_jpeg_error_mgr *cjerr = static_cast<canvas_jpeg_error_mgr*>(cinfo->err);
  cjerr->output_message(cinfo);
  // Return control to the setjmp point
  longjmp(cjerr->setjmp_buffer, 1);
}

// Capture libjpeg errors instead of writing stdout
static void canvas_jpeg_output_message(j_common_ptr cinfo) 
{
  canvas_jpeg_error_mgr *cjerr = static_cast<canvas_jpeg_error_mgr*>(cinfo->err);
  char buff[JMSG_LENGTH_MAX];
  cjerr->format_message(cinfo, buff);
  // (Only the last message will be returned to JS land.)
//   cjerr->image->errorInfo.set(buff);
}


// Load jpeg from buffer.
cairo_status_t
Image::loadJPEGFromBuffer(uint8_t *buf, unsigned len) 
{
  // JPEG setup
  struct jpeg_decompress_struct args;
  struct canvas_jpeg_error_mgr err;

  err.image = this;
  args.err = jpeg_std_error(&err);
  args.err->error_exit = canvas_jpeg_error_exit;
  args.err->output_message = canvas_jpeg_output_message;

  // Establish the setjmp return context for canvas_jpeg_error_exit to use
  if (setjmp(err.setjmp_buffer)) {
    // If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object, close the input file, and return.
    jpeg_destroy_decompress(&args);
    return CAIRO_STATUS_READ_ERROR;
  }

  jpeg_create_decompress(&args);

  jpeg_mem_src(&args, buf, len);

  jpeg_read_header(&args, 1);
  jpeg_start_decompress(&args);
  width = naturalWidth = args.output_width;
  height = naturalHeight = args.output_height;

  return decodeJPEGIntoSurface(&args);
}

cairo_status_t
Image::decodeJPEGIntoSurface(jpeg_decompress_struct *args) 
{
  cairo_status_t status = CAIRO_STATUS_SUCCESS;

  uint8_t *data = new uint8_t[naturalWidth * naturalHeight * 4];
  if (!data) {
    jpeg_abort_decompress(args);
    jpeg_destroy_decompress(args);
    std::cout << "decodeJPEGIntoSurface data malloc failed" << std::endl;
    return CAIRO_STATUS_NO_MEMORY;
  }

  uint8_t *src = new uint8_t[naturalWidth * args->output_components];
  if (!src) {
    free(data);
    jpeg_abort_decompress(args);
    jpeg_destroy_decompress(args);
    std::cout << "decodeJPEGIntoSurface src malloc failed" << std::endl;
    return CAIRO_STATUS_NO_MEMORY;
  }

  // These are the three main cases to handle. libjpeg converts YCCK to CMYK
  // and YCbCr to RGB by default.
  switch (args->out_color_space) {
    case JCS_CMYK:
      jpegToARGB(args, data, src, [](uint8_t const* src) {
        uint16_t k = static_cast<uint16_t>(src[3]);
        uint8_t r = k * src[0] / 255;
        uint8_t g = k * src[1] / 255;
        uint8_t b = k * src[2] / 255;
        return 255 << 24 | r << 16 | g << 8 | b;
      });
      break;
    case JCS_RGB:
      jpegToARGB(args, data, src, [](uint8_t const* src) {
        uint8_t r = src[0], g = src[1], b = src[2];
        return 255 << 24 | r << 16 | g << 8 | b;
      });
      break;
    case JCS_GRAYSCALE:
      jpegToARGB(args, data, src, [](uint8_t const* src) {
        uint8_t v = src[0];
        return 255 << 24 | v << 16 | v << 8 | v;
      });
      break;
    default:
      std::cout << "Unsupported JPEG encoding" << std::endl;
      status = CAIRO_STATUS_READ_ERROR;
      break;
  }

  if (!status) {
    _surface = cairo_image_surface_create_for_data(
      data
      , CAIRO_FORMAT_ARGB32
      , naturalWidth
      , naturalHeight
      , cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, naturalWidth));
  }

  jpeg_finish_decompress(args);
  jpeg_destroy_decompress(args);
  status = cairo_surface_status(_surface);

  delete[] src;

  if (status) {
    delete[] data;
    return status;
  }

  _data = data;

  return CAIRO_STATUS_SUCCESS;
}


void Image::jpegToARGB(jpeg_decompress_struct* args, uint8_t* data, uint8_t* src, JPEGDecodeL decode) 
{
  int stride = naturalWidth * 4;
  for (int y = 0; y < naturalHeight; ++y) {
    jpeg_read_scanlines(args, &src, 1);
    uint32_t *row = (uint32_t*)(data + stride * y);
    for (int x = 0; x < naturalWidth; ++x) {
      int bx = args->output_components * x;
      row[x] = decode(src + bx);
    }
  }
}

void Image::loaded() 
{
  state = COMPLETE;
  width = naturalWidth = cairo_image_surface_get_width(_surface);
  height = naturalHeight = cairo_image_surface_get_height(_surface);
  _data_len = naturalHeight * cairo_image_surface_get_stride(_surface);

  Napi::MemoryManagement::AdjustExternalMemory(Env(), _data_len);
}

}