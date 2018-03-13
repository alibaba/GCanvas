/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GTriangulate__
#define __GCanvas__GTriangulate__


#include "GPoint.h"
#include <vector>

typedef GPoint Vector2d;

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.
typedef std::vector< Vector2d > Vector2dVector;

class Triangulate
{
public:
    // triangulate a contour/polygon, places results in STL vector
    // as series of triangles.
    static bool Process(const Vector2dVector &contour, Vector2dVector &result);

    // compute area of a contour/polygon
    static float Area(const Vector2dVector &contour);

    // decide if point Px/Py is inside triangle defined by
    // (Ax,Ay) (Bx,By) (Cx,Cy)
    static bool InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx,
                               float Cy, float Px, float Py);

private:
    static bool Snip(const Vector2dVector &contour, int u, int v, int w, int n,
                     int *V);
};

#endif
