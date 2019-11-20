#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp"
void prepareCases(std::unordered_map<std::string, std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *mCanvasContext, int width, int height)>> &testCases)
{
    testCases['tc_2d_fill'] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        ctx->fillStyle = "#FFF";
        ctx->lineWidth = 2;
        ctx->beginPath();
        ctx->moveTo(10, 24);
        ctx->lineTo(100, 200);
        ctx->lineTo(50, 34);
        ctx->fill();
    };
    testCases['tc_2d_fillRect'] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        ctx->fillStyle = "#ff0000";
        ctx->fillRect(0, 0, 100, 100);
    };
}