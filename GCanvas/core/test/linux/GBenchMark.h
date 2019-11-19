#define CONTEXT_ES20
#ifndef GBENCHMARK_H
#define GBENCHMARK_H
#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>
#include <functional>

extern void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height);
extern void decodeFile2Pixels(std::string  filename, std::vector<unsigned char> &image);
class GBenchMark
{
public:
    GBenchMark() : mWidth(0), mHeight(0), mCanvas(nullptr)
    {
    }
    GBenchMark(int width, int height);
    virtual ~GBenchMark() {}
    void intilGLOffScreenEnviroment();

    float computeRatioWithW3C(std::string caseName);
    void run(std::string caseName, std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,int width,int height)> drawFunc);
private:
    std::shared_ptr<gcanvas::GCanvas> mCanvas;
    void initGcanvas();
    void render2file(std::string caseName);
    int mHeight;
    int mWidth;
    std::string w3cPrefix="../../w3c/build/";
};

#endif