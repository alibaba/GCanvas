/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Encode.h"

namespace gcanvas
{
static const char BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";
static const unsigned char BASE64_INDEXES[] = {
    62,  255, 255, 255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
    255, 255, 255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    255, 255, 255, 255, 255, 255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

static const char BASE64_PAD = '=';
static const char BASE64_FIRST = '+';

#define CSHL(v, n) ((unsigned char)((v) << (n)))
#define CSHR(v, n) ((unsigned char)((v) >> (n)))
#define CSHLR(v, l, r) CSHR(CSHL(v, l), r)
#define CSHRL(v, r, l) CSHL(CSHR(v, r), l)
#define DECODE(v) (BASE64_INDEXES[(v)-BASE64_FIRST])

int Base64EncodeLen(int inputLen) { return (inputLen + 2) / 3 * 4; }

int Base64EncodeBuf(char *szDest, const char *szSrc, int &iBufSize)
{
    unsigned char *output = (unsigned char *)szDest;
    const unsigned char *input = (const unsigned char *)szSrc;

    int ret = 0;
    for (int i = 0; i < iBufSize; i += 3)
    {
        output[ret++] = BASE64_CHARS[CSHR(input[i], 2)];

        if (i + 1 >= iBufSize)
        {
            output[ret] = BASE64_CHARS[CSHLR(input[i], 6, 2)];
            output[++ret] = BASE64_PAD;
            output[++ret] = BASE64_PAD;
            ++ret;
            break;
        }

        output[ret] =
            BASE64_CHARS[CSHLR(input[i], 6, 2) | CSHR(input[i + 1], 4)];
        if (i + 2 >= iBufSize)
        {
            output[++ret] = BASE64_CHARS[CSHLR(input[i + 1], 4, 2)];
            output[++ret] = BASE64_PAD;
            ++ret;
            break;
        }
        output[++ret] =
            BASE64_CHARS[CSHLR(input[i + 1], 4, 2) | CSHR(input[i + 2], 6)];
        output[++ret] = BASE64_CHARS[CSHLR(input[i + 2], 2, 2)];
        ++ret;
    }
    iBufSize = ret;
    return iBufSize;
}

int Base64DecodeBuf(char *szDest, const char *szSrc, int &iBufSize)
{
    unsigned char *output = (unsigned char *)szDest;
    const unsigned char *input = (const unsigned char *)szSrc;

    if (iBufSize % 4 > 0)
    {
        return -1;
    }

    int ret = 0;
    for (int i = 0; i < iBufSize; i += 4)
    {
        unsigned char buf[4] = {0};
        for (int j = 0; j < 4; ++j)
        {
            if (input[i + j] == BASE64_PAD)
            {
                break;
            }
            buf[j] = DECODE(input[i + j]);
        }

        buf[0] <<= 2;
        output[ret++] = buf[0] | CSHR(buf[1], 4);

        if (BASE64_PAD == input[i + 2])
        {
            break;
        }

        buf[1] <<= 4;
        output[ret++] = buf[1] | CSHR(buf[2], 2);

        if (BASE64_PAD == input[i + 3])
        {
            break;
        }

        buf[2] <<= 6;
        output[ret++] = buf[2] | buf[3];
    }
    iBufSize = ret;
    return iBufSize;
}
}
