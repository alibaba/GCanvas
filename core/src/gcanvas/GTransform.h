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
#include <float.h>


#define G_FLOAT_EQUAL(a,b)    fabs((a)-(b)) < FLT_EPSILON

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

/*
 * Return the transform [ a b c d tx ty ]
 * [a   c   tx]
 * [b   d   ty]
 * [0   0    1]
 * */
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

static inline float GTransformGetScaleX(const GTransform& t)
{
    return sqrt(t.a*t.a+t.c*t.c);
}

static inline float GTransformGetScaleY(const GTransform& t)
{
    return sqrt(t.b*t.b+t.d*t.d);
}

static inline float GTransformGetScale(const GTransform& t)
{
    float sx = GTransformGetScaleX(t);
    float sy = GTransformGetScaleY(t);
    return std::max<float>(sx, sy);
}

//for font scale, min value is 1, max value is 72
static inline float GTransformGetMaxScale(const GTransform& t)
{
    float sx = GTransformGetScaleX(t);
    float sy = GTransformGetScaleY(t);
    return std::min<float>(72,std::max<float>(1, std::max<float>(sx, sy)) );
}


/* Return a transform which rotates by `angle' radians:
 t' = [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] */
static inline GTransform GTransformMakeRotation(float angle)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    GTransform trans;
    trans.a = cosAngle;
    trans.b = sinAngle;
    trans.c = -sinAngle;
    trans.d = cosAngle;
    trans.tx = trans.ty = 0;
    return trans;
}

/* Return true if `t' is the identity transform, false otherwise. */
static inline bool GTransformIsIdentity(const GTransform& t)
{
    return (t.a == 1 && t.b == 0 && t.c == 0 && t.d == 1 && t.tx == 0 &&
            t.ty == 0);
}


static inline bool GTransformIsSimilarity(const GTransform& t)
{
    // translate or equal scale (no rotation or skew)
    return (t.a == t.d && t.b == 0 && t.c == 0);
}


/* Return true if `t1' and `t2' are equal, false otherwise. */
static inline bool GTransformEqualToTransform(const GTransform& t1, const GTransform& t2)
{
    return G_FLOAT_EQUAL(t1.a, t2.a) && G_FLOAT_EQUAL(t1.b, t2.b) &&
          G_FLOAT_EQUAL(t1.c, t2.c) && G_FLOAT_EQUAL(t1.d, t2.d) &&
          G_FLOAT_EQUAL(t1.tx, t2.tx) && G_FLOAT_EQUAL(t1.ty, t2.ty);
}

/* Translate `t' by `(tx, ty)' and return the result:
 t' = [ 1 0 0 1 tx ty ] * t */
static inline GTransform GTransformTranslate(const GTransform & t, float tx, float ty)
{
    GTransform result;
    result.a = t.a;
    result.b = t.b;
    result.c = t.c;
    result.d = t.d;
    result.tx = t.a * tx + t.c * ty + t.tx;
    result.ty = t.b * tx + t.d * ty + t.ty;
    
    return result;
}

/* Scale `t' by `(sx, sy)' and return the result:
 t' = [ sx 0 0 sy 0 0 ] * t */

static inline GTransform GTransformScale(const GTransform& t, float sx, float sy)
{
    GTransform result = t;
    result.a *= sx;
    result.b *= sx;
    result.c *= sy;
    result.d *= sy;
    
    return result;
}

/* Rotate `t' by `angle' radians and return the result:
 t' =  [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] * t */

static inline GTransform GTransformRotate(const GTransform& t, float angle)
{
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    GTransform result = t;
    result.a = t.a*cosAngle    + t.c*sinAngle;
    result.b = t.b*cosAngle    + t.d*sinAngle;
    result.c = t.a*(-sinAngle) + t.c*cosAngle;
    result.d = t.b*(-sinAngle) + t.d*cosAngle;
    
    return result;
}

/* Concatenate `t2' to `t1' and return the result:
 t' = t1 * t2 */
static inline GTransform GTransformConcat(const GTransform& t2, const GTransform& t1)
{
    GTransform result;
    result.a = t1.a * t2.a + t1.b * t2.c;
    result.b = t1.a * t2.b + t1.b * t2.d;
    result.c = t1.c * t2.a + t1.d * t2.c;
    result.d = t1.c * t2.b + t1.d * t2.d;
    result.tx = t1.tx * t2.a + t1.ty * t2.c + t2.tx;
    result.ty = t1.tx * t2.b + t1.ty * t2.d + t2.ty;
    return result;
}

/* Transform `point' by `t' and return the result:
 p' = p * t
 where p = [ x y 1 ]. */
static inline GPoint GPointApplyGTransform(const GPoint& point, const GTransform& t)
{
    if(GTransformIsIdentity(t)) {
        return point;
    }
    GPoint p;
    p.x = (t.a * point.x + t.c * point.y + t.tx);
    p.y = (t.b * point.x + t.d * point.y + t.ty);
    return p;
}

static inline void GPointApplyGTransformInPlace(GPoint& point, const GTransform& t)
{
    float oldX = point.x;
    float oldY = point.y;
    point.x = (t.a * oldX + t.c * oldY + t.tx);
    point.y = (t.b * oldX + t.d * oldY + t.ty);
}


/**
 * copy transform data to float array
 */
static inline void GTransformCopy(GTransform& t, float out[])
{
    out[0] = t.a;
    out[1] = t.b;
    out[2] = t.c;
    out[3] = t.d;
    out[4] = t.tx;
    out[5] = t.ty;
}

static inline GTransform GTransformInvert(const GTransform& t)
{
    if( GTransformIsIdentity(t) ){
        return t;
    }
    double k = t.a * t.d - t.b * t.c;
    
    if( fabs(k) < FLT_EPSILON ){
        return t;
    }
    
    GTransform result;
    result.a = (float) ( t.d / k );
    result.b = (float) ( -t.b / k );
    result.c = (float) ( -t.c / k );
    result.d = (float) ( t.a / k );
    result.tx = (float) (t.c * t.ty - t.d * t.tx) / k;
    result.ty = (float) (t.b * t.tx - t.a * t.ty) / k;
    return result;
}

#endif /* GCANVAS_GTRANSFORM_H */
