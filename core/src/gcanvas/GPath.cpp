/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GPath.h"
#include "GCanvas2dContext.h"

#define  G_PATH_RECURSION_LIMIT 8
#define  G_PATH_DISTANCE_EPSILON 1.0f
#define  G_PATH_COLLINEARITY_EPSILON FLT_EPSILON
#define  G_PATH_STEPS_FOR_CIRCLE 48.0f
#define  G_PATH_ANGLE_EPSILON = 0.01;

#define  ERROR_DEVIATION    1e-6
#define  PI_1               M_PI
#define  PI_2               2.f * M_PI
#define  DEFAULT_STEP_COUNT 100

#define PATH_MIN_VALID 0.01f



static void NormalizeAngles(float& startAngle, float& endAngle, bool anticlockwise) {
    float pi2 = PI_2;

    float newStartAngle = startAngle;
    if (newStartAngle < 0) {
        newStartAngle = pi2 + fmodf(newStartAngle, -pi2);
    } else {
        newStartAngle = fmodf(newStartAngle, pi2);
    }

    float delta = newStartAngle - startAngle;
    startAngle = newStartAngle;
    endAngle = endAngle + delta;

    assert((newStartAngle >= 0 && newStartAngle < pi2));

    if (anticlockwise && startAngle - endAngle >= pi2) {
        endAngle = startAngle - pi2;
    } else if (!anticlockwise && endAngle - startAngle >= pi2) {
        endAngle = startAngle + pi2;
    }
}



GPath::GPath() {
    Reset();
    mTransform = GTransformIdentity;
}


GPath::GPath(const GPath &other) {
    needNewSubPath = other.needNewSubPath;
    mStartPosition = other.mStartPosition;
    mCurrentPosition = other.mCurrentPosition;
    mTransform = other.mTransform;
    mClipFillRule = other.mClipFillRule;
    mMinPosition = other.mMinPosition;
    mMaxPosition = other.mMaxPosition;

    mClipFillRule = other.mClipFillRule;

    ClearSubPath();

    for (int i = 0; i < other.mPathStack.size(); i++) {
        GSubPath* tt = other.mPathStack[i];
        GSubPath* nitem = new GSubPath();
        nitem->points = tt->points;
        nitem->isClosed = tt->isClosed;
        mPathStack.push_back(nitem);
    }
}


GPath::~GPath() {
    ClearSubPath();
}


void GPath::Reset() {
    ClearSubPath();

    needNewSubPath = true;

    mStartPosition = PointMake(0, 0);
    mCurrentPosition = PointMake(0, 0);

    mClipFillRule = FILL_RULE_NONZERO;
    mMinPosition = PointMake(INFINITY, INFINITY);
    mMaxPosition = PointMake(-INFINITY, -INFINITY);
}


void GPath::ClearSubPath() {
    for (int i = 0; i < mPathStack.size(); i++) {
        delete mPathStack[i];
    }
    mPathStack.clear();
}



/**
 * The closePath() method, when invoked, must do nothing if the object's path has no subpaths.
 * Otherwise, it must mark the last subpath as closed,
 * create a new subpath whose first point is the same as the previous subpath's first point,
 * and finally add this new subpath to the path.
 */
void GPath::Close() {
    if (mPathStack.size() <= 0) {
        return;
    }

    GetCurPath()->isClosed = true;
    PushPoint(mStartPosition.x, mStartPosition.y);

    // new sub path at prev path start position
    NewSubPath(mStartPosition.x, mStartPosition.y);
}



void GPath::EnsureSubPathAtPoint(float x, float y) {
    if (!needNewSubPath) {
        return;
    }

    NewSubPath(x, y);
    needNewSubPath = false;
}


void GPath::MoveTo(float x, float y) {
    GPoint p = TransformPoint(x, y);
    NewSubPath(p.x, p.y);
    needNewSubPath = false;
}


void GPath::LineTo(float x, float y) {
    GPoint p = TransformPoint(x, y);
    if (mPathStack.size() <= 0) {
        EnsureSubPathAtPoint(p.x, p.y);
    } else {
        PushPoint(p.x, p.y);
    }
}


void GPath::Rect(float x, float y, float w, float h) {
    MoveTo(x, y);
    LineTo(x + w, y);
    LineTo(x + w, y + h);
    LineTo(x, y + h);
    Close();
}



