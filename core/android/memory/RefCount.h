/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef REFCOUNT_H_
#define REFCOUNT_H_

namespace gcanvas
{
/// <description>
/// RefCount
/// </description>
class RefCount
{
public:
    /// <description>
    /// TODO
    /// </description>
    RefCount() : mRefCount(0) {}

    /// <description>
    /// TODO
    /// </description>
    void AddRef() { ++mRefCount; }

    /// <description>
    /// TODO
    /// </description>
    /// <remarks>
    /// TODO
    /// </remarks>
    void ReleaseRef()
    {
        if (--mRefCount <= 0) delete this;
    }

protected:
    /// <description>
    /// TODO
    /// </description>
    /// <remarks>
    /// TODO
    /// </remarks>
    virtual ~RefCount() {}

protected:
    unsigned int mRefCount;
};
}

#endif
