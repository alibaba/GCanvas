/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas_GFontFamily__
#define __GCanvas_GFontFamily__

#include "GFontStyle.h"

#include <export.h>
#include <vector>

namespace gcanvas
{


    /**
     * font family内的单个字体
     */
    struct GFontFamilyItem {

        const char* font_file_name;

        GFontStyle::Style style;

        GFontStyle::Variant variant;

        GFontStyle::Weight  weight;


        GFontFamilyItem();

    };



class GFontFamily
{
public:
    GFontFamily(std::list< const char * > &fontFamily);
    virtual ~GFontFamily() {}

    API_EXPORT const char *MatchFamilyStyle(GFontStyle &fontStyle);
    char *GetProperFontFile();

private:
    void matchFontFamily(std::list<const char *> &fontFamily);

    void matchFontFamilyLegacy(std::list<const char *> &fontFamily);


    // @depreated
    const char *mFontNormal= nullptr;
    const char *mFontLight= nullptr;
    const char *mFontBold= nullptr;
    const char *mFontItalic= nullptr;
    const char *mFontBoldItalic= nullptr;

    std::vector<GFontFamilyItem> fontItems;

    };
}

#endif
