/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Util.h"
#include "Log.h"
#include <string.h>

#ifdef ANDROID
#include <linux/time.h>
#include <sys/time.h>
#include <semaphore.h>
#endif

namespace gcanvas
{
// flip the pixels by y axis
void FlipPixel(unsigned char *pixels, int w, int h)
{
    // taken from example here:
    // http://www.codesampler.com/2010/11/02/introduction-to-opengl-es-2-0/
    // Flip and invert the PNG image since OpenGL likes to load everything
    // backwards from what is considered normal!
    int halfTheHeightInPixels = h / 2;
    int heightInPixels = h;

    // Assuming RGBA for 4 components per pixel.
    int numColorComponents = 4;

    // Assuming each color component is an unsigned char.
    int widthInChars = w * numColorComponents;

    unsigned char *top = nullptr;
    unsigned char *bottom = nullptr;
    unsigned char temp = 0;

    for (int h = 0; h < halfTheHeightInPixels; ++h)
    {
        top = pixels + h * widthInChars;
        bottom = pixels + (heightInPixels - h - 1) * widthInChars;

        for (int w = 0; w < widthInChars; ++w)
        {
            // Swap the chars around.
            temp = *top;
            *top = *bottom;
            *bottom = temp;

            ++top;
            ++bottom;
        }
    }
}

// get a part of pixel from rgba datas
void GetSegmentPixel(const unsigned char *srcPx, unsigned int sw,
                     unsigned int x, unsigned int y, unsigned int dw,
                     unsigned int dh, unsigned char *destPx)
{
    srcPx += (sw * y + x) * 4;
    for (unsigned int i = 0; i < dh; ++i)
    {
        memcpy(destPx, srcPx, dw * 4);

        srcPx += sw * 4;
        destPx += dw * 4;
    }
}

bool IsSupportNeon() { return false; }

#ifdef ANDROID
void timeraddMS(struct timeval *a, uint ms)
{
    a->tv_usec += ms * 1000;
    if(a->tv_usec >= 1000000)
    {
        a->tv_sec += a->tv_usec / 1000000;
        a->tv_usec %= 1000000;
    }
}

void waitUtilTimeout(sem_t *sem,uint ms){
    int ret;
    struct timeval now;
    struct timespec outtime;

    gettimeofday(&now, NULL);
//    LOG_D("start to wait,sec=%d,usec=%d\n",now.tv_sec,now.tv_usec);
    timeraddMS(&now, ms);
    outtime.tv_sec = now.tv_sec;
    outtime.tv_nsec = now.tv_usec * 1000;
    ret = sem_timedwait(sem,&outtime);
    if (ret == -1)
    {
        gettimeofday(&now, NULL);
        LOG_D("wait time out,sec=%d,usec=%d\n",now.tv_sec,now.tv_usec);
    }
    else
    {
        gettimeofday(&now, NULL);
//        LOG_D("success wait response,sec=%d,usec=%d\n",now.tv_sec,now.tv_usec);
    }

}

#endif

}