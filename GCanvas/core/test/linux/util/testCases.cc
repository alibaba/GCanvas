#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp"

void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases){
    
   testCases["fillRect"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height){
          ctx->SetFillStyle("#ff00ff");
          ctx->FillRect(0, 0, width, height);
   };
   testCases["drawText"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height){
          ctx->SetFont("20px Georgia");
          ctx->DrawText("你好123 ",10,20);
   };

   testCases["drawText"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height){};
}