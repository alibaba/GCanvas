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
//#include <gcanvas/GContext2dType.h>
//#include <gcanvas/GPoint.h>
#include <string>

#endif

#include "../gcanvas/GPoint.h"
#include "../gcanvas/GContext2dType.h"


namespace gcanvas {

    // flip the pixels by y axis
    void FlipPixel(unsigned char *pixels, int w, int h);

    // read pixels form texture
    void ReadPixelsFromTexture(unsigned char *pixels, int width, int height, int textureId);


    bool IsSamePoint(GPoint& p1, GPoint& p2, float min);


    bool IsNanPoint(const GPoint& p);


    bool HasSamePoint(GPoint& p1, GPoint& p2, GPoint& p3, float minValue);


    bool IsTrianglePointsValid(GPoint& p1, GPoint& p2, GPoint& p3, float minValue);


    float CalcPointAngle(const GPoint &director, const GPoint &center);


    float PointDistance(GPoint& s, GPoint& t);


//    inline float Determinate(float a, float b, float c, float d) {
//        return a*d - b*c;
//    }
//
//
//    bool LineLineIntersect(GPoint& output,
//                           float x1, float y1, //Line 1 start
//                           float x2, float y2, //Line 1 end
//                           float x3, float y3, //Line 2 start
//                           float x4, float y4 //Line 2 end
//    );


#ifdef ANDROID

    API_EXPORT GCompositeOperation StringToGlobalCompositeOp(const char *str);


    API_EXPORT std::string GlobalCompositeOpToString(GCompositeOperation &op);

    void waitUtilTimeout(sem_t *sem, uint ms);

#endif

}

#endif /* GCANVAS_UTIL_H */
