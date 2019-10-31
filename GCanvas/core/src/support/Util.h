/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_UTIL_H
#define GCANVAS_UTIL_H

#ifdef ANDROID

#include <sys/types.h>
#include <semaphore.h>

#endif

namespace gcanvas
{
// flip the pixels by y axis
void FlipPixel(unsigned char *pixels, int w, int h);
    
#ifdef ANDROID

void waitUtilTimeout(sem_t *sem,uint ms);
#endif

}

#endif /* GCANVAS_UTIL_H */
