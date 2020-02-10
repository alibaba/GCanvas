#ifndef GRADIENT_H
#define GRADIENT_H
#include <napi.h>
#include <vector>
namespace NodeBinding
{
struct LinearGradientInfo
{
    float startX;
    float startY;
    float endX;
    float endY;
};

struct RadialGradientInfo
{
    float startX;
    float startY;
    float startR;
    float endX;
    float endY;
    float endR;
};
class Gradient : public Napi::ObjectWrap<Gradient>
{
public:
    Gradient(const Napi::CallbackInfo &info);
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const Napi::CallbackInfo &info);
    std::vector<float> &getOffsets() { return this->offsets; }
    std::vector<std::string> &getColors() { return this->colors; }
    std::shared_ptr<LinearGradientInfo> mLinearGradientInfo = nullptr;
    std::shared_ptr<RadialGradientInfo> mRadialGradientInfo = nullptr;

    int getCount() { return this->offsets.size(); }

private:
    static Napi::FunctionReference constructor;
    void addColorStop(const Napi::CallbackInfo &info);
    std::vector<float> offsets;
    std::vector<std::string> colors;
};
} // namespace NodeBinding
#endif