/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "CharacterSet.h"

Utf8ToUCS2::Utf8ToUCS2(const Utf8ToUCS2 &utf8)
    : utf8(0), utf8len(0), ucs2(0), ucs2len(0)
{
}

Utf8ToUCS2::Utf8ToUCS2(const char *utf8In, int utf8lenIn)
    : utf8(utf8In), utf8len(utf8lenIn), ucs2(0), ucs2len(0)
{
    getUcs2();
}

Utf8ToUCS2::~Utf8ToUCS2()
{
    if (ucs2) delete[] ucs2;
}

void Utf8ToUCS2::getUcs2()
{
    ucs2 = new unsigned short[utf8len + 1];

    char *ptr = (char *)utf8, *last = (char *)utf8 + utf8len;
    int offset = 0;
    unsigned ch;

    for (; ptr < last; offset++)
    {
        if ((*ptr & 0x80) == 0)
        {
            ucs2[offset] = (*ptr);
            ++ptr;
        }
        else if ((*ptr & 0x20) == 0)
        {
            ch = *ptr & 0x1f;
            ++ptr;
            ch <<= 6;
            ch += *ptr & 0x3f;
            ucs2[offset] = ch;
            ++ptr;
        }
        else if ((*ptr & 0x10) == 0)
        {
            ch = *ptr & 0x0f;
            ++ptr;
            ch <<= 6;
            ch += *ptr & 0x3f;
            ++ptr;
            ch <<= 6;
            ch += *ptr & 0x3f;
            ucs2[offset] = ch;
            ++ptr;
        }
        else
        {
            // symbol number is > 0xffff
            ucs2[offset] = 'X';
            ptr += 4;
        }
    }
    ucs2len = offset;
}
