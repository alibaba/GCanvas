/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <cstdlib>
#include "GFontStyle.h"
#include "GStrSeparator.h"

namespace gcanvas
{
GFontStyle::GFontStyle(const char *font)
{
    mStyle = Style::NORMAL;
    mVariant = Variant::NORMAL;
    mWeight = Weight::NORMAL;
    mSize = 10;
    mFamily = "sans-serif";

    if (font != nullptr)
    {
        Initialize(font);
    }
}

GFontStyle::~GFontStyle() {}

void GFontStyle::Initialize(const char *font)
{
    std::string newFont(font);
    GStrSeparator sepStr;
    short ct = sepStr.SepStrBySpace((char *)newFont.c_str(), 5);
    char **parts = sepStr.GetSepArray();
    int index = 0;

    while (index < ct)
    {
        if (strcmp("normal", parts[index]) == 0 ||
            strcmp("400", parts[index]) == 0)
        {
            // "normal" should be skipped in any case
        }
        else if (strcmp("oblique", parts[index]) == 0)
        {
            mStyle = Style::OBLIQUE;
        }
        else if (strcmp("italic", parts[index]) == 0)
        {
            mStyle = Style::ITALIC;
        }
        else if (strcmp("small-caps", parts[index]) == 0)
        {
            mVariant = Variant::SMALL_CAPS;
        }
        else if (strcmp("bold", parts[index]) == 0 ||
                 strcmp("700", parts[index]) == 0)
        {
            mWeight = Weight::BOLD;
        }
        else if (strcmp("bolder", parts[index]) == 0)
        {
            mWeight = Weight::BOLDER;
        }
        else if (strcmp("lighter", parts[index]) == 0)
        {
            mWeight = Weight::LIGHTER;
        }
        else if (strcmp("100", parts[index]) == 0)
        {
            mWeight = Weight::THIN;
        }
        else if (strcmp("200", parts[index]) == 0)
        {
            mWeight = Weight::EXTRA_LIGHT;
        }
        else if (strcmp("300", parts[index]) == 0)
        {
            mWeight = Weight::LIGHT;
        }
        else if (strcmp("500", parts[index]) == 0)
        {
            mWeight = Weight::MEDIUM;
        }
        else if (strcmp("600", parts[index]) == 0)
        {
            mWeight = Weight::SEMI_BOLD;
        }
        else if (strcmp("800", parts[index]) == 0)
        {
            mWeight = Weight::EXTRA_BOLD;
        }
        else if (strcmp("900", parts[index]) == 0)
        {
            mWeight = Weight::BLACK;
        }
        else
        {
            std::string fontSize(parts[index]);
            auto pos = fontSize.find("px");
            if (pos > 0)
            {
                auto len = strlen(parts[index]);
                if (pos == (len - 2))
                {
                    char *endPtr = nullptr;
                    mSize = strtof(parts[index], &endPtr);

                    if (++index < ct)
                    {
                        mFamily = parts[index];
                    }
                }
            }
        }

        ++index;
    }
}
}
