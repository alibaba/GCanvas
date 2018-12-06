/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas_GFontStyle__
#define __GCanvas_GFontStyle__

#include <iostream>
#include <list>
#include <map>

namespace gcanvas
{
class GFontStyle final
{
public:
    GFontStyle(const char *font = nullptr, float ratio = 1.0);
    ~GFontStyle();

    enum class Style
    {
        NORMAL = 0x0001,
        ITALIC = 0x0002,
        OBLIQUE = 0x0004
    };

    enum class Variant
    {
        NORMAL = 0x0008,
        SMALL_CAPS = 0x0010
    };

    enum class Weight
    {
        LIGHTER = 0x0020,
        THIN = 0x0040,       // 100
        EXTRA_LIGHT = 0x0080, // 200
        LIGHT = 0x0100,      // 300
        NORMAL = 0x200,      // 400
        MEDIUM = 0x0400,     // 500
        SEMI_BOLD = 0x0800,   // 600
        BOLD = 0x1000,       // 700
        BOLDER = 0x2000,
        EXTRA_BOLD = 0x4000, // 800
        BLACK = 0x8000      // 900
    };

    Style GetStyle() { return mStyle; }

    Variant GetVariant() { return mVariant; }

    Weight GetWeight() { return mWeight; }

    float GetSize() { return mSize; }

    std::string GetFamily() const { return mFamily; }
    
    std::string GetName() const { return mFontName; }
private:
    void Initialize(const char *font);
    std::string mFontName;
    Style mStyle;
    Variant mVariant;
    Weight mWeight;
    float mSize;
    float mRatio;
    std::string mFamily;
};
}

#endif
