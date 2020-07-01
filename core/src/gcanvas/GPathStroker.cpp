/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/
#include "GPathStroker.h"
#include "GPath.h"
#include "GCanvas2dContext.h"
#include "../support/Util.h"
#include "../support/Log.h"

#define  PI_1               M_PI
#define  PI_2               2.f * M_PI
#define PATH_MIN_VALID 0.01f


//void LogPathOutline(const char* prefix, GPathOutLine& outline) {
//    LOG_E("%s:from(%f,%f),to(%f,%f),fromIn(%f,%f),fromOut(%f,%f),toIn(%f,%f),toOut(%f,%f)",
//          prefix, outline.from.x, outline.from.y, outline.to.x, outline.to.y,
//          outline.fromIn.x, outline.fromIn.y, outline.fromOut.x, outline.fromOut.y,
//          outline.toIn.x, outline.toIn.y, outline.toOut.x, outline.toOut.y);
//}


static void CalculateOutlinePoint(GPathOutLine& outline, float outOffsetX, float outOffsetY) {
    outline.fromIn.x = outline.from.x - outOffsetX;
    outline.fromIn.y = outline.from.y + outOffsetY;
    outline.fromOut.x = outline.from.x + outOffsetX;
    outline.fromOut.y = outline.from.y - outOffsetY;

    outline.toIn.x = outline.to.x - outOffsetX;
    outline.toIn.y = outline.to.y + outOffsetY;
    outline.toOut.x = outline.to.x + outOffsetX;
    outline.toOut.y = outline.to.y - outOffsetY;
}


static void CalculateOutlinePoint(float halfLineWidthTrans, GPathOutLine& outline) {
    float deltaX = outline.to.x - outline.from.x;
    float deltaY = outline.to.y - outline.from.y;
    float length = sqrtf(deltaX * deltaX + deltaY * deltaY);
    float coff = halfLineWidthTrans / length;
    outline.offsetX = deltaY * coff;
    outline.offsetY = deltaX * coff;

    CalculateOutlinePoint(outline, outline.offsetX, outline.offsetY);
}


/**
 * 1.similarity transform can directly draw
 * 2.not similarity transform should first inverse transform handle lineWidth, then transform back
 */
void GPathStroker::StrokePath(GCanvasContext *context, GPath *path, std::vector<GVertex> *vertexVec) {
    GColorRGBA color = BlendStrokeColor(context);
    GTransform& transform = context->mCurrentState->mTransform;
    bool isSimilarity = GTransformIsSimilarity(context->mCurrentState->mTransform);
    bool drawCapFlag = context->LineCap() != LINE_CAP_BUTT;

    float lineWidth = context->LineWidth();
    float lineWidthTrans = lineWidth;
    if (isSimilarity) {
        lineWidthTrans = lineWidth * GTransformGetScaleX(transform);
        if (lineWidthTrans < 1) {
            return;
        }
    } else {
        float lineWX = lineWidth * GTransformGetScaleX(transform);
        float lineWY = lineWidth * GTransformGetScaleY(transform);
        if (lineWX < 1 && lineWY < 1) {
            return;
        }
    }
    float halfLineWidthTrans = 0.5f * lineWidthTrans;

    GTransform inverseTransform;
    if (!isSimilarity) {
        inverseTransform = GTransformInvert(transform);
    }
    GTransform outputTransform = isSimilarity ? GTransformIdentity : transform;

    std::vector<GSubPath*>* pathStack = &(path->mPathStack);
    // first deal line dash if needed
    if (context->LineDash().size() > 0) {
        pathStack = path->CreateLineDashPath(context);
    }

    for (auto iter = pathStack->begin(); iter != pathStack->end(); ++iter) {
        std::vector<GPoint> &pts = (std::vector<GPoint>&)(*iter)->points;

        if (pts.size() <= 1) {
            continue;
        }

        bool subPathIsClosed = (*iter)->isClosed;
        bool firstInSub = true;
        GPathOutLine outline;
        GPathOutLine nextOutline;

        GPoint firstPoint = *(pts.begin());
        if (!isSimilarity) {
            firstPoint = GPointApplyGTransform(firstPoint, inverseTransform);
        }
        GPoint secondPoint = {0, 0};

        // filter close point
        FilterTooClosePoints(pts);

        for (auto ptIter = pts.begin() + 1; ptIter != pts.end(); ++ptIter) {
            // step0: prepare point
            if (firstInSub) {
                outline.from = firstPoint;
                outline.to = *ptIter;
                if (!isSimilarity) {
                    outline.to = GPointApplyGTransform(outline.to, inverseTransform);
                }
                secondPoint = outline.to;

                CalculateOutlinePoint(halfLineWidthTrans, outline);
            }

            // step1: draw line
            context->PushQuad(outline.fromIn, outline.toIn, outline.toOut, outline.fromOut, color,
                              outputTransform, vertexVec);
            // LogPathOutline("pushquad outline: ", outline);

            // step2: draw head cap
            if (firstInSub) {
                firstInSub = false;
                if (drawCapFlag && !subPathIsClosed) {
                    DrawLineCap(context, halfLineWidthTrans, outline.from,
                                outline.fromOut, outline.fromIn,
                                -outline.offsetY, -outline.offsetX, color, outputTransform, vertexVec);
                }
            }

            // step3: draw tail cap
            // subPath is end && subPath not closed, try draw cap (no need draw join)
            auto nextIter = ptIter + 1;
            if (!subPathIsClosed && (nextIter == pts.end())) {
                if (drawCapFlag) {
                    DrawLineCap(context, halfLineWidthTrans, outline.to,
                                outline.toIn, outline.toOut, outline.offsetY, outline.offsetX, color,
                                outputTransform, vertexVec);
                }
                // no need draw line join, exit iterate
                break;
            }

            // step4: draw line join
            GPoint nextCenter;
            if (nextIter != pts.end()) {
                nextCenter = *nextIter;
                if (!isSimilarity) {
                    nextCenter = GPointApplyGTransform(nextCenter, inverseTransform);
                }
            } else {
                nextCenter = secondPoint;
            }

            nextOutline.from = outline.to;
            nextOutline.to = nextCenter;

            CalculateOutlinePoint(halfLineWidthTrans, nextOutline);
            DrawLineJoin(context, halfLineWidthTrans, outline, nextOutline, color, outputTransform, vertexVec);
            // use nextOutline, avoid compute again
            outline = nextOutline;
        }
    }

    // delete lineDash tmp pathStack
    if (context->LineDash().size() > 0) {
        delete pathStack;
    }
}


