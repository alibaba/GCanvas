#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

#include <iostream>
#include <GCanvas.hpp>
#include <lodepng.h>

#define CONTEXT_ES20
#include "GBenchMark.h"
const static GLuint renderBufferHeight = 400;
const static GLuint renderBufferWidth = 400;

   
extern void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases);

int main(int argc, char *argv[])
{
    std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>> testCases;

    prepareCases(testCases);

   GBenchMark becnMarker(renderBufferWidth,renderBufferHeight);
   becnMarker.intilGLOffScreenEnviroment();
   for(auto it=testCases.begin();it!=testCases.end();it++){
            becnMarker.run(it->first,it->second);
   }
   becnMarker.dumpResult();
    
    
}