/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <stdio.h>

#include "GTriangulate.h"

static const float EPSILON = 0.0000000001f;

float Triangulate::Area(const Vector2dVector &contour)
{
    int n = (int)contour.size();

    float A = 0.0f;

    for (int p = n - 1, q = 0; q < n; p = q++)
    {
        A += contour[p].x * contour[q].y - contour[q].x * contour[p].y;
    }
    return A * 0.5f;
}

/*
 InsideTriangle decides if a point P is Inside of the triangle
 defined by A, B, C.
 */
bool Triangulate::InsideTriangle(float Ax, float Ay, float Bx, float By,
                                 float Cx, float Cy, float Px, float Py)

{
    float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    float cCROSSap, bCROSScp, aCROSSbp;

    ax = Cx - Bx;
    ay = Cy - By;
    bx = Ax - Cx;
    by = Ay - Cy;
    cx = Bx - Ax;
    cy = By - Ay;
    apx = Px - Ax;
    apy = Py - Ay;
    bpx = Px - Bx;
    bpy = Py - By;
    cpx = Px - Cx;
    cpy = Py - Cy;

    aCROSSbp = ax * bpy - ay * bpx;
    cCROSSap = cx * apy - cy * apx;
    bCROSScp = bx * cpy - by * cpx;

    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

bool Triangulate::Snip(const Vector2dVector &contour, int u, int v, int w,
                       int n, int *V)
{
    int i;
    float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

    Ax = contour[V[u]].x;
    Ay = contour[V[u]].y;

    Bx = contour[V[v]].x;
    By = contour[V[v]].y;

    Cx = contour[V[w]].x;
    Cy = contour[V[w]].y;

    // 判断是否是ccw方向三角形，如果不是表明是凹三角形，需要剔除
    if (EPSILON > (((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax))))
    {
        // LOG_I("un ccw wind triangle: %f,%f,%f,%f,%f,%f, (u=%d,v=%d,w=%d)", Ax, Ay, Bx, By, Cx, Cy, u,v,w);
        return false;
    }

    // return true;
    for (i = 0; i < n; i++)
    {
        if ((i == u) || (i == v) || (i == w)) continue;
        // 忽略接近值
        Px = contour[V[i]].x;
        Py = contour[V[i]].y;
        // 避免拆分算法过程中 拆分太细的重复点
        if (((Px - Ax) < EPSILON && (Py - Ay) < EPSILON) ||
            ((Px - Bx) < EPSILON && (Py - By) < EPSILON) ||
                ((Px - Cx) < EPSILON && (Py - Cy) < EPSILON)) {
            continue;
        }

        // mock
//        if (InsideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py)) {
//            return false;
//        }
    }

   return true;
}


/**
 * 只支持凸多边形
 * @param contour 输入数据数据
 * @param result 输出数据
 * @return 返回结果
 */
bool Triangulate::Process(const Vector2dVector &contour, Vector2dVector &result)
{
    /* allocate and initialize list of Vertices in polygon */

    int n = (int)contour.size();
    if (n < 3) return false;

    int *V = new int[n];

    /* we want a counter-clockwise polygon in V */

    if (0.0f < Area(contour))
        for (int v = 0; v < n; v++) V[v] = v;
    else
        for (int v = 0; v < n; v++) V[v] = (n - 1) - v;

    int nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2 * nv; /* error detection */

    for (int m = 0, v = nv - 1; nv > 2;)
    {
        /* if we loop, it is probably a non-simple polygon */
        if (0 >= (count--))
        {
            //** Triangulate: ERROR - probable bad polygon!
            return false;
        }

        /* three consecutive vertices in current polygon, <u,v,w> */
        int u = v;
        if (nv <= u) u = 0; /* previous */
        v = u + 1;
        if (nv <= v) v = 0; /* new v    */
        int w = v + 1;
        if (nv <= w) w = 0; /* next     */

        if (Snip(contour, u, v, w, nv, V))
        {
            int a, b, c, s, t;

            /* true names of the vertices */
            a = V[u];
            b = V[v];
            c = V[w];

            /* output Triangle */
            result.push_back(contour[a]);
            result.push_back(contour[b]);
            result.push_back(contour[c]);

            m++;

            /* remove v from remaining polygon */
            for (s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t];
            nv--;

            /* resest error detection counter */
            count = 2 * nv;
        }
    }

    delete[] V;

    return true;
}
