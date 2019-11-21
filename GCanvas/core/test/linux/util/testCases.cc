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

    testCases["tc_2d_MDN_scale"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // Scaled rectangle
        ctx->Scale(9, 3);
        ctx->SetFillStyle("red ");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);

        // Reset current transformation matrix to the identity matrix
        ctx->SetTransform(1, 0, 0, 1, 0, 0);

        // Non-Scaled rectangle
        ctx->SetFillStyle("gray ");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);
    };

    testCases["tc_2d_MDN_scale"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // Scaled rectangle
        ctx->Scale(9, 3);
        ctx->SetFillStyle("red ");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);

        // Reset current transformation matrix to the identity matrix
        ctx->SetTransform(1, 0, 0, 1, 0, 0);

        // Non-Scaled rectangle
        ctx->SetFillStyle("gray ");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);
    };

    testCases["tc_2d_rotate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Rotate(20 * M_PI / 180);
        ctx->SetFillStyle("yellow ");
        ctx->FillRect(50 * ratio, 20 * ratio, 100 * ratio, 50 * ratio);
    };

    testCases["tc_2d_lineGap_butt"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineCap("butt ");
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(100 * ratio, 10 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_lineJoin_round"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineJoin("round");
        ctx->SetMiterLimit(5 * ratio);
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(200 * ratio, 100 * ratio);
        ctx->LineTo(50 * ratio, 200 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_clearRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->ClearRect(0, 0, width, height);
    };

    testCases["tc_2d_fill"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);

        ctx->SetFillStyle("#FFFFFF");
        ctx->SetLineWidth(2);
        ctx->BeginPath();
        ctx->MoveTo(10 * ratio, 24 * ratio);
        ctx->LineTo(100 * ratio, 200 * ratio);
        ctx->LineTo(50 * ratio, 34 * ratio);
        ctx->Fill();
    };

    testCases["tc_2d_stroke_alpha"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetStrokeStyle("rgba(255,0,0,0.5) ");
        ctx->SetLineWidth(5 * ratio);
        ctx->StrokeRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };

    testCases["tc_2d_shadow"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("blue");
        ctx->SetShadowColor("black");
        ctx->SetShadowBlur(20 * ratio);
        ctx->SetShadowOffsetX(20 * ratio);
        ctx->SetShadowOffsetY(20 * ratio);
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };
    testCases["tc_2d_shadowOffsetY"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // Shadow
        ctx->SetShadowColor("red ");
        ctx->SetShadowOffsetY(25 * ratio);
        ctx->SetShadowBlur(10 * ratio);

        // Rectangle
        ctx->SetFillStyle("blue ");
        ctx->FillRect(20 * ratio, 20 * ratio, 150 * ratio, 80 * ratio);
    };
    testCases["tc_2d_shadowOffsetX"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // Shadow
        ctx->SetShadowColor("red ");
        ctx->SetShadowOffsetX(25 * ratio);
        ctx->SetShadowBlur(10 * ratio);

        // Rectangle
        ctx->SetFillStyle("blue ");
        ctx->FillRect(20 * ratio, 20 * ratio, 150 * ratio, 100 * ratio);
    };
    testCases["tc_2d_shaow_blur"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("blue");
        ctx->SetShadowBlur(20 * ratio);
        ctx->SetShadowColor("black");
        ctx->SetShadowOffsetX(10 * ratio);
        ctx->SetShadowOffsetY(10 * ratio);
        ctx->FillRect(40 * ratio, 100 * ratio, 200 * ratio, 200 * ratio);
    };

    testCases["tc_2d_moveTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(50 * ratio, 50 * ratio);
        ctx->LineTo(50 * ratio, 150 * ratio);
        ctx->LineTo(150 * ratio, 150 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_lineGap_square"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineCap("square ");
        ctx->MoveTo(10 * ratio, 100 * ratio);
        ctx->LineTo(100 * ratio, 100 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_lineJoin_bevel"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineJoin("bevel ");
        ctx->SetMiterLimit(5);
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(200 * ratio, 100 * ratio);
        ctx->LineTo(50 * ratio, 200 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_MDN_lineTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->MoveTo(90 * ratio, 130 * ratio);
        ctx->LineTo(95 * ratio, 25 * ratio);
        ctx->LineTo(150 * ratio, 80 * ratio);
        ctx->LineTo(205 * ratio, 25 * ratio);
        ctx->LineTo(210 * ratio, 130 * ratio);
        ctx->SetLineWidth(15 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_lineGap_butt"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineCap("butt ");
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(100 * ratio, 10 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_lineJoin_miter"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineJoin("miter ");
        ctx->SetMiterLimit(5 * ratio);
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(200 * ratio, 100 * ratio);
        ctx->LineTo(50 * ratio, 200 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_quadraticCurveTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->QuadraticCurveTo(20 * ratio, 100 * ratio, 200 * ratio, 50 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_lineGap_round"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineCap("round");
        ctx->MoveTo(10 * ratio, 50 * ratio);
        ctx->LineTo(100 * ratio, 50 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_lineJoin_round"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineJoin("round ");
        ctx->SetMiterLimit(5 * ratio);
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(200 * ratio, 100 * ratio);
        ctx->LineTo(50 * ratio, 200 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_bezierCurveTo"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { 
    int  ratio=1; 
  ctx->BeginPath();
ctx->MoveTo(20*ratio , 20*ratio );
ctx->BezierCurveTo(20*ratio , 100*ratio , 200*ratio , 100*ratio , 200 *ratio, 20*ratio);
ctx->Stroke();
  
 };
}