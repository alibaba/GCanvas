/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef IDELETE_H_
#define IDELETE_H_

namespace gcanvas
{
class IDelete
{
public:
    IDelete() {}
    virtual void Delete() = 0;

protected:
    virtual ~IDelete() {}
};
}

#endif
