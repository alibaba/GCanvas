#define CONTEXT_ES20
#ifndef GBACKEND_H
#define GBACKEND_H
#include <iostream>
#include <GCanvas.hpp>
#include "lodepng.h"
#include <functional>
#include <unordered_map>
#include "GConvert.h"
#include "NodeBindingUtil.h"
#include "Util.h"

namespace NodeBinding
{
extern void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height);
extern void decodeFile2Pixels(std::string filename, std::vector<unsigned char> &image);
class GRenderContext
{
public:
    GRenderContext() : mWidth(0), mHeight(0), mCanvas(nullptr)
    {
    }
    GRenderContext(int width, int height);
    virtual ~GRenderContext() {}
    void initRenderEnviroment();
    void render2file(std::string caseName);
    void drawFrame();
    GCanvasContext *getCtx() { return mCanvas->GetGCanvasContext(); }
    int inline getWdith() { return this->mWidth; }
    int inline getHeight() { return this->mHeight; }

private:
    std::shared_ptr<gcanvas::GCanvas> mCanvas;
    void initCanvas();
    int mHeight;
    int mWidth;
    int drawCount = 0;
};
} // namespace NodeBinding

#endif