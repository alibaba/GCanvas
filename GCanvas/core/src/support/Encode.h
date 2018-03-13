/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef _GCANVAS_ENCODE_H_
#define _GCANVAS_ENCODE_H_

namespace gcanvas
{
////////////////////////////base64 encoding////////////////////////////
int Base64EncodeLen(int inputLen);
int Base64EncodeBuf(char *szDest, const char *szSrc, int &iBufSize);
int Base64DecodeBuf(char *szDest, const char *szSrc, int &iBufSize);
}

#endif