void GPathStroker::FilterTooClosePoints(std::vector<GPoint> &pts) {
    float minValidValue = PATH_MIN_VALID;
    GPoint stopPoint = *(pts.begin());

    GPoint prevPoint = stopPoint;
    float distance;

    for (auto ptIter = pts.begin() + 1; ptIter != pts.end();) {
        GPoint curPoint = *ptIter;
        distance = gcanvas::PointDistance(curPoint, prevPoint);
        if (distance < minValidValue) {
            ptIter = pts.erase(ptIter);
        } else {
            prevPoint = curPoint;
            ptIter++;
        }
    }
}



void GPathStroker::DrawLineJoin(GCanvasContext* context, float halfLineWidth, GPathOutLine& lineOne, GPathOutLine& lineTwo,
                                GColorRGBA color, GTransform& transform, std::vector<GVertex> *vertexVec) {
    // angle changed
    float nextAngle = gcanvas::CalcPointAngle(lineTwo.to, lineTwo.from);
    float curAngle = gcanvas::CalcPointAngle(lineOne.from, lineOne.to);
    float angleDiff = nextAngle - curAngle;
    if (angleDiff < 0) {
        angleDiff += PI_1 * 2;
    }
    // LOG_E("angle check:next=%f, curAngle=%f, diff=%f", nextAngle, curAngle, angleDiff);
    // resolve use which side outline points
    GPoint joinP1,  joinP2;
    if (angleDiff > PI_1) {
        joinP1 = lineTwo.fromOut;
        joinP2 = lineOne.toOut;
    } else {
        joinP1 = lineOne.toIn;
        joinP2 = lineTwo.fromIn;
    }

    // LOG_E("choose join point:startNear(%f,%f), endNear(%f,%f)",
    //      joinP1.x, joinP1.y, joinP2.x, joinP2.y);
    if (context->LineJoin() == LINE_JOIN_ROUND) {
        DrawArcForCapOrJoin(context, halfLineWidth, lineTwo.from, joinP1, joinP2, color,
                            transform, vertexVec, PATH_MIN_VALID);
    } else if (context->LineJoin() == LINE_JOIN_MITER) {
        DrawMiterJoin(context, halfLineWidth, lineTwo.from, joinP2, joinP1, color, transform,
                      vertexVec);
    } else if (context->LineJoin() == LINE_JOIN_BEVEL) {
        if (gcanvas::IsTrianglePointsValid(lineTwo.from, joinP1, joinP2, PATH_MIN_VALID)) {
            context->PushTriangle(lineTwo.from, joinP1, joinP2, color, transform, vertexVec);
        } else {
            // ignore
        }
    }
}



