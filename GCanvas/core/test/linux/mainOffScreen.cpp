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

class CaseOne:public GBenchMarkCase{
public:
    CaseOne(std::string name):GBenchMarkCase(name){
            printf("case one \n");
    }
    void draw(std::shared_ptr<gcanvas::GCanvas> canvas,int width,int height) override  {
                if(canvas){
                    printf("run  one \n");
                    canvas->mCanvasContext->SetFillStyle("#ffffff");
                    canvas->mCanvasContext->FillRect(0, 0, width, height);
                }
    }
};

int main(int argc, char *argv[])
{
   std::shared_ptr<gcanvas::GCanvas> p(new gcanvas::GCanvas ("benchMark", {false, true}, nullptr));
   GBenchMark mbench(renderBufferWidth,renderBufferHeight,p);
   mbench.intilGLOffScreenEnviroment();
   std::shared_ptr<GBenchMarkCase> case1(new CaseOne("filRect"));
   mbench.run(case1);
   mbench.outputRenderResult2File();
    float ratio = mbench.compareWithW3CResult();
    std::cout << "ratio is  " << ratio << std::endl;
}