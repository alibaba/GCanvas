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

namespace gcanvas
{
class GFontFamily
{
public:
    GFontFamily(std::list< const char * > &fontFamily);
    virtual ~GFontFamily() {}

    const char *MatchFamilyStyle(GFontStyle &fontStyle);
    char *GetProperFontFile();

private:
    void matchFontFamily(std::list<const char *> &fontFamily);

    const char *mFontNormal;
    const char *mFontBold;
    const char *mFontItalic;
    const char *mFontBoldItalic;
};
}

#endif
