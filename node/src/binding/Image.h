#ifndef IMAGE_H
#define IMAGE_H
#include <napi.h>
#include <curl/curl.h>
#include <vector>
#include  "NodeBindingUtil.h"
#include "lodepng.h"

namespace NodeBinding
{   
class DownloadWorker : public Napi::AsyncWorker
{
public:
    DownloadWorker(Napi::Function &callback,
                   std::vector<unsigned char> &out, unsigned int &width, unsigned int &height) : Napi::AsyncWorker(callback),
                                                                                                 _pixels(out),
                                                                                                 _height(height),
                                                                                                 _width(width)
    {
    }

    void Execute();
    void OnOK();
    std::string url;
    void setOnErrorCallback(Napi::Function func);

private:
    Napi::FunctionReference onErrorCallback;
    std::vector<unsigned char> &_pixels;
    unsigned int &_width;
    unsigned int &_height;
    ChunkContent content;
};

class Image : public Napi::ObjectWrap<Image>
{
public:
    Image(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env, Napi::Object exports);
    virtual ~Image()
    {
        delete (mWorker);
        this->mWorker = nullptr;
    }
    int getWidth();
    int getHeight();
    std::vector<unsigned char>& getPixels();
private:
    std::vector<unsigned char> pixels;
    static Napi::FunctionReference constructor;
    Napi::Function onLoadCallback;
    Napi::Function onErrorCallback;
    std::string src;
    DownloadWorker *mWorker = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    Napi::Value getSrc(const Napi::CallbackInfo &info);
    void setSrc(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnLoad(const Napi::CallbackInfo &info);
    void setOnLoad(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getOnError(const Napi::CallbackInfo &info);
    void setOnError(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getWidth(const Napi::CallbackInfo &info);
    void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getHeight(const Napi::CallbackInfo &info);
    void setHeight(const Napi::CallbackInfo &info, const Napi::Value &value);
};
} // namespace NodeBinding

#endif