void GPath::QuadraticCurveTo(float cpx, float cpy, float x, float y) {
    GPoint cp = TransformPoint(cpx, cpy);
    GPoint p = TransformPoint(x, y);

    EnsureSubPathAtPoint(cp.x, cp.y);

    float x0 = mCurrentPosition.x;
    float y0 = mCurrentPosition.y;

    GPoint points[4] = {
            PointMake(x0, y0),
            PointMake(x0 + 2.0f/3.0f*(cp.x-x0), y0 + 2.0f/3.0f*(cp.y-y0)),
            PointMake(p.x + 2.0f/3.0f*(cp.x-p.x), p.y + 2.0f/3.0f*(cp.y-p.y)),
            PointMake(p.x, p.y),
    };

    SubdivideCubicTo(this, points);
}


void GPath::BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y,
                          float x, float y) {
    // mDistanceTolerance = G_PATH_DISTANCE_EPSILON / scale;
    // mDistanceTolerance *= mDistanceTolerance;
    GPoint cp1 = TransformPoint(cp1x, cp1y);
    GPoint cp2 = TransformPoint(cp2x, cp2y);
    GPoint p = TransformPoint(x, y);

    EnsureSubPathAtPoint(cp1.x, cp1.y);

    GPoint points[4] = {
            PointMake(mCurrentPosition.x, mCurrentPosition.y),
            PointMake(cp1.x, cp1.y),
            PointMake(cp2.x, cp2.y),
            PointMake(p.x, p.y),
    };

    SubdivideCubicTo(this, points);
}



/**
 * TODO
 */
API_EXPORT void GPath::Ellipse(float x, float y, float radiusX, float radiusY, float rotation, float startAngle,
                               float endAngle, bool antiClockwise) {

}



void GPath::ArcTo(float x1, float y1, float x2, float y2, float radius) {
    // Lifted from http://code.google.com/p/fxcanvas/
    GPoint cp1 = TransformPoint(x1, y1);
    EnsureSubPathAtPoint(cp1.x, cp1.y);

    GPoint cp = GPointApplyGTransform(mCurrentPosition, GTransformInvert(mTransform));

    float a1 = cp.y - y1;
    float b1 = cp.x - x1;
    float a2 = y2 - y1;
    float b2 = x2 - x1;
    float mm = fabsf(a1 * b2 - b1 * a2);

    if (mm < 1.0e-8 || radius == 0) { // points at same line, just draw straight line
        LineTo(x1, y1);
    } else {
        float dd = a1 * a1 + b1 * b1;
        float cc = a2 * a2 + b2 * b2;
        float tt = a1 * a2 + b1 * b2;
        float k1 = radius * sqrtf(dd) / mm;
        float k2 = radius * sqrtf(cc) / mm;
        float j1 = k1 * tt / dd;
        float j2 = k2 * tt / cc;
        float cx = k1 * b2 + k2 * b1;
        float cy = k1 * a2 + k2 * a1;
        float px = b1 * (k2 + j1);
        float py = a1 * (k2 + j1);
        float qx = b2 * (k1 + j2);
        float qy = a2 * (k1 + j2);
        float startAngle = atan2f(py - cy, px - cx);
        float endAngle = atan2f(qy - cy, qx - cx);

        Arc(cx + x1, cy + y1, radius, startAngle, endAngle, (b1 * a2 > b2 * a1));
    }
}


void GPath::Arc(float cx, float cy, float radius, float startAngle,
                float endAngle, bool antiClockwise) {
    NormalizeAngles(startAngle, endAngle, antiClockwise);

    // calc spanAngle with antiClockwise flag
    float spanAngle = endAngle - startAngle;
    if (antiClockwise) {
        spanAngle = -spanAngle;
    }

    float spanAngleAbs = fabs(spanAngle);
    if (spanAngleAbs < ERROR_DEVIATION) { // the same angle, do nothing
        return;
    }

    float pi2 = PI_2;
    while (spanAngle < 0) {
        spanAngle += pi2 * 2;
    }
    while (spanAngle - pi2 > ERROR_DEVIATION) {
        spanAngle -= pi2;
    }

    bool isCircle = (fabs(spanAngle - pi2) < ERROR_DEVIATION);
    int stepCount = DEFAULT_STEP_COUNT;
    if (!isCircle) {
        stepCount = (int)(DEFAULT_STEP_COUNT * (spanAngle / pi2));
    }

    if (stepCount < 1) {
        stepCount = 1;
    }
    float deltaAngle = spanAngle / stepCount;
    if (antiClockwise) {
        deltaAngle = -deltaAngle;
    }

    float dx = cosf(deltaAngle);
    float dy = sinf(deltaAngle);

    float xOffset = radius * cosf(startAngle);
    float yOffset = radius * sinf(startAngle);

    GPoint point;
    for (int step = 0; step <= stepCount; ++step) {
        point = TransformPoint(cx + xOffset, cy + yOffset);
        if (step == 0) {
            if (mPathStack.size() <= 0) {
                EnsureSubPathAtPoint(point.x, point.y);
            } else {
                PushPoint(point.x, point.y);
            }
        } else {
            PushPoint(point);
        }

        float oldX = xOffset, oldY = yOffset;
        xOffset = oldX * dx - oldY * dy;
        yOffset = oldX * dy + oldY * dx;
    }
}


