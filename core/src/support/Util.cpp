/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Util.h"
#include "Log.h"
#include "GGL.h"
#include <string.h>

#ifdef ANDROID

#include <linux/time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <gcanvas/GPoint.h>

#endif

namespace gcanvas {
    // flip the pixels by y axis
    void FlipPixel(unsigned char *pixels, int w, int h) {
        // taken from example here:
        // http://www.codesampler.com/2010/11/02/introduction-to-opengl-es-2-0/
        // Flip and invert the PNG image since OpenGL likes to load everything
        // backwards from what is considered normal!
        int halfTheHeightInPixels = h / 2;
        int heightInPixels = h;

        // Assuming RGBA for 4 components per pixel.
        int numColorComponents = 4;

        // Assuming each color component is an unsigned char.
        int widthInChars = w * numColorComponents;

        unsigned char *top = nullptr;
        unsigned char *bottom = nullptr;
        unsigned char temp = 0;

        for (int h = 0; h < halfTheHeightInPixels; ++h) {
            top = pixels + h * widthInChars;
            bottom = pixels + (heightInPixels - h - 1) * widthInChars;

            for (int w = 0; w < widthInChars; ++w) {
                // Swap the chars around.
                temp = *top;
                *top = *bottom;
                *bottom = temp;

                ++top;
                ++bottom;
            }
        }
    }

    void ReadPixelsFromTexture(unsigned char *pixels, int width, int height, int textureId)
    {
        if( !pixels || textureId <= 0 ) return;
        
        GLint mSaveFboFrame = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mSaveFboFrame);
        GLint mSaveTexture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &mSaveTexture);
        
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        
        glFlush();
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        glBindFramebuffer(GL_FRAMEBUFFER, mSaveFboFrame);
        glBindTexture(GL_TEXTURE_2D, mSaveTexture);
        
        glDeleteFramebuffers(1, &fbo);
    }


    bool IsSamePoint(GPoint& p1, GPoint& p2, float min) {
        return abs(p1.x - p2.x) < min && abs(p1.y - p2.y) < min;
    }


    bool IsNanPoint(const GPoint& p) {
        return std::isnan(p.x) || std::isnan(p.y);
    }


    bool HasSamePoint(GPoint& p1, GPoint& p2, GPoint& p3, float minValue) {
        return IsSamePoint(p1, p2, minValue) || IsSamePoint(p2, p3, minValue) || IsSamePoint(p1, p3, minValue);
    }


    float CalcPointAngle(const GPoint &director, const GPoint &center) {
        GPoint adjust = PointSub(director, center);
        return atan2(adjust.y, adjust.x);
    }


    bool IsTrianglePointsValid(GPoint& p1, GPoint& p2, GPoint& p3, float minValue) {
        if (IsNanPoint(p1) || IsNanPoint(p3) || IsNanPoint(p2)) {
            return false;
        }
        return !HasSamePoint(p1, p2, p3, minValue);
    }


    float PointDistance(GPoint& s, GPoint& t) {
        float deltaX = s.x - t.x;
        float deltaY = s.y - t.y;
        return sqrtf(deltaX * deltaX + deltaY * deltaY);
    }


//    bool LineLineIntersect(GPoint& output,
//                           float x1, float y1, //Line 1 start
//                           float x2, float y2, //Line 1 end
//                           float x3, float y3, //Line 2 start
//                           float x4, float y4) { //Line 2 end
//        //http://mathworld.wolfram.com/Line-LineIntersection.html
//        float detL1 = Determinate(x1, y1, x2, y2);
//        float detL2 = Determinate(x3, y3, x4, y4);
//        float x1mx2 = x1 - x2;
//        float x3mx4 = x3 - x4;
//        float y1my2 = y1 - y2;
//        float y3my4 = y3 - y4;
//
//        float xnom = Determinate(detL1, x1mx2, detL2, x3mx4);
//        float ynom = Determinate(detL1, y1my2, detL2, y3my4);
//        float denom = Determinate(x1mx2, y1my2, x3mx4, y3my4);
//        if(denom == 0.0) {  //Lines don't seem to cross
//            output.x = NAN;
//            output.y = NAN;
//            return false;
//        }
//
//        output.x = xnom / denom;
//        output.y = ynom / denom;
//        if(!isfinite(output.x) || !isfinite(output.y)) { //Probably a numerical issue
//            return false;
//        }
//
//        return true; //All OK
//    }


#ifdef ANDROID
    GCompositeOperation StringToGlobalCompositeOp(const char *str) {
        GCompositeOperation op;
        if (strcmp(str, "source-over") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_SOURCE_OVER;
        } else if (strcmp(str, "source-atop") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_SOURCE_ATOP;
        } else if (strcmp(str, "source-in") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_SOURCE_IN;
        } else if (strcmp(str, "source-out") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_SOURCE_OUT;
        } else if (strcmp(str, "destination-over") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_DESTINATION_OVER;
        } else if (strcmp(str, "destination-atop") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_SOURCE_ATOP;
        } else if (strcmp(str, "destination-in") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_DESTINATION_IN;
        } else if (strcmp(str, "destination-out") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_DESTINATION_OUT;
        } else if (strcmp(str, "lighter") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_LIGHTER;
        } else if (strcmp(str, "xor") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_XOR;
        } else if (strcmp(str, "copy") == 0) {
            op = GCompositeOperation::COMPOSITE_OP_COPY;
        } else {
            op = GCompositeOperation::COMPOSITE_OP_NONE;
        }
        return op;
    }


    std::string GlobalCompositeOpToString(GCompositeOperation &op) {
        std::string result = "";
        switch (op) {
            case COMPOSITE_OP_SOURCE_OVER:
                result = "source-over";
                break;
            case COMPOSITE_OP_SOURCE_ATOP:
                result = "source-atop";
                break;
            case COMPOSITE_OP_SOURCE_IN:
                result = "source-in";
                break;
            case COMPOSITE_OP_SOURCE_OUT:
                result = "source-out";
                break;
            case COMPOSITE_OP_DESTINATION_OVER:
                result = "destination-over";
                break;
            case COMPOSITE_OP_DESTINATION_ATOP:
                result = "destination-atop";
                break;
            case COMPOSITE_OP_DESTINATION_IN:
                result = "destination-in";
                break;
            case COMPOSITE_OP_DESTINATION_OUT:
                result = "destination-out";
                break;
            case COMPOSITE_OP_LIGHTER:
                result = "lighter";
                break;
            case COMPOSITE_OP_COPY:
                result = "copy";
                break;
            case COMPOSITE_OP_XOR:
                result = "xor";
                break;
        }
        return result;
    }



    void timeraddMS(struct timeval *a, uint ms) {
        a->tv_usec += ms * 1000;
        if (a->tv_usec >= 1000000) {
            a->tv_sec += a->tv_usec / 1000000;
            a->tv_usec %= 1000000;
        }
    }

    void waitUtilTimeout(sem_t *sem, uint ms) {
#ifdef DEBUG
        sem_wait(sem);
#else
        int ret;
        struct timeval now;
        struct timespec outtime;

        gettimeofday(&now, NULL);
        timeraddMS(&now, ms);
        outtime.tv_sec = now.tv_sec;
        outtime.tv_nsec = now.tv_usec * 1000;
        ret = sem_timedwait(sem, &outtime);
        if (ret == -1) {
            gettimeofday(&now, NULL);
            LOG_D("wait time out,sec=%d,usec=%d\n", now.tv_sec, now.tv_usec);
        } else {
            gettimeofday(&now, NULL);
        }
#endif
    }

#endif

}
