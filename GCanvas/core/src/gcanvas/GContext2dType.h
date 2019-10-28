/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GContext2dType_h
#define GContext2dType_h

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

typedef enum
{
    LINE_CAP_BUTT = 0,
    LINE_CAP_ROUND,
    LINE_CAP_SQUARE
} GLineCap;

typedef enum
{
    LINE_JOIN_MITER = 0,
    LINE_JOIN_BEVEL,
    LINE_JOIN_ROUND
} GLineJoin;

typedef enum
{
    DIRECTION_INHERIT,
    DIRECTION_LEFT_TO_RIGHT,
    DIRECTION_RIGHT_TO_LEFT
} GDirection;

typedef enum
{
    COMPOSITE_OP_SOURCE_OVER = 0,
    COMPOSITE_OP_LIGHTER = 1,
    COMPOSITE_OP_DARKER = 2,
    COMPOSITE_OP_DESTINATION_OUT = 3,
    COMPOSITE_OP_DESTINATION_OVER = 4,
    COMPOSITE_OP_SOURCE_ATOP = 5,
    COMPOSITE_OP_XOR = 6,
    COMPOSITE_OP_REPLACE = 7,
    COMPOSITE_OP_ALPHA = 8,
    COMPOSITE_OP_ADD = 9,
    COMPOSITE_OP_SOURCE_OUT = 10,
    COMPOSITE_OP_DESTINATION_IN = 11,
    COMPOSITE_OP_NONE,
} GCompositeOperation;

typedef struct {
    GLenum source;
    GLenum destination;
} GBlendOperationFuncs;

extern GBlendOperationFuncs GCompositeOperationFuncs(int index);

//static const struct
//{
//    GLenum source;
//    GLenum destination;
//} GCompositeOperationFuncs[] = {
//        {GL_ONE,           GL_ONE_MINUS_SRC_ALPHA}, //0
//        {GL_SRC_ALPHA,           GL_ONE}, //1
//        {GL_DST_COLOR,           GL_ONE_MINUS_SRC_ALPHA}, //2
//        {GL_ZERO,                GL_ONE_MINUS_SRC_ALPHA}, //3
//        {GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA}, //4
//        {GL_DST_ALPHA,           GL_ZERO}, //5
//        {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}, //6
//        {GL_ONE,                 GL_ZERO}, //7
//        {GL_SRC_ALPHA,           GL_DST_ALPHA}, //8
//        {GL_ONE,                 GL_ONE}, //9
//        {GL_ONE_MINUS_DST_ALPHA, GL_ZERO},//10
//        {GL_ZERO,                GL_SRC_ALPHA},//11
//};

typedef enum
{
    FILL_RULE_NONZERO,
    FILL_RULE_EVENODD
} GFillRule;

typedef enum
{
    FILL_TARGET_COLOR,
    FILL_TARGET_DEPTH
} GFillTarget;

#endif /* GContext2dType_h */