void GPath::NewSubPath(float x, float y) {
    GPoint pos = PointMake(x, y);
    GSubPath* subPath = new GSubPath();
    subPath->isClosed = false;
    mPathStack.push_back(subPath);

    PushPoint(x, y);
    mStartPosition = pos;
}


GSubPath* GPath::GetCurPath() {
    return mPathStack[mPathStack.size() - 1];
}


void GPath::PushPoint(GPoint pt) { PushPoint(pt.x, pt.y); }



void GPath::PushPoint(float x, float y) {
    GPoint p = PointMake(x, y);

    GSubPath* curPath = GetCurPath();
    if (!curPath->points.empty()) {
        if (mCurrentPosition.x == x && mCurrentPosition.y == y) {
            return;
        }
    }

    curPath->points.push_back(p);
    mCurrentPosition.x = x;
    mCurrentPosition.y = y;
    mMinPosition.x = std::min<float>( mMinPosition.x, x );
    mMinPosition.y = std::min<float>( mMinPosition.y, y );
    mMaxPosition.x = std::max<float>( mMaxPosition.x, x );
    mMaxPosition.y = std::max<float>( mMaxPosition.y, y );
}



void GPath::ClipRegion(GCanvasContext *context) {
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    // clip use high mask bit
    GLuint mask = 0x80;
    GLuint ref = 0x80;
    glStencilMask(mask);

    // use stencil func
    if (context->HasClipRegion()) {
        glStencilFunc(GL_EQUAL, ref, mask);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    } else {
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, ref, mask);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    }

    for (auto pathIter = mPathStack.begin(); pathIter != mPathStack.end(); ++pathIter) {
        const std::vector<GPoint> &path = (*pathIter)->points;
        if (path.size() < 3) {
            continue;
        }

        glVertexAttribPointer(context->PositionSlot(), 2, GL_FLOAT, GL_FALSE,
                              sizeof(GPoint), &(path.front()));
        context->mDrawCallCount++;
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) path.size());
    }
    context->BindPositionVertexBuffer();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // reset stencil
    SetStencilForClip();
}


//void GPath::PushTriangleFanPoints(GCanvasContext *context, tSubPath* subPath, GColorRGBA color) {
//    std::vector<GPoint> &pts = subPath->points;
//    if (subPath->isClosed) {
//        context->PushTriangleFanPoints(pts, color);
//    } else {
//        // push head point
//        pts.push_back(pts.front());
//        context->PushTriangleFanPoints(pts, color);
//        pts.pop_back();
//    }
//    context->SendVertexBufferToGPU(GL_TRIANGLE_FAN);
//}


