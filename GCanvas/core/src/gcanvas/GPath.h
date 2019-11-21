/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GPATH_H
#define GCANVAS_GPATH_H

#include "GPoint.h"
#include "GTransform.h"
#include <float.h>
#include <iostream>
#include <vector>

class GCanvasContext;

typedef struct {
    std::vector<GPoint> points;
    bool isClosed;
} tSubPath;

class GPath {
public:
    GPath();

    GPath(const GPath &other);
    
    void MoveTo(float x, float y);

    void LineTo(float x, float y);

    void Close();

    void Reset();

    void EndSubPath();

    void QuadraticCurveTo(float cpx, float cpy, float x, float y, float scale);

    void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x,
                       float y, float scale);

    void ArcTo(float x1, float y1, float x2, float y2, float radius);

    void Arc(float x, float y, float radius, float startAngle, float endAngle,
             bool antiClockwise);

    void ClipRegion(GCanvasContext *context);
    
    void DrawPolygons2DToContext(GCanvasContext *context, GFillRule rule, GFillTarget target = FILL_TARGET_COLOR);

    std::vector<tSubPath> *DrawLineDash(GCanvasContext *context);
    
    void CreateLinesFromPoints(GCanvasContext *context, GColorRGBA color, std::vector<GVertex> *vertexVec);
    
    void StencilRectForStroke(GCanvasContext *context, std::vector<GVertex> &vertexVec);
    
    void DrawLinesToContext(GCanvasContext *context);

    void GetRect(GRectf& rect);

    static void SubdivideCubicTo(GPath *path, GPoint points[4], int level = 4);

    static void ChopCubicAt(GPoint src[4], GPoint dst[7], float t);

    static inline GPoint interp(const GPoint &v0, const GPoint &v1, const GPoint &t);

private:
    void push(GPoint pt);

    void push(float x, float y);

    void recursiveQuadratic(float x1, float y1, float x2, float y2, float x3,
                            float y3, int level);

    void recursiveBezier(float x1, float y1, float x2, float y2, float x3,
                         float y3, float x4, float y4, int level);

    void drawArcToContext(GCanvasContext *context, GPoint point, GPoint p1,
                          GPoint p2, GColorRGBA color, std::vector<GVertex> *vec, float samePointThreshold = 0.0001f);

    void drawLineJoinMiter(GCanvasContext *context, const GPoint &center,
                           const GPoint &p1, const GPoint &p2,
                           GColorRGBA color, std::vector<GVertex> *vec);

    void drawLineCap(GCanvasContext *context, const GPoint &center,
                     const GPoint &p1, const GPoint &p2, float deltaX,
                     float deltaY, GColorRGBA color, std::vector<GVertex> *vec, float samePointThreshold = 0.0001f);

    float calcPointAngle(const GPoint &director, const GPoint &center);

    tSubPath &GetCurPath();

    void PushTriangleFanPoints(GCanvasContext *context, tSubPath* subPath, GColorRGBA color);

    void RestoreStencilForClip(GCanvasContext *context);
    void SetStencilForClip();
private:
    GPoint mStartPosition;
    GPoint mCurrentPosition;
    bool mHasInitStartPosition;
    tSubPath mCurPath;
    std::vector<tSubPath> mPathStack;
    float mDistanceTolerance;
    
    GTransform mTransfrom;
    
    GPoint mMinPosition;
    GPoint mMaxPosition;

public:
    GFillRule mFillRule;
};

#endif /* GCANVAS_GPATH_H */
