/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GTreemap.h"


GTreemap::GTreemap(int w, int h) : mWidth(w), mHeight(h), mLineLast(w), mVerticalLast(h),
                                   mCurrentLineHeight(0) {

}

bool GTreemap::Add(const GSize &size, GRect &rect) {
    rect.SetSize(size);
    if (size.width > mLineLast) {
        if (size.width > mWidth) {
            return false;
        } else {

            mVerticalLast -= mCurrentLineHeight;
            if (mVerticalLast < size.height) {
                mVerticalLast += mCurrentLineHeight;

                return false;
            } else {
                rect.SetPosition(0, mHeight - mVerticalLast);
                mCurrentLineHeight = size.height;
                mLineLast = mWidth - size.width;

            }

        }
    } else {
        rect.SetPosition(mWidth - mLineLast, mHeight - mVerticalLast);

        if (mCurrentLineHeight < size.height) {
            if (mVerticalLast < size.height) {
                return false;
            } else {

                mCurrentLineHeight = size.height;
                mLineLast -= size.width;
            }
        } else {

            mLineLast -= size.width;
        }
    }
    return true;
}

void GTreemap::Clear() {
    mLineLast = mWidth;
    mVerticalLast = mHeight;
    mCurrentLineHeight = 0;
}