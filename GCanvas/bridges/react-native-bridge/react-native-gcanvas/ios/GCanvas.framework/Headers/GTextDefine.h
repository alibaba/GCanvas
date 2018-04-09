/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GTextDefine_h
#define GTextDefine_h

typedef enum {
    TEXT_BASELINE_ALPHABETIC = 0, // for Western fonts: Default. The text
                                   // baseline is the normal alphabetic baseline
    TEXT_BASELINE_MIDDLE,  // The text baseline is the middle of the em square
    TEXT_BASELINE_TOP,     // The text baseline is the top of the em square
    TEXT_BASELINE_HANGING, // for Indian fonts : The text baseline is the
                            // hanging baseline
    TEXT_BASELINE_BOTTOM,  // The text baseline is the bottom of the bounding
                            // box
    TEXT_BASELINE_IDEOGRAPHIC // for CJK fonts: The text baseline is the
                               // ideographic baseline
} GTextBaseline;

typedef enum {
    TEXT_ALIGN_START =
        0, // Default. The text baseline is the normal alphabetic baseline
    TEXT_ALIGN_END,    // The text ends at the specified position
    TEXT_ALIGN_LEFT,   // The text starts at the specified position
    TEXT_ALIGN_CENTER, // The center of the text is placed at the specified
                        // position
    TEXT_ALIGN_RIGHT   // The text ends at the specified position
} GTextAlign;

#endif /* GTextDefine_h */
