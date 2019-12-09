/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef __GCANVAS_CHARACTER_SET_H__
#define __GCANVAS_CHARACTER_SET_H__

#include <export.h>

struct Utf8ToUCS2
{
    const char *utf8;
    int utf8len;
    unsigned short *ucs2;
    int ucs2len;

    API_EXPORT Utf8ToUCS2(const char *utf8In, int utf8lenIn);
    virtual ~Utf8ToUCS2();

private:
    Utf8ToUCS2(const Utf8ToUCS2 &);
    void getUcs2();
};

#endif
