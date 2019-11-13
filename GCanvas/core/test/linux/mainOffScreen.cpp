#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>
#define CONTEXT_ES20

#include "GBenchMark.h"
const static GLuint renderBufferHeight = 300;
const static GLuint renderBufferWidth = 300;


int main(int argc, char *argv[])
{
   std::shared_ptr<gcanvas::GCanvas> p(new gcanvas::GCanvas ("benchMark", {false, true}, nullptr));
   GBenchMark mbench(renderBufferWidth,renderBufferHeight,p);
   mbench.intilGLOffScreenEnviroment();
   mbench.draw();
   mbench.outputRenderResult2File();
    float ratio = mbench.compareWithW3CResult();
    std::cout << "ratio is  " << ratio << std::endl;
}