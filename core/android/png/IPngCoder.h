/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef PNG_CODER_H_
#define PNG_CODER_H_

namespace gcanvas
{
class IPngCoder
{
public:
    virtual ~IPngCoder() {}

    virtual int Init() = 0;
    virtual int UnInit() = 0;

    virtual bool IsSupportEncode() = 0;
    virtual bool IsSupportDecode() = 0;

    virtual int Decode(const char *filePath, unsigned char **outputBuffer,
                       unsigned int *w, unsigned int *h) = 0;
    virtual int Decode(const unsigned char *inBuffer, unsigned int bufferLen,
                       unsigned char **outputBuffer, unsigned int *w,
                       unsigned int *h) = 0;
    virtual int Encode(const char *filename, const unsigned char *image,
                       unsigned w, unsigned h) = 0;
	virtual int Encode(unsigned char** out, int* outsize, const unsigned char* image, unsigned w, unsigned h) = 0;
    virtual void FreeBuffer(unsigned char *buffer) = 0;
};
}

#endif /* PNG_CODER_H_ */
