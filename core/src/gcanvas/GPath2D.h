/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/
#ifndef GCANVAS_GPATH2D_H
#define GCANVAS_GPATH2D_H


#include "GL/GGL.h"
#include "GTransform.h"
#include "GPath.h"
#include <vector>


enum GPathCmdType {
    MOVE_TO,
    LINE_TO,
    QUADRATIC_CURVE_TO,
    BEZIER_CURVE_TO,
    ARC_TO,
    ARC,
    ELLIPSE,
    RECT,
    CLOSE_PATH
};


struct GPathCmd {

    GPathCmd () {
        byteLen = 0;
        data = nullptr;
    }

    ~GPathCmd() {
        if (data != nullptr) {
            free (data);
        }
    }

    GPathCmdType cmdType;
    size_t byteLen;
    void* data;
};



/**
 * GPath2D object, for implement Canvas Path2d interface
 */
class GPath2D {

public:

    API_EXPORT GPath2D();


    API_EXPORT ~GPath2D();


    API_EXPORT GPath2D(const GPath2D &other);


    API_EXPORT void AddPath(GPath2D&, GTransform& transform);


    API_EXPORT void ClosePath();


    API_EXPORT void MoveTo(float x, float y);


    API_EXPORT  void LineTo(float x, float y);


    API_EXPORT void QuadraticCurveTo(float cpx, float cpy, float x, float y, float scale);


    API_EXPORT void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x,
                                  float y, float scale);

    API_EXPORT void ArcTo(float x1, float y1, float x2, float y2, float radius);


    API_EXPORT void Arc(float x, float y, float radius, float startAngle, float endAngle,
                        bool antiClockwise2);


    API_EXPORT void Ellipse(float x, float y, float radiusX, float radiusY, float rotation, float startAngle,
                            float endAngle, bool antiClockwise);


    API_EXPORT void Rect(int x, int y, int w, int h);


    void WriteToPath(GPath& path);


private:

    void ClearPath();


    void TransformPathCmd(GPathCmd* cmd,  GTransform& transform);


    void AddPaths(std::vector<GPathCmd*>&, GTransform* transform = nullptr);


private:


    std::vector<GPathCmd*> cmds;


};


#endif //GCANVAS_GPATH2D_H
