/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GPOINT_H
#define GCANVAS_GPOINT_H

#include "GGL.h"
#include "GContext2dType.h"
#include "export.h"

#include <cmath>
#include <iostream>


typedef struct
{
    GLfloat x;
    GLfloat y;
} GPoint;

typedef union
{
    struct
    {
        GLfloat r, g, b, a;
    } rgba;
    GLfloat components[4];
} GColorRGBA;



typedef struct
{
    GPoint pos;
    GPoint uv;
    GColorRGBA color;
} GVertex;

static inline GPoint PointMake(float x, float y)
{
    GPoint p = {x, y};
    return p;
}

static inline GPoint PointAdd(GPoint a, GPoint b)
{
    GPoint p = {a.x + b.x, a.y + b.y};
    return p;
}

static inline GPoint PointSub(GPoint a, GPoint b)
{
    GPoint p = {a.x - b.x, a.y - b.y};
    return p;
}

//static inline GPoint PointMultiply(GPoint a, GPoint b)
//{
//    GPoint p = {a.x * b.x, a.y * b.y};
//    return p;
//}


static inline GPoint PointNormalize(GPoint v)
{
    double ln = sqrtf(v.x * v.x + v.y * v.y);
    if (ln == 0)
    {
        return v;
    }

    v.x = (float) (v.x / ln);
    v.y = (float) (v.y / ln);
    return v;
}

struct GRectf
{
    GPoint leftTop = {0, 0};
    GPoint bottomRight = {0, 0};

    float Width() const
    {
        return bottomRight.x - leftTop.x;
    }

    float Height() const
    {
        return bottomRight.y - leftTop.y;
    }

    void Enlarge(float x, float y)
    {
        leftTop.x -= x;
        leftTop.y -= y;
        bottomRight.x += x;
        bottomRight.y += y;
    }
};

inline GRectf operator/(const GRectf &rect, float factor)
{
    GRectf result = rect;
    result.leftTop.x /= factor;
    result.leftTop.y /= factor;
    result.bottomRight.x /= factor;
    result.bottomRight.y /= factor;
    return result;
}



#endif /* GCANVAS_GPOINT_H */