void GPath::Fill(GCanvasContext *context, GFillRule rule, GFillTarget target, bool needTransform) {
    context->SendVertexBufferToGPU();
    
    GColorRGBA color = BlendColor(context, context->mCurrentState->mFillColor);
    
    // Disable drawing to the color buffer, enable the stencil buffer
    if (context->mCurrentState->mShader) {
        if (context->mCurrentState->mShader->GetTexcoordSlot() > 0) {
            glDisableVertexAttribArray((GLuint)context->mCurrentState->mShader->GetTexcoordSlot());
        }
        if (context->mCurrentState->mShader->GetColorSlot() > 0) {
            glDisableVertexAttribArray((GLuint)context->mCurrentState->mShader->GetColorSlot());
        }
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xff);
    
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // Clear the needed area in the stencil buffer
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    GColorRGBA white = {1,1,1,1};
    
    GPoint minPos = mMinPosition;
    GPoint maxPos = mMaxPosition;
    
    GTransform transform = needTransform ? mTransform : GTransformIdentity;
    
    if(needTransform) {
        minPos = GPointApplyGTransform(minPos, transform);
        maxPos = GPointApplyGTransform(maxPos, transform);
    }

    context->PushRectangle(minPos.x, minPos.y, maxPos.x-minPos.x, maxPos.y-minPos.y, 0, 0, 0, 0, white);
    context->SendVertexBufferToGPU();
    
    if( rule == FILL_RULE_NONZERO ) {
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    } else if( rule == FILL_RULE_EVENODD ) {
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    }
    
    for (auto iter = mPathStack.begin(); iter != mPathStack.end(); ++iter) {
        GSubPath* path = (*iter);
        // ignore not valid subPath (less 2 points)
        if (path->points.size() <= 2) {
             continue;
        }
        
        if (needTransform) {
            // fillBlur need apply transform
            std::vector<GPoint> pointVector;
            for (auto it = path->points.begin() ; it != path->points.end(); ++it) {
                GPoint pt = GPointApplyGTransform(*it, transform);
                pointVector.push_back(pt);
            }
            if (context->mCurrentState->mShader) {
                glVertexAttribPointer((GLuint) context->mCurrentState->mShader->GetPositionSlot(), 2,
                                      GL_FLOAT, GL_FALSE, 0,
                                      &(pointVector.front()));
            }
            context->mDrawCallCount++;
            glDrawArrays(GL_TRIANGLE_FAN, 0, (int)pointVector.size());
        } else {
            if (context->mCurrentState->mShader) {
                glVertexAttribPointer((GLuint) context->mCurrentState->mShader->GetPositionSlot(), 2,
                                      GL_FLOAT, GL_FALSE, 0,
                                      &(path->points.front()));
            }
        	context->mDrawCallCount++;
            glDrawArrays(GL_TRIANGLE_FAN, 0, (int)path->points.size());
        }
    }
    
    context->BindVertexBuffer();
    
    //enable color or depth buffer, push a rect with the correct size and color
    if( target == FILL_TARGET_DEPTH ) {
        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
    } else if( target == FILL_TARGET_COLOR ) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_BLEND);
    }
    
    glStencilFunc(GL_NOTEQUAL, 0x00, 0xff);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    
    context->PushRectangle(minPos.x, minPos.y, maxPos.x - minPos.x, maxPos.y - minPos.y, 0, 0, 0, 0, color);
    context->SendVertexBufferToGPU();

    glDisable(GL_STENCIL_TEST);
    
    if( target == FILL_TARGET_DEPTH ) {
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);
        
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_BLEND);
    }
}


