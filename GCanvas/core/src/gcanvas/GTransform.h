/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GTRANSFORM_H
#define GCANVAS_GTRANSFORM_H

#include "GPoint.h"

#include <math.h>

// -----------------------------------------------------------
// --    Transform utility class
// --    Used by Canvas.setTransform
// --    Populates the vertexArr of a Quad
// -----------------------------------------------------------

struct GTransform
{
    float a, b, c, d, tx, ty;

    GTransform() : a(1), b(0), c(0), d(1), tx(0), ty(0) {}
    GTransform(float v1, float v2, float v3, float v4, float v5, float v6)
        : a(v1), b(v2), c(v3), d(v4), tx(v5), ty(v6)
    {
    }
};

/* The identity transform: [ 1 0 0 1 0 0 ]. */
//const GTransform GTransformIdentity(1, 0, 0, 1, 0, 0);

#define  GTransformIdentity  GTransform(1, 0, 0, 1, 0, 0)

/* Return the transform [ a b c d tx ty ]. */
static inline GTransform GTransformMake(float a, float b, float c, float d,
                                        float tx, float ty)
{
    GTransform trans(a, b, c, d, tx, ty);
    return trans;
}

/* Return a transform which translates by `(tx, ty)':
 t' = [ 1 0 0 1 tx ty ] */
static inline GTransform GTransformMakeTranslation(float tx, float ty)
{
    GTransform trans(1, 0, 0, 1, tx, ty);
    return trans;
}

/* Return a transform which scales by `(sx, sy)':
 t' = [ sx 0 0 sy 0 0 ] */
static inline GTransform GTransformMakeScale(float sx, float sy)
{
    return GTransform(sx, 0, 0, sy, 0, 0);
}

static inline float GTransformGetScale(const GTransform& t)
{
    return sqrt(t.a*t.a+t.d*t.d);
}


/* Return a transform which rotates by `angle' radians:
 t' = [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] */
static inline GTransform GTransformMakeRotation(float angle)
{
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    GTransform trans;
    trans.a = (float)cosAngle;
    trans.b = (float)-sinAngle;
    trans.c = (float)sinAngle;
    trans.d = (float)cosAngle;
    trans.tx = trans.ty = (float)0.0;
    return trans;
}

/* Return true if `t' is the identity transform, false otherwise. */
static inline bool GTransformIsIdentity(GTransform t)
{
    return (t.a == 1 && t.b == 0 && t.c == 0 && t.d == 1 && t.tx == 0 &&
            t.ty == 0);
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
static inline bool GTransformEqualToTransform(GTransform t1, GTransform t2)
{
    return t1.a == t2.a && t1.b == t2.b && t1.c == t2.c && t1.d == t2.d &&
           t1.tx == t2.tx && t1.ty == t2.ty;
}

/* Translate `t' by `(tx, ty)' and return the result:
 t' = [ 1 0 0 1 tx ty ] * t */
static inline GTransform GTransformTranslate(GTransform t, float tx,
                                             float ty)
{
    t.tx += tx;
    t.ty += ty;
    return t;
}

/* Scale `t' by `(sx, sy)' and return the result:
 t' = [ sx 0 0 sy 0 0 ] * t */

static inline GTransform GTransformScale(GTransform& t, float sx, float sy)
{
    t.a = sx * t.a;
    t.b = sy * t.b;
    t.c = sx * t.c;
    t.d = sy * t.d;
    t.tx = sx * t.tx;
    t.ty = sy * t.ty;
    return t;
}

/* Rotate `t' by `angle' radians and return the result:
 t' =  [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] * t */

static inline GTransform GTransformRotate(const GTransform& t, float angle)
{
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    GTransform result;
    result.a = (float)(cosAngle * t.a - sinAngle * t.b);
    result.b = (float)(sinAngle * t.a + cosAngle * t.b);
    result.c = (float)(cosAngle * t.c - sinAngle * t.d);
    result.d = (float)(sinAngle * t.c + cosAngle * t.d);
    result.tx = (float)(cosAngle * t.tx - sinAngle * t.ty + t.tx);
    result.ty = (float)(sinAngle * t.tx + cosAngle * t.ty + t.ty);
    return result;
}

/* Concatenate `t2' to `t1' and return the result:
 t' = t1 * t2 */
static inline GTransform GTransformConcat(const GTransform& t1, const GTransform& t2)
{
    GTransform result;
    result.a = t1.a * t2.a + t1.b * t2.c;
    result.b = t1.a * t2.b + t1.b * t2.d;
    result.c = t1.c * t2.a + t1.d * t2.c;
    result.d = t1.c * t2.b + t1.d * t2.d;
    result.tx = t1.a * t2.tx + t1.b * t2.ty + t1.tx;
    result.ty = t1.c * t2.tx + t1.d * t2.ty + t1.ty;
    return result;
}

/* Transform `point' by `t' and return the result:
 p' = p * t
 where p = [ x y 1 ]. */
static inline GPoint GPointApplyGTransform(float x, float y, GTransform t)
{
    GPoint p;
    p.x = (float)((double)t.a * x + (double)t.c * y + t.tx);
    p.y = (float)((double)-t.b * x + (double)t.d * y + t.ty);
    return p;
}

#endif /* GCANVAS_GTRANSFORM_H */
