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

    testCases["tc_2d_arc"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);
        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->Arc(50, 75, 50, 0, 0.5 * M_PI);
        ctx->Stroke();
    };

    testCases["tc_2d_MDN_arcTo_1"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        // Tangential lines
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetStrokeStyle("gray");
        ctx->MoveTo(200 * 1, 20 * 1);
        ctx->LineTo(200 * 1, 130 * 1);
        ctx->LineTo(50 * 1, 20 * 1);
        ctx->Stroke();

        // Arc
        ctx->BeginPath();
        ctx->SetStrokeStyle("black");
        ctx->SetLineWidth(5 * 1);
        ctx->MoveTo(200 * 1, 20 * 1);
        ctx->ArcTo(200 * 1, 130 * 1, 50 * 1, 20 * 1, 40 * 1);
        ctx->Stroke();

        // Start point
        ctx->BeginPath();
        ctx->SetFillStyle("blue");
        ctx->Arc(200 * 1, 20 * 1, 5 * 1, 0, 2 * M_PI);
        ctx->Fill();

        // Control points
        // ctx->BeginPath();
        // ctx->SetFillStyle("red");
        // ctx->Arc(200 * ratio, 130 * ratio, 5 * ratio, 0, 2 * M_PI); // Control point one
        // ctx->Arc(50 * ratio, 20 * ratio, 5 * ratio, 0, 2 * M_PI);  // Control point two
        // ctx->Fill();
    };
    testCases["tc_2d_MDN_arcTo_2"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        ctx->BeginPath();
        ctx->SetStrokeStyle("balck");
        ctx->MoveTo(180 * 1, 90 * 1);
        ctx->ArcTo(180 * 1, 130 * 1, 110 * 1, 130 * 1, 130 * 1);
        ctx->LineTo(110 * 1, 130 * 1);
        ctx->Stroke();
    };

    testCases["tc_2d_arcTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio=1;
        ctx->SetFillStyle( "#000000");
        ctx->FillRect(0, 0, width,height);

        ctx->BeginPath();
        ctx->SetStrokeStyle ("yellow");
        ctx->MoveTo(20 * ratio, 20 * ratio);                                      // 创建开始点
        ctx->LineTo(100 * ratio, 20 * ratio);                                     // 创建水平线
        ctx->ArcTo(150 * ratio, 20 * ratio, 150 * ratio, 70 * ratio, 50 * ratio); // 创建弧
        ctx->LineTo(150 * ratio, 120 * ratio);                                    // 创建垂直线
        ctx->Stroke();
    };
}