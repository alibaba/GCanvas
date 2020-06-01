/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
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

    struct ColorStop
    {
        float offset;
        std::string color;

        ColorStop(float offset, std::string color)
        {
            this->offset = offset;
            this->color = color;
        }
    };

struct less_than_key
{
    inline bool operator() (const ColorStop& colorStop1, const ColorStop& colorStop2)
    {
        return (colorStop1.offset < colorStop2.offset);
    }
};
    class Gradient : public Napi::ObjectWrap<Gradient>
    {
    public:
        Gradient(const Napi::CallbackInfo &info);
        static void Init(Napi::Env env);
        static Napi::Object NewInstance(Napi::Env env, const Napi::CallbackInfo &info);
        std::shared_ptr<LinearGradientInfo> mLinearGradientInfo = nullptr;
        std::shared_ptr<RadialGradientInfo> mRadialGradientInfo = nullptr;
        int getCount() { return this->offsets.size(); }
        const std::vector<ColorStop> &getColorStops();

    private:
        static Napi::FunctionReference constructor;
        void addColorStop(const Napi::CallbackInfo &info);
        std::vector<ColorStop> offsets;

        bool compareInterval(ColorStop colorStop1, ColorStop colorStop2)
        {
            return (colorStop1.offset < colorStop2.offset);
        }
    };
} // namespace NodeBinding
#endif