#include <unordered_map>
#include <functional>
#include <string>
#include <math.h>
#include "GCanvas.hpp"
void prepareCases(std::unordered_map<std::string, std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *mCanvasContext, int width, int height)>> &testCases)
{
    testCases["tc_2d_fillRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        ctx->SetFillStyle("#ff0000");
        ctx->FillRect(0, 0, 100, 100);
    };

    testCases["tc_2d_closePath"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);

        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(100 * ratio, 100 * ratio);
        ctx->LineTo(70 * ratio, 200 * ratio);
        ctx->ClosePath();
        ctx->Stroke();
    };

    testCases["tc_2d_beginPath"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // First path
        ctx->BeginPath();
        ctx->SetStrokeStyle("blue ");
        ctx->MoveTo(20 * ratio, 20 * ratio);
        ctx->LineTo(200 * ratio, 20 * ratio);
        ctx->Stroke();

        // Second path
        ctx->BeginPath();
        ctx->SetStrokeStyle("green ");
        ctx->MoveTo(20 * ratio, 20 * ratio);
        ctx->LineTo(120 * ratio, 120 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_strokeRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetStrokeStyle("rgba(255,0,255,1.0)");
        ctx->SetLineWidth(5 * ratio);
        ctx->StrokeRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };
}