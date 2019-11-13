#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <EGL/egl.h>
#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>
#define CONTEXT_ES20

#include "GBenchMark.h"
const static GLuint renderBufferHeight = 300;
const static GLuint renderBufferWidth = 300;

extern void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height);
extern void decodeFile2Pixels(const char *filename, std::vector<unsigned char> &image);


void intilGLOffScreenEnviroment()
{

}

void createGcanvas(gcanvas::GCanvas &c)
{
    printf("run the gcanvas in the offscreen \n");
    c.CreateContext();
    c.OnSurfaceChanged(0, 0, renderBufferWidth, renderBufferHeight);
    c.mCanvasContext->SetFillStyle("#ff0000");
    c.mCanvasContext->FillRect(0, 0, renderBufferWidth, renderBufferHeight);
    c.drawFrame();
}

void outputRenderResult2File()
{
    int size = 4 * renderBufferHeight * renderBufferWidth;
    unsigned char *data = new unsigned char[size];
    glReadPixels(0, 0, renderBufferWidth, renderBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
    encodePixelsToFile("a.png", data, renderBufferWidth, renderBufferHeight);
    delete data;
}

float compareWithW3CResult()
{
    std::vector<unsigned char> standrandImage;
    std::vector<unsigned char> gcanvasImage;
    decodeFile2Pixels("../../w3c/build/mycanvasPage.png", standrandImage);
    decodeFile2Pixels("a.png", gcanvasImage);
    int N = std::min(standrandImage.size(), gcanvasImage.size());
    int errorCount = 0;
    int rightCount = 0;
    for (int i = 0; i < N; i++)
    {
        if (standrandImage[i] != gcanvasImage[i])
            errorCount++;
        else
            rightCount++;
    }
    return 1.0f * rightCount / N;
}

int main(int argc, char *argv[])
{
    // intilGLOffScreenEnviroment();
   GBenchMark mbench(renderBufferWidth,renderBufferHeight);
   mbench.intilGLOffScreenEnviroment();
    gcanvas::GCanvas c("benchMark", {false, true}, nullptr);

    createGcanvas(c);
    outputRenderResult2File();
    float ratio = compareWithW3CResult();
    std::cout << "ratio is  " << ratio << std::endl;
}