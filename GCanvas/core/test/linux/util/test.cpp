#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{ 
  testCases["tc_2d_translate"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { 
    int  ratio=1; 
  ctx->SetFillStyle( = '#000000';
ctx->FillRect(10*ratio, 10*ratio, 100*ratio, 50*ratio);
ctx->Translate(100*ratio, 100*ratio);
ctx->FillRect(10*ratio, 10*ratio, 100*ratio, 50*ratio);
  };
 }
