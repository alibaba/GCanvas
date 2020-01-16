#ifndef IMG_DATA_H
#define IMG_DATA_H
#include <napi.h>
namespace NodeBinding
{
class ImageData : public Napi::ObjectWrap<ImageData>
{
public:
    ImageData(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::Value width, const Napi::Value height);
    std::vector<u_int8_t> &getPixles();
    int getWidth(); 
    int getHeight();

private:
    static Napi::FunctionReference constructor;
    int width = 0;
    int height = 0;
    std::vector<u_int8_t> pixels;
    Napi::Value getData(const Napi::CallbackInfo &info);
    void setData(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
    void setHeight(const Napi::CallbackInfo &info, const Napi::Value &value);
};
} // namespace NodeBinding

#endif