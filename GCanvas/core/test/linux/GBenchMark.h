#define CONTEXT_ES20
#ifndef GBENCHMARK_H
#define GBENCHMARK_H
#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>


class GBenchMark{
 public:
         GBenchMark():mWidth(0),mHeight(0){

         }
        GBenchMark(int width,int height);
         void intilGLOffScreenEnviroment();
         void outputRenderResult2File();
         float compareWithW3CResult();
private:
        // gcanvas::GCanvas c;
        int mHeight;
        int mWidth;
};

#endif