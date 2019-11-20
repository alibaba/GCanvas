#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{ 
  testCases["tc_2d_lineTo"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { 
    int  ratio=1; 
  ctx->beginPath();
ctx->moveTo(0,0);
ctx->lineTo(150*ratio,150*ratio);
ctx->stroke();


  };
 }
