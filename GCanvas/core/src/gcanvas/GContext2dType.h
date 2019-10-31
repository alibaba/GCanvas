/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GCONTEXT2DTYPE_H
#define GCANVAS_GCONTEXT2DTYPE_H

#include "GGL.h"

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

#endif /* GCANVAS_GCONTEXT2DTYPE_H */
