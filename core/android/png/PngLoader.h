/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef PNGLOADER_H_
#define PNGLOADER_H_

#include "IPngCoder.h"
#include <stdlib.h>
#include <vector>

#ifndef ENABLE_LODE_PNG
#define ENABLE_LODE_PNG 1
#endif

namespace gcanvas
{
class PngLoader
{
public:
    PngLoader();
    ~PngLoader();

    static PngLoader &Instance();

    int Init();
    int UnInit();

    int DecodePng(const char *filePath, unsigned char **outputBuffer,
                  unsigned int *w, unsigned int *h);
    int DecodePng(const unsigned char *inBuffer, unsigned int bufferLen,
                  unsigned char **outputBuffer, unsigned int *w,
                  unsigned int *h);

    int EncodePng(const char *filename, const unsigned char *image, unsigned w,
                  unsigned h);
	int EncodePng(unsigned char** out, int* outsize, const unsigned char* image, unsigned w, unsigned h);

    void FreeBuffer(unsigned char **buffer);

private:
    typedef std::vector< IPngCoder * > PngCoderVector;

    PngCoderVector mCoders;
};
}

#endif /* PNGLOADER_H_ */
