#ifndef CAIRO_IMAGEDATA_H
#define CAIRO_IMAGEDATA_H


#include <napi.h>
#include <vector>

namespace cairocanvas
{

class ImageData: public Napi::ObjectWrap<ImageData> 
{
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(const Napi::CallbackInfo &info);
    static Napi::Object NewInstance(const Napi::CallbackInfo &info, Napi::Value width, Napi::Value height);
    ImageData(const Napi::CallbackInfo &info);

    std::vector<uint8_t> &getPixels();
    inline int getWidth() { return _width; }
    inline int getHeight() { return _height; }
    inline uint8_t *data() { return _data; }


  private:
    int _width;
    int _height;
    uint8_t *_data;

  private:
    static Napi::FunctionReference constructor;
    Napi::ObjectReference mImageDataRef;
    bool mHasImageDataWrite = false;
    std::vector<uint8_t> pixels;
    
    Napi::Value getData(const Napi::CallbackInfo &info);
    // void setData(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
};

}

#endif