#define CONTEXT_ES20
#ifndef GBENCHMARK_H
#define GBENCHMARK_H
#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>
#include "GBenchMarkCase.h"

extern void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height);
extern void decodeFile2Pixels(const char *filename, std::vector<unsigned char> &image);
class GBenchMark
{
public:
    GBenchMark() : mWidth(0), mHeight(0), mCanvas(nullptr)
    {
    }
    GBenchMark(int width, int height, std::shared_ptr<gcanvas::GCanvas> canvas);
    virtual ~GBenchMark() {}
    void intilGLOffScreenEnviroment();
    void outputRenderResult2File();
    float compareWithW3CResult();
    void run(std::shared_ptr<GBenchMarkCase> oneCase);
    void draw();
private:
    std::shared_ptr<gcanvas::GCanvas> mCanvas;
    void initGcanvas();
    int mHeight;
    int mWidth;
};

#endif