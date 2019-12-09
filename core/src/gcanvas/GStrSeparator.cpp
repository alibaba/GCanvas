/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GStrSeparator.h"

#include <ctype.h>
#include <string.h>

namespace gcanvas
{
GStrSeparator::GStrSeparator() { memset(mPointers, 0, sizeof(mPointers)); }

short GStrSeparator::SepStrByCharArray(char *str, const char *byteArray,
                                        short byteCount, short maxCount)
{
    if (maxCount < 0)
    {
        maxCount = PARSESEPSTRMAXCOUNT;
    }
    short sepCount = 0, i = 0;
    bool lastMark = true;
    char *cur = str;

    for (; *cur; cur++)
    {
        for (i = 0; i < byteCount; i++)
        {
            if (*cur == byteArray[i])
            {
                lastMark = true;
                *cur = 0;
                break;
            }
        }
        if (*cur && lastMark)
        {
            if (sepCount < maxCount)
            {
                mPointers[sepCount++] = cur;
            }
            lastMark = false;
        }
    }
    return sepCount;
};

short GStrSeparator::SepStrBySpace(char *str, short maxCount)
{
    if (maxCount < 0)
    {
        maxCount = PARSESEPSTRMAXCOUNT;
    }

    short sepCount = 0;
    bool lastMark = true;
    char *cur = str;

    for (; *cur; cur++)
    {
        if (isspace((unsigned char)*cur))
        {
            lastMark = true;
            *cur = 0;
        }
        else
        {
            if (lastMark)
            {
                if (sepCount < maxCount)
                {
                    mPointers[sepCount++] = cur;
                }
                lastMark = false;
            }
        }
    }
    return sepCount;
};
}
