/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__Util__
#define __GCanvas__Util__

#ifdef ANDROID

#include <sys/types.h>
#include <semaphore.h>

#endif

namespace gcanvas
{
// flip the pixels by y axis
void FlipPixel(unsigned char *pixels, int w, int h);

// get a part of pixel from rgba datas
void GetSegmentPixel(const unsigned char *srcPx, unsigned int sw,
                     unsigned int x, unsigned int y, unsigned int dw,
                     unsigned int dh, unsigned char *destPx);

bool IsSupportNeon();
#ifdef ANDROID
void waitUtilTimeout(sem_t *sem,uint ms);
#endif

}

#endif
