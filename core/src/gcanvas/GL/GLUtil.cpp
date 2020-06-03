/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#include "GLUtil.h"

namespace gcanvas {


//////////////////////////////////////////////////////////////////////////////
///   Pixels Sample
//////////////////////////////////////////////////////////////////////////////
    struct RGBA {
        RGBA &operator+=(int c) {
            r += c & 0xff;
            g += c >> 8 & 0xff;
            b += c >> 16 & 0xff;
            a += c >> 24 & 0xff;

            return *this;
        }

        operator uint32_t() {
            uint32_t c = 0;
            c += r & 0xff;
            c += g << 8 & 0xff00;
            c += b << 16 & 0xff0000;
            c += a << 24 & 0xff000000;

            return c;
        }

        uint16_t r = 0;
        uint16_t g = 0;
        uint16_t b = 0;
        uint16_t a = 0;
    };

    inline RGBA operator/(const RGBA &a, int b) {
        RGBA c;
        c.r = a.r / b;
        c.g = a.g / b;
        c.b = a.b / b;
        c.a = a.a / b;
        return c;
    }

    inline int GetPixel(int *pixels, int x, int y, int width, int height) {
        if (x < 0) {
            x = 0;
        }
        if (x > width - 1) {
            x = width - 1;
        }
        if (y < 0) {
            y = 0;
        }
        if (y > height - 1) {
            y = height - 1;
        }
        return pixels[y * width + x];
    }

    void PixelsSampler(int inWidth, int inHeight, int *inPixels, int outWidth, int outHeight,
                       int *outPixels) {
        for (int y = 0; y < outHeight; ++y) {
            int inY = y * inHeight / outHeight;
            int revertY = inHeight - inY - 1;
            for (int x = 0; x < outWidth; ++x) {
                int inX = x * inWidth / outWidth + 1;
                RGBA pixel;
                pixel += GetPixel(inPixels, inX, revertY, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX - 1, revertY - 1, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX, revertY - 1, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX + 1, revertY - 1, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX - 1, revertY, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX + 1, revertY, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX - 1, revertY + 1, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX, revertY + 1, inWidth, inHeight);
                pixel += GetPixel(inPixels, inX - 1, revertY + 1, inWidth, inHeight);
                outPixels[y * outWidth + x] = pixel / 9;
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
///   Pixels BindTexture
//////////////////////////////////////////////////////////////////////////////
    GLuint PixelsBindTexture(const unsigned char *rgbaData, GLint format, unsigned int width,
                             unsigned int height, std::vector<GCanvasLog> *errVec) {
        if (nullptr == rgbaData)
            return (GLuint) - 1;
        
        
        GLint bindingTexture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &bindingTexture);
        
        GLenum glerror = 0;
        GLuint glID;
        glGenTextures(1, &glID);
        glerror = glGetError();
        if (glerror && errVec) {
            GCanvasLog log;
            FillLogInfo(log, "gen_texture_fail", "<function:%s, glGetError:%x>", __FUNCTION__,
                        glerror);
            errVec->push_back(log);
        }
        glBindTexture(GL_TEXTURE_2D, glID);
        glerror = glGetError();
        if (glerror && errVec) {
            GCanvasLog log;
            FillLogInfo(log, "bind_texture_fail", "<function:%s, glGetError:%x>", __FUNCTION__,
                        glerror);
            errVec->push_back(log);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, rgbaData);
        glerror = glGetError();
        if (glerror && errVec) {
            GCanvasLog log;
            FillLogInfo(log, "glTexImage2D_fail", "<function:%s, glGetError:%x>",
                        __FUNCTION__, glerror);
            errVec->push_back(log);
        }
        
        glBindTexture(GL_TEXTURE_2D, bindingTexture);
        
        return glID;
    }

}
