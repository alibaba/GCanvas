/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GTREEMAP_H
#define GCANVAS_GTREEMAP_H

#include <new>

struct GSize
{
    GSize(int w, int h) : width(w), height(h)
    {

    }

    int width;
    int height;
};

struct GRect
{
    GRect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), width(w), height(h)
    {

    }

    GRect(int x, int y, const GSize &size) : x(x), y(y), width(size.width), height(size.height)
    {

    }

    void Set(int x, int y, const GSize &size)
    {
        new (this) GRect(x, y, size);
    }

    void SetPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void SetSize(const GSize &size)
    {
        width = size.width;
        height = size.height;
    }

    int x;
    int y;
    int width;
    int height;
};


class GTreemap
{
public:
    GTreemap(int w, int h);

    ~GTreemap()
    {};

    int GetWidth()
    { return mWidth; };

    int GetHeight()
    { return mHeight; };

    bool Add(const GSize &size, GRect &rect);

    void Clear();

private:
    unsigned int mWidth;
    unsigned int mHeight;
    int mLineLast;
    int mVerticalLast;
    int mCurrentLineHeight;
};

#endif /* GCANVAS_GTREEMAP_H */
