#ifndef CAIRO_IMAGE_H
#define CAIRO_IMAGE_H

#include <cairo.h>
#include <functional>
#include <napi.h>
#include <stdint.h> // node < 7 uses libstdc++ on macOS which lacks complete c++11

#include <curl/curl.h>
#include <vector>
#include "NodeBindingUtil.h"
#include "lodepng.h"
#include "ImagePixelInfo.h"
#include "ImageAsyncWorker.h"

#include "jpeglib.h"
#include "jerror.h"
namespace cairocanvas
{
using JPEGDecodeL = std::function<uint32_t (uint8_t* const src)>;

extern std::shared_ptr<ImagePixelInfo> findCacheByUrl(const std::string &url);

class Image: public Napi::ObjectWrap<Image> 
{
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env);

    Image(const Napi::CallbackInfo &info);
    virtual ~Image();

    int getWidth();
    int getHeight();
    std::vector<unsigned char> &getPixels();

    cairo_surface_t* getSurface();
  private:
    static Napi::FunctionReference constructor;
    std::string mSrc;
    Napi::FunctionReference mOnErrorCallback;
    Napi::FunctionReference mOnLoadCallback;

    ImageAsyncWorker *mDownloadImageWorker = nullptr;
    std::vector<unsigned char> emptyPixels;
    std::shared_ptr<ImagePixelInfo> mImageMemCached;

    void DownloadCallback(Napi::Env env, uint8_t *data, size_t size, std::string errMsg );

    Napi::Value getSource(const Napi::CallbackInfo &info);
    void setSource(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnLoadCallback(const Napi::CallbackInfo &info);
    void setOnLoadCallback(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnErrorCallback(const Napi::CallbackInfo &info);
    void setOnErrorCallback(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);


    inline uint8_t *data(){ return cairo_image_surface_get_data(_surface); }
    inline int stride(){ return cairo_image_surface_get_stride(_surface); }
    static int isPNG(uint8_t *data);
    static int isJPEG(uint8_t *data);
    static cairo_status_t readPNG(void *closure, unsigned char *data, unsigned len);

    cairo_status_t loadFromBuffer(uint8_t *buf, unsigned len);
    cairo_status_t loadPNGFromBuffer(uint8_t *buf);
    cairo_status_t loadPNG();

    cairo_status_t loadJPEGFromBuffer(uint8_t *buf, unsigned len);
    void jpegToARGB(jpeg_decompress_struct* args, uint8_t* data, uint8_t* src, JPEGDecodeL decode);
    cairo_status_t decodeJPEGIntoSurface(jpeg_decompress_struct *info);
    
    void loaded();

    //cairo data
    cairo_surface_t *_surface;
    uint8_t *_data = nullptr;
    int _data_len;
    int width, height;
    int naturalWidth, naturalHeight;

    void clearData();


    enum {
        DEFAULT
      , LOADING
      , COMPLETE
    } state;

    enum data_mode_t {
        DATA_IMAGE = 1
      , DATA_MIME = 2
    } data_mode;

    typedef enum {
        UNKNOWN
      , JPEG
      , PNG
    } type;

    static type extension(const char *filename);

};

}

#endif