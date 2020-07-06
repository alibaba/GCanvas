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
#include "GPathStroker.h"
#include <float.h>
#include <iostream>
#include <vector>

class GCanvasContext;



typedef struct {
    std::vector<GPoint> points;
    bool isClosed;
} GSubPath;




class GPath {

public:
    GPath();

    GPath(const GPath &other);

    ~GPath();


    void MoveTo(float x, float y);

    void LineTo(float x, float y);

    void QuadraticCurveTo(float cpx, float cpy, float x, float y);

    void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);

    void ArcTo(float x1, float y1, float x2, float y2, float radius);

    void Arc(float x, float y, float radius, float startAngle, float endAngle,
             bool antiClockwise2);

    void Ellipse(float x, float y, float radiusX, float radiusY, float rotation, float startAngle,
                 float endAngle, bool antiClockwise);


    void Rect(float x, float y, float w, float h);

    void Close();

    void Reset();


    void ClipRegion(GCanvasContext *context);

    void Stroke(GCanvasContext *context);

    void Stroke(GCanvasContext *context, GColorRGBA color, std::vector<GVertex> *vertexVec);


    void Fill(GCanvasContext *context, GFillRule rule, GFillTarget target = FILL_TARGET_COLOR,
              bool needTransform = false);


    void StencilRectForStroke(GCanvasContext *context, std::vector<GVertex> &vertexVec);

    void DrawVertexToContext(GCanvasContext *context, std::vector<GVertex> &vertexVec);

    void GetRect(GRectf &rect);

    static void GetRectCoverVertex(GRectf &rect, std::vector<GVertex> &vertexVec);

    static void SubdivideCubicTo(GPath *path, GPoint points[4], int level = 4);

    static void ChopCubicAt(GPoint src[4], GPoint dst[7], float t);

    static inline GPoint Interp(const GPoint &v0, const GPoint &v1, const GPoint &t);

private:


    std::vector<GSubPath*> *CreateLineDashPath(GCanvasContext *context);


    void PushPoint(GPoint pt);


    void PushPoint(float x, float y);


    void EnsureSubPathAtPoint(float x, float y);


    void ClearSubPath();


    void NewSubPath(float x, float y);


    GPoint TransformPoint(float x, float y);


    GSubPath* GetCurPath();

//    void PushTriangleFanPoints(GCanvasContext *context, tSubPath* subPath, GColorRGBA color);

//    void RestoreStencilForClip(GCanvasContext *context);

    void SetStencilForClip();


private:
    GPoint mStartPosition;
    GPoint mCurrentPosition;

    std::vector<GSubPath*> mPathStack;

    GPoint mMinPosition;
    GPoint mMaxPosition;

    bool needNewSubPath;

    GPathStroker stroker;

    friend GPathStroker;

public:
    GFillRule mClipFillRule;
    GTransform mTransform;

};

#endif /* GCANVAS_GPATH_H */
