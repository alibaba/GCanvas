/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <ctype.h>
#include <cstring>

#include "GFontFamily.h"

using namespace std;

namespace gcanvas
{
#if defined(__unix__) || defined(__APPLE__)
GFontFamily::GFontFamily(std::list< const char * > &fontFamily)
{
    matchFontFamily(fontFamily);
}

const char *GFontFamily::MatchFamilyStyle(GFontStyle &fontStyle)
{
    bool italic = false;
    bool bold = false;
    bool light = false;

    int style = (int)fontStyle.GetStyle();
    if ((style & (int)GFontStyle::Style::ITALIC) ||
        (style & (int)GFontStyle::Style::OBLIQUE))
    {
        italic = true;
    }

    int weight = (int)fontStyle.GetWeight();
    if (weight)
    {
        if (weight > static_cast<int>(GFontStyle::Weight::MEDIUM))
        {
            bold = true;
        }
        else if(weight < static_cast<int>(GFontStyle::Weight::NORMAL))
        {
            light = true;

        }
    }

    const char *closest = this->mFontNormal;
    if (bold && italic)
    {
        if (this->mFontBoldItalic != nullptr)
        {
            closest = this->mFontBoldItalic;
        }
        else if (this->mFontBold != nullptr)
        {
            closest = this->mFontBold;
        }
        else if (this->mFontItalic != nullptr)
        {
            closest = this->mFontItalic;
        }
    }
    else if (bold)
    {
        if (this->mFontBold != nullptr)
        {
            closest = this->mFontBold;
        }
    }
    else if (italic)
    {
        if (this->mFontItalic != nullptr)
        {
            closest = this->mFontItalic;
        }
    }
    else if (light)
    {
        if (this->mFontLight != nullptr)
        {
            closest = this->mFontLight;
        }
    }
    return closest;
}

void GFontFamily::matchFontFamily(std::list<const char *> &fontFamily)
{
    const char *italic = "italic";
    const char *bold = "bold";
    const char *light = "light";
    for (auto it = fontFamily.begin(); it != fontFamily.end(); ++it)
    {
        int length = strlen(*it);
        char *fontFileLowerCase = new char[length + 1];
        strcpy(fontFileLowerCase, *it);
        for (int i = 0; i < length; ++i)
        {
            fontFileLowerCase[i] = tolower(fontFileLowerCase[i]);
        }

        if (strstr(fontFileLowerCase, bold) != nullptr)
        {
            if (strstr(fontFileLowerCase, italic) != nullptr)
            {
                this->mFontBoldItalic = *it;
            }
            else
            {
                this->mFontBold = *it;
            }
        }
        else if (strstr(fontFileLowerCase, italic))
        {
            this->mFontItalic = *it;
        }
        else if (strstr(fontFileLowerCase, light))
        {
            this->mFontLight = *it;
        }
        else
        {
            this->mFontNormal = *it;
        }

        delete[] fontFileLowerCase;
    }
}

char *GFontFamily::GetProperFontFile()
{
    if (this->mFontNormal != nullptr)
    {
        return (char *)this->mFontNormal;
    }
    if (this->mFontLight != nullptr)
    {
        return (char *)this->mFontLight;
    }
    if (this->mFontBold != nullptr)
    {
        return (char *)this->mFontBold;
    }
    if (this->mFontBoldItalic != nullptr)
    {
        return (char *)this->mFontBoldItalic;
    }
    if (this->mFontItalic != nullptr)
    {
        return (char *)this->mFontItalic;
    }

    return (char *)this->mFontNormal;
}
#else

#endif
}