std::vector<GSubPath*>* GPath::CreateLineDashPath(GCanvasContext *context) {
    std::vector<float> lineDash = context->LineDash();
    std::vector<float> firstLineDash = context->LineDash();
    bool firstNeedDraw = true;
    int firstDashIndex = 0;
    float lineDashOffset = context->LineDashOffset();
    // deal offset
    if (lineDashOffset > 0.1 && lineDashOffset < 50) {
        float totalLength = 0;
        for (int i = 0; i < lineDash.size(); i++) {
            totalLength += lineDash[i];
        }
        while (lineDashOffset > totalLength) {
            lineDashOffset -= totalLength;
            if (lineDash.size() % 2 == 1) {
                firstNeedDraw = !firstNeedDraw;
            }
        }

        for (int i = 0; i < firstLineDash.size(); i++) {
            float dash = firstLineDash[i];
            if (dash > lineDashOffset) {
                firstLineDash[i] = dash - lineDashOffset;
                break;
            } else {
                firstNeedDraw = !firstNeedDraw;
                lineDashOffset -= dash;
                firstLineDash[i] = 0;
                firstDashIndex++;
                if (i == firstLineDash.size() - 1) {
                    // reset
                    firstDashIndex = 0;
                    firstLineDash = lineDash;
                }
            }
        }
    }

    std::vector<GSubPath*> *tmpPathStack = new std::vector<GSubPath*>;
    for (auto iter = mPathStack.begin(); iter != mPathStack.end(); ++iter) {
        const std::vector<GPoint> &pts = (*iter)->points;
        GPoint startPoint, stopPoint;
        if (pts.size() <= 1) {
            continue;
        }

        // dash width
        int dashIndex = firstDashIndex;
        float dashWidth = firstLineDash[dashIndex];
        bool needDraw = firstNeedDraw;
        bool isFirst = true;

        // new tmp path
        GSubPath tmpPath;
        tmpPath.isClosed = false;
        float currentWidth = 0;
        stopPoint = *(pts.begin());
        tmpPath.points.push_back(stopPoint);

        for (auto ptIter = pts.begin() + 1; ptIter != pts.end();) {
            startPoint = stopPoint;
            stopPoint = *ptIter;
            // calc cur length
            float deltaX = stopPoint.x - startPoint.x;
            float deltaY = stopPoint.y - startPoint.y;
            float length = sqrtf(deltaX * deltaX + deltaY * deltaY);

            if (length + currentWidth > dashWidth) {
                // insert point
                float gapWidth = dashWidth - currentWidth;
                float interDeltaX = deltaX * gapWidth / length;
                float interDeltaY = deltaY * gapWidth / length;
                GPoint interPoint = PointMake(interDeltaX + startPoint.x, interDeltaY + startPoint.y);

                tmpPath.points.push_back(interPoint);

                if (needDraw) {
                    GSubPath* rsubPath = new GSubPath();
                    rsubPath->points = tmpPath.points;
                    tmpPathStack->push_back(rsubPath);
                }

                // new path, update variables
                tmpPath.points.clear();
                tmpPath.isClosed = false;
                needDraw = !needDraw;
                currentWidth = 0;
                tmpPath.points.push_back(interPoint);
                stopPoint = interPoint;

                // update dash
                if (isFirst) {
                    dashIndex++;
                    if (dashIndex >= firstLineDash.size()) {
                        isFirst = false;
                        dashIndex = 0;
                        dashWidth = lineDash[dashIndex];
                    } else {
                        dashWidth = firstLineDash[dashIndex];
                    }
                } else {
                    dashIndex = (dashIndex + 1) % lineDash.size();
                    dashWidth = lineDash[dashIndex];
                }
                continue;
            } else {
                tmpPath.points.push_back(stopPoint);
                currentWidth += length;
                if (ptIter == pts.end() - 1) {
                    // finished，push vertex
                    if (needDraw) {
                        GSubPath* rsubPath = new GSubPath();
                        rsubPath->points = tmpPath.points;
                        tmpPathStack->push_back(rsubPath);
                    }
                }
            }
            ++ptIter;
        }
    }
    return tmpPathStack;
}


void GPath::Stroke(GCanvasContext *context, GColorRGBA color, std::vector<GVertex> *vertexVec) {
    stroker.StrokePath(context, this, vertexVec);
}


void GPath::Stroke(GCanvasContext *context) {
    context->SetTexture(InvalidateTextureId);
    GColorRGBA color = BlendStrokeColor(context);
    std::vector<GVertex> vertexVec;
    if (color.rgba.a < 1.0) { //transparent, use stencil buffer
        Stroke(context, color, &vertexVec);
        StencilRectForStroke(context, vertexVec);
    } else {
        Stroke(context, color, NULL);
    }
}


void GPath::DrawVertexToContext(GCanvasContext *context, std::vector<GVertex> &vertexVec) {
    context->SetTexture(InvalidateTextureId);
    GColorRGBA color = BlendStrokeColor(context);
    
    if (color.rgba.a < 1.0) { //transparent, use stencil buffer
        StencilRectForStroke(context, vertexVec);
    } else {
        context->PushVertexs(vertexVec);
    }
}