void GPathStroker::DrawArcForCapOrJoin(GCanvasContext *context, float halfLineWidth,
                             GPoint& center, GPoint& p1, GPoint& p2,
                             GColorRGBA color, GTransform& transform, std::vector<GVertex> *vec,
                             float samePointThreshold) {
    // filter invalid point
    bool fixAndroidCompatible = false;
#ifdef ANDROID
    fixAndroidCompatible = true;
#endif

    float minValue = samePointThreshold;
    // LOG_E("DrawArcForCapOrJoin p1(%f,%f),center(%f,%f),p3(%f,%f)", p1.x, p1.y, center.x,center.y, p2.x, p2.y);
    if (fixAndroidCompatible && (gcanvas::IsSamePoint(center, p1, minValue) ||
            gcanvas::IsSamePoint(p1, p2, minValue) || gcanvas::IsSamePoint(center, p2, minValue))) {
        // LOG_E("DrawArcForCapOrJoin ignore by samePoint");
        return;
    }

    // filter nan pint
    if (gcanvas::IsNanPoint(center) || gcanvas::IsNanPoint(p1) || gcanvas::IsNanPoint(p2)) {
        return;
    }

    GPoint v1 = PointNormalize(PointSub(p1, center));
    GPoint v2 = PointNormalize(PointSub(p2, center));
    float angle2;
    if (v1.x == -v2.x && v1.y == -v2.y) {
        angle2 = M_PI;
    } else {
        angle2 = acosf(v1.x * v2.x + v1.y * v2.y);
    }

    if (fixAndroidCompatible && std::isnan(angle2)) {
        return;
    }

    // 1 step per 5 pixel
    float needStep =  (angle2 * halfLineWidth) / 5.0f;
    int numSteps = 0;

    if (fixAndroidCompatible) {
        if (needStep < minValue) {
            numSteps = 0;
        } else if (needStep < 1) {
            numSteps = 1;
        } else {
            numSteps = (int)std::min<float>(64, std::max<float>(20, needStep));
        }
    } else {
        numSteps = (int)std::min<float>(64, std::max<float>(20, needStep));
    }

    if (numSteps > 0) {
        // calculate angle step
        float step = (angle2 / numSteps);
        float angle1 = atan2(1.0f, 0.0f) - atan2(v1.x, -v1.y);
        // starting point
        float angle = angle1;

        GPoint arcP1 = {center.x + cosf(angle) * halfLineWidth,
                        center.y - sinf(angle) * halfLineWidth};
        GPoint arcP2;
        for (int i = 0; i < numSteps; i++) {
            angle += step;
            arcP2 = PointMake(center.x + cosf(angle) * halfLineWidth,
                              center.y - sinf(angle) * halfLineWidth);

            context->PushTriangle(arcP1, center, arcP2, color, transform, vec);
            arcP1 = arcP2;

            // LOG_E("drawArc Join triangle:p1(%f,%f),center(%f,%f),p2(%f,%f)",
            //        arcP1.x, arcP1.x, center.x, center.y, arcP2.x, arcP2.y);
        }
    }
}


void GPathStroker::DrawLineCap(GCanvasContext *context, float halfLineWidth,
                GPoint &center, GPoint &p1, GPoint &p2, float deltaX, float deltaY,
                GColorRGBA color, GTransform& transform, std::vector<GVertex> *vec,
                float samePointThreshold) {
    if (context->LineCap() == LINE_CAP_SQUARE) {
        context->PushQuad(p1, p2, PointMake(p2.x + deltaX, p2.y + deltaY),
                          PointMake(p1.x + deltaX, p1.y + deltaY), color, transform, vec);
    } else if (context->LineCap() == LINE_CAP_ROUND) {
        DrawArcForCapOrJoin(context, halfLineWidth, center, p1, p2, color, transform, vec, samePointThreshold);
    }
}



void GPathStroker::DrawMiterJoin(GCanvasContext *context, float halfLineWidth,
                                 const GPoint &center, const GPoint &p1, const GPoint &p2,
                                 GColorRGBA color, GTransform transform, std::vector<GVertex> *vec) {
    float angle1 = gcanvas::CalcPointAngle(p1, center);
    float angle2 = gcanvas::CalcPointAngle(p2, center);

    float angleGap = (angle2 - angle1);
    if (angleGap < 0) {
        angleGap += (PI_1 * 2);
    }
    angleGap /= 2;
    float miterLen = fabsf(1 / cosf(angleGap));
    //LOG_E("miterLen:%f, angleGap=%f", miterLen, angleGap);
    if (miterLen > context->MiterLimit()) {
        context->PushTriangle(center, p1, p2, color, transform);
        return;
    }

    float miterAngle = angle1 + angleGap;
    GPoint miterPoint = { center.x + cosf(miterAngle) * miterLen * halfLineWidth,
            center.y + sinf(miterAngle) * miterLen * halfLineWidth};

    //LOG_E("DrawMiterJoin PushQuad=center(%f,%f),p1(%f,%f) miter(%f,%f),p2(%f,%f)",
    //        center.x, center.y, p1.x, p1.y, miterPoint.x, miterPoint.y, p2.x, p2.y);
    // filter opengl invalid point( eg:oppo a59 )
    if (gcanvas::IsNanPoint(p1) || gcanvas::IsNanPoint(p2) ||
            gcanvas::IsNanPoint(center) || gcanvas::IsNanPoint(miterPoint)) {
        return;
    }

    context->PushQuad(center, p1, miterPoint, p2, color, transform, vec);
}

