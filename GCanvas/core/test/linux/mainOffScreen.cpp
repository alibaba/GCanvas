#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>
#include <fstream>
#define CONTEXT_ES20

#include "GBenchMark.h"
const static GLuint renderBufferHeight = 300;
const static GLuint renderBufferWidth = 300;

class CaseOne:public GBenchMarkCase{
public:
    CaseOne(std::string name):GBenchMarkCase(name){}
    void draw(std::shared_ptr<gcanvas::GCanvas> canvas,int width,int height) override  {
                if(canvas){
                  //   canvas->mCanvasContext->SetFillStyle("#ff0000");
                  //   canvas->mCanvasContext->FillRect(0, 0, width, height);
                    canvas->mCanvasContext->SetFont("20px Georgia");
                    canvas->mCanvasContext->DrawText("hello world",10,20);
                }
    }
};

   

int main(int argc, char *argv[])
{
   std::shared_ptr<gcanvas::GCanvas> p(new gcanvas::GCanvas ("benchMark", {false, true}, nullptr));
   GBenchMark becnMarker(renderBufferWidth,renderBufferHeight,p);
   becnMarker.intilGLOffScreenEnviroment();
   std::shared_ptr<GBenchMarkCase> case1(new CaseOne("fillRect"));
   becnMarker.run(case1);
    float ratio = becnMarker.computeRatioWithW3C(case1->getCaseName());
   std::cout << "------------------"<< std::endl;
   std::cout << "the case name is "<<case1->getCaseName() << std::endl;
   std::cout << "------------------" << std::endl;
   std::cout << "the correct ratio is " << ratio << std::endl;
    case1->ratio=ratio;
    
    
   std:: ofstream myfile;
   myfile.open ("result.txt");
   myfile << case1->getCaseName();
   myfile << "#";
   myfile << case1->ratio;
   myfile.close();
}