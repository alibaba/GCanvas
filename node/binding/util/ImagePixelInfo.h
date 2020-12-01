/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef IMAGE_PIXEL_INFO_H
#define IMAGE_PIXEL_INFO_H
#include <vector>
class ImagePixelInfo
{
public:
    std::vector<unsigned char> &getPixels(){
        return this->pixels;
    }
    unsigned int width = 0;
    unsigned int height = 0;

    size_t size(){
        return  this->pixels.size();
    }

    char *buffer;
    size_t bufferSize;

private:
    std::vector<unsigned char> pixels;

};
#endif