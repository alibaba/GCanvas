/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef LESSER_H_
#define LESSER_H_

#include <string.h>

namespace gcanvas
{
struct Lesser
{
public:
    bool operator()(const char *l, const char *r) const
    {
        return strcmp(l, r) < 0;
    }
};
}

#endif /* LESSER_H_ */
