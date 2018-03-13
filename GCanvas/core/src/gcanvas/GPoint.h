/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GPoint__
#define __GCanvas__GPoint__

#include <cmath>
#include <iostream>
#ifndef _WIN32

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#else
#include <GLES2/gl2.h>
#endif

typedef struct
{
    GLfloat x;
    GLfloat y;
} GPoint;

typedef union {
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

    v.x = (float)(v.x / ln);
    v.y = (float)(v.y / ln);
    return v;
}

#endif
