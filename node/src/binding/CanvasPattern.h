#ifndef IMG_PATTERN_H
#define IMG_PATTERN_H
#include <napi.h>
#include <iostream>
#include "Image.h"
namespace NodeBinding
{
class ImagePattern : public Napi::ObjectWrap<ImagePattern>
{
public:
    ImagePattern(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::Value repetition);
    std::shared_ptr<Image> content;
    const std::string& getRepetition();
private:
    static Napi::FunctionReference constructor;
   std::string repetition;
};
} // namespace NodeBinding
#endif