#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{ 
  testCases["tc_2d_fill"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { 
    int  ratio=1; 
  
ctx->SetFillStyle( = "#000000";
ctx->FillRect(0, 0, width, height);

    ctx->SetFillStyle( = "#FFF";
    ctx->SetLineWidth( = 2;
    ctx->BeginPath();
    ctx->MoveTo(10*ratio, 24*ratio);
    ctx->LintTo(100*ratio, 200*ratio);
    ctx->LintTo(50*ratio, 34*ratio);
    ctx->Fill();
  };
 }