void GPath::StencilRectForStroke(GCanvasContext *context, std::vector<GVertex> &vertexVec) {
    context->SendVertexBufferToGPU();
    GColorRGBA color = BlendStrokeColor(context);
    
    //set environment
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    
    // clear stencil buffer
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    GColorRGBA white = {1,1,1,1};
    
    float scale = GTransformGetScaleX(mTransform);
    float width = scale * context->LineWidth();
    float extend = context->MiterLimit() * width;
    if (extend < width * 0.5) {
        extend = width * 0.5;
    }
    
    GRect rect;
    rect.x = mMinPosition.x - extend;
    rect.y = mMinPosition.y - extend;
    rect.width = mMaxPosition.x - mMinPosition.x + extend * 2;
    rect.height = mMaxPosition.y - mMinPosition.y + extend * 2;
    if (rect.height > context->GetHeight() || rect.width > context->GetWidth()) {
        rect.x = 0;
        rect.y = 0;
        rect.width = context->GetWidth();
        rect.height = context->GetHeight();
    }
    
    context->PushRectangle(rect.x, rect.y, rect.width, rect.height, 0, 0, 0, 0, white);
    context->SendVertexBufferToGPU();
    
    // first draw stencil
    glStencilFunc(GL_ALWAYS, 0x1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    context->PushVertexs(vertexVec);
    context->SendVertexBufferToGPU();
    
    // second draw color buffer with stencil buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0x00, 0xff);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    
    context->PushRectangle(rect.x, rect.y, rect.width, rect.height, 0, 0, 0, 0, color);
    context->SendVertexBufferToGPU();
    
    // reset
    glDisable(GL_STENCIL_TEST);
}


void GPath::SubdivideCubicTo(GPath *path, GPoint points[4], int level) {
    if (--level >= 0) {
        GPoint tmp[7];

        ChopCubicAt(points, tmp, 0.5f);
        SubdivideCubicTo(path, &tmp[0], level);
        SubdivideCubicTo(path, &tmp[3], level);
    } else {
        path->PushPoint(points[1]);
        path->PushPoint(points[2]);
        path->PushPoint(points[3]);
    }
}


void GPath::ChopCubicAt(GPoint src[4], GPoint dst[7], float t) {
    GPoint tt = PointMake(t, t);

    GPoint ab = Interp(src[0], src[1], tt);
    GPoint bc = Interp(src[1], src[2], tt);
    GPoint cd = Interp(src[2], src[3], tt);
    GPoint abc = Interp(ab, bc, tt);
    GPoint bcd = Interp(bc, cd, tt);
    GPoint abcd = Interp(abc, bcd, tt);

    dst[0] = src[0];
    dst[1] = ab;
    dst[2] = abc;
    dst[3] = abcd;
    dst[4] = bcd;
    dst[5] = cd;
    dst[6] = src[3];
}


GPoint GPath::Interp(const GPoint &v0, const GPoint &v1, const GPoint &t) {
    return PointMake(v0.x + (v1.x - v0.x) * t.x, v0.y + (v1.y - v0.y) * t.y);
}


void GPath::GetRect(GRectf& rect) {
    rect.leftTop = mMinPosition;
    rect.bottomRight = mMaxPosition;
    rect.isTransformed = true;
}


void GPath::GetRectCoverVertex(GRectf &rect, std::vector<GVertex> &vertexVec) {
    float topX = 0, topY = 0, bottomX = 0, bottomY = 0;
    for (int i = 0; i < vertexVec.size(); i++) {
        GVertex &vertex = vertexVec[i];
        if (i == 0) {
            topX = vertex.pos.x;
            topY = vertex.pos.y;
            bottomX = topX;
            bottomY = topY;
            continue;
        }
        
        topX = std::min<float>( topX, vertex.pos.x );
        topY = std::min<float>( topY, vertex.pos.y );
        bottomX = std::max<float>( bottomX, vertex.pos.x );
        bottomY = std::max<float>( bottomY, vertex.pos.y );
    }
    rect.leftTop = PointMake(topX, topY);
    rect.bottomRight = PointMake(bottomX, bottomY);
}


//void GPath::RestoreStencilForClip(GCanvasContext *context) {
//    if (context->HasClipRegion()) {
//        // reset stencil
//        glClear(GL_STENCIL_BUFFER_BIT);
//        SetStencilForClip();
//    } else {
//        // disable
//        glStencilMask(0xFF);
//        glClear(GL_STENCIL_BUFFER_BIT);
//        glDisable(GL_STENCIL_TEST);
//    }
//}


void GPath::SetStencilForClip() {
    GLuint mask = 0x80;
    glStencilMask(mask);
    glStencilFunc(GL_EQUAL, mask, mask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


GPoint GPath::TransformPoint(float x, float y) {
    if(!GTransformIsIdentity(mTransform) ){
        return GPointApplyGTransform(PointMake(x, y), mTransform);
    } else {
        return PointMake(x, y);
    }
}


inline double calc_distance(double x1, double y1, double x2, double y2) {
    double dx = x2-x1;
    double dy = y2-y1;
    return sqrt(dx * dx + dy * dy);
}


inline double calc_sq_distance(double x1, double y1, double x2, double y2) {
    double dx = x2-x1;
    double dy = y2-y1;
    return dx * dx + dy * dy;
}
