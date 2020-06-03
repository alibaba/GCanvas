//
// Created by zhanqu on 2020/4/26.
//

#ifndef GCANVAS_GPATHSTROKER_H
#define GCANVAS_GPATHSTROKER_H


#include "GPoint.h"
#include "GTransform.h"


class GPath;
class GCanvasContext;



/**
 *    [out]           [out]
 *     |              |
 *    from  ------->  to
 *     |              |
 *    [in]            [in]
 */
struct GPathOutLine {
    GPoint from;
    GPoint to;

    float offsetX;
    float offsetY;

    GPoint fromIn;
    GPoint fromOut;

    GPoint toIn;
    GPoint toOut;
};



class GPathStroker {

public:

    void StrokePath(GCanvasContext *context, GPath *path, std::vector<GVertex> *vertexVec) ;


private:


    void FilterTooClosePoints(std::vector<GPoint> &pts);


    void DrawLineJoin(GCanvasContext* context, float halfLineWidth, GPathOutLine& lineOne, GPathOutLine& lineTwo,
                             GColorRGBA color, GTransform& transform, std::vector<GVertex> *vertexVec);


    void DrawArcForCapOrJoin(GCanvasContext *context, float halfLineWidth,
                                           GPoint& center, GPoint& p1, GPoint& p2,
                                           GColorRGBA color, GTransform& transform, std::vector<GVertex> *vec,
                                           float samePointThreshold);


    void DrawMiterJoin(GCanvasContext *context, float halfLineWidth,
                       const GPoint &center, const GPoint &p1, const GPoint &p2,
                       GColorRGBA color, GTransform transform, std::vector<GVertex> *vec);


    void DrawLineCap(GCanvasContext *context, float halfLineWidth,
                                   GPoint &center, GPoint &p1, GPoint &p2, float deltaX, float deltaY,
                                   GColorRGBA color, GTransform& transform, std::vector<GVertex> *vec,
                                   float samePointThreshold = 0.001f);
};




#endif //GCANVAS_GPATHSTROKER_H
