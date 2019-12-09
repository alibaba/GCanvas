#include <unordered_map>
#include <functional>
#include <string>
#include <math.h>
#include "GCanvas.hpp"
void prepareCases(std::unordered_map<std::string, std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *mCanvasContext, int width, int height)>> &testCases)
{
    testCases["tc_2d_arc"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000 ");
        ctx->FillRect(0, 0, width, height);
        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->Arc(50 * ratio, 75 * ratio, 50 * ratio, 0, 0.5 * M_PI);
        ctx->Stroke();
    };
    testCases["tc_2d_arcTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000 ");
        ctx->FillRect(0, 0, width, height);
        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->MoveTo(20 * ratio, 20 * ratio);                                      // 创建开始点
        ctx->LineTo(100 * ratio, 20 * ratio);                                     // 创建水平线
        ctx->ArcTo(150 * ratio, 20 * ratio, 150 * ratio, 70 * ratio, 50 * ratio); // 创建弧
        ctx->LineTo(150 * ratio, 120 * ratio);                                    // 创建垂直线
        ctx->Stroke();
    };
    testCases["tc_2d_MDN_arc"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        for (int i = 0; i <= 3; i++)
        {
            for (int j = 0; j <= 2; j++)
            {
                ctx->BeginPath();
                int x = 25 + j * 50 * ratio;          // x coordinate
                int y = 25 + i * 50 * ratio;          // y coordinate
                int radius = 20 * ratio;              // Arc radius
                int startAngle = 0;                   // Starting point on circle
                int endAngle = M_PI + (M_PI * j) / 2; // End point on circle
                int anticlockwise = i % 2 == 1;       // Draw anticlockwise

                ctx->Arc(x, y, radius, startAngle, endAngle, anticlockwise);

                if (i > 1)
                {
                    ctx->Fill();
                }
                else
                {
                    ctx->Stroke();
                }
            }
        }
    };
    testCases["tc_2d_MDN_arcTo_1"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        // Tangential lines
        ctx->BeginPath();
        ctx->SetStrokeStyle("gray");
        ctx->MoveTo(200 * ratio, 20 * ratio);
        ctx->LineTo(200 * ratio, 130 * ratio);
        ctx->LineTo(50 * ratio, 20 * ratio);
        ctx->Stroke();

        // Arc
        ctx->BeginPath();
        ctx->SetStrokeStyle("black");
        ctx->SetLineWidth(5 * ratio);
        ctx->MoveTo(200 * ratio, 20 * ratio);
        ctx->ArcTo(200 * ratio, 130 * ratio, 50 * ratio, 20 * ratio, 40 * ratio);
        ctx->Stroke();

        // Start point
        ctx->BeginPath();
        ctx->SetFillStyle("blue");
        ctx->Arc(200 * ratio, 20 * ratio, 5 * ratio, 0, 2 * M_PI);
        ctx->Fill();

        // Control points
        ctx->BeginPath();
        ctx->SetFillStyle("red");
        ctx->Arc(200 * ratio, 130 * ratio, 5 * ratio, 0, 2 * M_PI); // Control point one
        ctx->Arc(50 * ratio, 20 * ratio, 5 * ratio, 0, 2 * M_PI);   // Control point two
        ctx->Fill();
    };
    testCases["tc_2d_MDN_arcTo_2"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(180 * ratio, 90 * ratio);
        ctx->ArcTo(180 * ratio, 130 * ratio, 110 * ratio, 130 * ratio, 130 * ratio);
        ctx->LineTo(110 * ratio, 130 * ratio);
        ctx->Stroke();
    };
    testCases["td_2d_clip"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);
        ctx->SetFillStyle("#ff0000");
        ctx->SetStrokeStyle("#fff");

        ctx->BeginPath();
        ctx->Arc(150 * ratio, 150 * ratio, 75 * ratio, 0 * ratio, 3.14 * 2, false);
        ctx->Clip();

        ctx->FillRect(0, 0, 200 * ratio, 200 * ratio);
        ctx->SetFillStyle("#00ff00");
        ctx->FillRect(180 * ratio, 180 * ratio, 200 * ratio, 200 * ratio);
    };

    testCases["tc_2d_clearRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->ClearRect(0, 0, width, height);
    };
    testCases["tc_2d_fill"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);

        ctx->SetFillStyle("#FFF");
        ctx->SetLineWidth(2);
        ctx->BeginPath();
        ctx->MoveTo(10 * ratio, 24 * ratio);
        ctx->LineTo(100 * ratio, 200 * ratio);
        ctx->LineTo(50 * ratio, 34 * ratio);
        ctx->Fill();
    };
    testCases["tc_2d_fillRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#ff0000");
        ctx->FillRect(0, 0, 100 * ratio, 100 * ratio);
    };
    testCases["tc_2d_bezierCurveTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(20 * ratio, 20 * ratio);
        ctx->BezierCurveTo(20 * ratio, 100 * ratio, 200 * ratio, 100 * ratio, 200 * ratio, 20 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_text_fillText"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#FF0000 ");
        ctx->SetFont("30px Arial");
        ctx->DrawText("w3school.com.cn", 10 * ratio, 50 * ratio);
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
    testCases["tc_2d_lineGap_round"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->SetLineWidth(10 * ratio);
        ctx->SetLineCap("round");
        ctx->MoveTo(10 * ratio, 50 * ratio);
        ctx->LineTo(100 * ratio, 50 * ratio);
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
    testCases["tc_2d_lineTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(0, 0);
        ctx->LineTo(150 * ratio, 150 * ratio);
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
    testCases["tc_2d_quadraticCurveTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->QuadraticCurveTo(20 * ratio, 100 * ratio, 200 * ratio, 50 * ratio);
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
    testCases["tc_2d_closePath"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetFillStyle("#000000 ");
        ctx->FillRect(0, 0, width, height);

        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->MoveTo(10 * ratio, 10 * ratio);
        ctx->LineTo(100 * ratio, 100 * ratio);
        ctx->LineTo(70 * ratio, 200 * ratio);
        ctx->ClosePath();
        ctx->Stroke();
    };
    testCases["tc_2d_lineTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->MoveTo(90 * ratio, 130 * ratio);
        ctx->LineTo(95 * ratio, 25 * ratio);
        ctx->LineTo(150 * ratio, 80 * ratio);
        ctx->LineTo(205 * ratio, 25 * ratio);
        ctx->LineTo(210 * ratio, 130 * ratio);
        ctx->SetLineWidth(15 * ratio);
        ctx->Stroke();
    };
    testCases["tc_2d_moveTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->BeginPath();
        ctx->MoveTo(50 * ratio, 50 * ratio);
        ctx->LineTo(50 * ratio, 150 * ratio);
        ctx->LineTo(150 * ratio, 150 * ratio);
        ctx->Stroke();
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
    testCases["tc_2d_shaow_blur"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("blue");
        ctx->SetShadowBlur(20 * ratio);
        ctx->SetShadowColor("black");
        ctx->SetShadowOffsetX(10 * ratio);
        ctx->SetShadowOffsetY(10 * ratio);
        ctx->FillRect(40 * ratio, 100 * ratio, 200 * ratio, 200 * ratio);
    };
    testCases["tc_2d_stroke_alpha"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetStrokeStyle("rgba(255,0,0,0.5) ");
        ctx->SetLineWidth(5 * ratio);
        ctx->StrokeRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };
    testCases["tc_2d_stroke"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetFillStyle("#000000 ");
        ctx->FillRect(0, 0, width, height);

        ctx->SetStrokeStyle("rgba(255,0,0,1.0) ");
        ctx->SetLineWidth(5 * ratio);
        ctx->Rect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
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
        ctx->SetFillStyle("red");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);

        // Reset current transformation matrix to the identity matrix
        ctx->SetTransform(1, 0, 0, 1, 0, 0);

        // Non-Scaled rectangle
        ctx->SetFillStyle("gray ");
        ctx->FillRect(10 * ratio, 10 * ratio, 8 * ratio, 20 * ratio);
    };
    testCases["tc_2d_MDN_transform"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Transfrom(1, .2, .8, 1, 0, 0);
        ctx->FillRect(0, 0, 100 * ratio, 100 * ratio);
    };

    // testCases["tc_2d_get_put_ImageData"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    //     int ratio = 1;
    //     ctx->SetFillStyle("#000000");
    //     ctx->rect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    //     ctx->Fill();

    //     let imageData = ctx->getImageData(60 * ratio, 60 * ratio, 200 * ratio, 100 * ratio);
    //     ctx->putImageData(imageData, 150 * ratio, 10 * ratio);

    // ctx->SetFillStyle( = "red";
    // ctx->FillRect(50*ratio, 50*ratio, 50*ratio, 50*ratio);

    // let imgData = ctx->getImageData(50*ratio, 50*ratio, 50*ratio, 50*ratio);
    // ctx->putImageData(imgData, 50*ratio, 150*ratio);
    // };

    testCases["tc_2d_rotate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Rotate(20 * M_PI / 180);
        ctx->SetFillStyle("yellow");
        ctx->FillRect(50 * ratio, 20 * ratio, 100 * ratio, 50 * ratio);
    };
    testCases["tc_2d_save"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000 ");
        ctx->Save();
        ctx->SetFillStyle("red ");
        ctx->FillRect(10 * ratio, 10 * ratio, 150 * ratio, 100 * ratio);
        ctx->Restore();
        ctx->FillRect(50 * ratio, 50 * ratio, 150 * ratio, 100 * ratio);
    };
    testCases["tc_2d_scale"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->StrokeRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
        ctx->Scale(2, 2);
        ctx->StrokeRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
    };
    testCases["tc_2d_resetTransform"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->Rotate(45 * M_PI / 180);
        ctx->FillRect(70 * ratio, 0, 100 * ratio, 30 * ratio);
        ctx->ResetTransform();
        ctx->SetFillStyle("red");
        ctx->FillRect(70 * ratio, 0, 100 * ratio, 30 * ratio);
    };
    testCases["tc_2d_transform"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("yellow");
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
        ctx->Transfrom(1, 0.5, -0.5, 1, 30 * ratio, 10 * ratio);
        ctx->SetFillStyle("red");
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
        ctx->Transfrom(1, 0.5, -0.5, 1, 30 * ratio, 10 * ratio);
        ctx->SetFillStyle("blue");
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
    };
    testCases["tc_2d_translate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
        ctx->Translate(100 * ratio, 100 * ratio);
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 50 * ratio);
    };

    testCases["tc_2d_text_fillText_alpha"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("rgba(255,0,0,0.5) ");
        ctx->SetFont("30px Arial");
        ctx->DrawText("helloworld", 10 * ratio, 50 * ratio);
    };

    testCases["tc_2d_text_measureText"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->SetFont("30px Arial");
        const char *txt = "Hello World";
        ctx->DrawText(txt, 10 * ratio, 50 * ratio);
        float textWidth = ctx->MeasureTextWidth(txt);
        // ctx->FillText("width:" + textWidth.toFixed(1), 10 * ratio, 100 * ratio);

        ctx->SetFillStyle("#ff0000");
        ctx->FillRect(10 * ratio, 60 * ratio, textWidth, 5 * ratio);
    };

    testCases["tc_2d_text_strokeText"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000 ");
        ctx->SetFont("40px Arial");
        ctx->StrokeText("helloworld", 10 * ratio, 50 * ratio);
    };

    testCases["tc_2d_text_textAlign"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000 ");
        ctx->SetStrokeStyle("blue");
        ctx->MoveTo(125 * ratio, 10 * ratio);
        ctx->LineTo(125 * ratio, 300 * ratio);
        ctx->Stroke();
        ctx->SetFont("15px Arial-MT");
        ctx->SetTextAlign(TEXT_ALIGN_START);
        ctx->DrawText("textAlign=start", 125 * ratio, 10 * ratio);
        ctx->SetTextAlign(TEXT_ALIGN_END);
        ctx->SetFont("15px");
        ctx->DrawText("textAlign=end", 125 * ratio, 50 * ratio);
        ctx->SetTextAlign(TEXT_ALIGN_LEFT);
        ctx->DrawText("textAlign=left", 125 * ratio, 100 * ratio);
        ctx->SetTextAlign(TEXT_ALIGN_CENTER);
        ctx->DrawText("textAlign=center", 125 * ratio, 150 * ratio);
        ctx->SetTextAlign(TEXT_ALIGN_RIGHT);
        ctx->DrawText("textAlign=right", 125 * ratio, 200 * ratio);
    };

    testCases["compite_desover"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("red");
        ctx->FillRect(20 * ratio, 140 * ratio, 75 * ratio, 50 * ratio);
        ctx->DoSetGlobalCompositeOperation(COMPOSITE_OP_DESTINATION_OVER);
        ctx->SetFillStyle("blue");
        ctx->FillRect(50 * ratio, 170 * ratio, 75 * ratio, 50 * ratio);
    };

    testCases["composite_sourceover"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("red");
        ctx->FillRect(20 * ratio, 140 * ratio, 75 * ratio, 50 * ratio);
        ctx->DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER);
        ctx->SetFillStyle("blue");
        ctx->FillRect(50 * ratio, 170 * ratio, 75 * ratio, 50 * ratio);
    };

    testCases["tc_2d_lineargradient"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);
        float startArr[] = {10.0, 0};
        float endArr[] = {100.0, 0};
        float posArray[] = {0, 1};
        std::string colorArray[] = {"white", "red"};
        ctx->SetFillStyleLinearGradient(startArr, endArr, 2, posArray, colorArray, false);
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };

    testCases["tc_2d_radialgradient"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->FillRect(0, 0, width, height);
        float startArr[] = {75, 50, 5};
        float endArr[] = {90, 60, 100};
        float posArray[] = {0, 1};
        std::string colorArray[] = {"red", "white"};
        ctx->SetFillStyleRadialGradient(startArr, endArr, 2,
                                        posArray,
                                        colorArray,
                                        false);
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
    };

    testCases["tc_2d_globalAlpha"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("red");
        ctx->FillRect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
        ctx->SetGlobalAlpha(0.2);
        ctx->SetFillStyle("green");
        ctx->FillRect(50 * ratio, 50 * ratio, 100 * ratio, 100 * ratio);
        ctx->SetFillStyle("blue");
        ctx->FillRect(100 * ratio, 100 * ratio, 100 * ratio, 100 * ratio);
    };

    testCases["tc_2d_line_dash"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#ff0000");

        std::vector<float> dashes{10, 5};
        ctx->SetLineDash(dashes);
        ctx->SetLineWidth(5 * ratio);
        ctx->SetLineCap("butt");
        ctx->BeginPath();
        ctx->MoveTo(30 * ratio, 100 * ratio);
        ctx->LineTo(500 * ratio, 100 * ratio);
        ctx->Stroke();

        std::vector<float> dashes2 = ctx->GetLineDash();
        dashes2.push_back(30);
        ctx->SetLineDash(dashes2);

        ctx->BeginPath();
        ctx->MoveTo(30 * ratio, 200 * ratio);
        ctx->LineTo(500 * ratio, 200 * ratio);
        ctx->Stroke();
    };

    testCases["tc_2d_get_put_ImageData"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->Rect(10 * ratio, 10 * ratio, 100 * ratio, 100 * ratio);
        ctx->Fill();
        unsigned char *imageData = new unsigned char[200 * 100 * 4];
        ctx->GetImageData(60 * ratio, 60 * ratio, 200 * ratio, 100 * ratio, imageData);
        ctx->PutImageData(imageData, 200, 100, 150 * ratio, 10 * ratio, 0, 0, 200, 100);

        ctx->SetFillStyle("red");
        ctx->FillRect(50 * ratio, 50 * ratio, 50 * ratio, 50 * ratio);

        uint8_t *imgData = new uint8_t[50 * 50 * 4];
        ctx->GetImageData(50 * ratio, 50 * ratio, 50 * ratio, 50 * ratio, imgData);
        ctx->PutImageData(imgData, 50, 50, 50 * ratio, 150 * ratio, 0, 0, 50, 50);
        delete imageData;
        delete imgData;
        imageData = nullptr;
        imgData = nullptr;
    };

    testCases["tc_sample_path_arc_rotate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000");
        ctx->FillRect(0, 0, width, height);

        ctx->BeginPath();
        ctx->Arc(150, 150, 120, 0, 6.28);
        ctx->SetFillStyle("#fff");
        ctx->Fill();

        ctx->Save();
        ctx->BeginPath();
        ctx->Translate(150, 150);
        ctx->SetFillStyle("#ff0000");
        float degree = 1.0;
        ctx->Arc(0, 0, 100, 0, degree);
        ctx->Save();
        ctx->Rotate(degree);
        ctx->MoveTo(100, 0);
        ctx->LineTo(0, 0);
        ctx->Restore();
        ctx->Rotate(0);
        ctx->LineTo(100, 0);
        ctx->Fill();

        ctx->Restore();
    };
    testCases["tc_sample_path_rotate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetLineWidth(10);
        ctx->BeginPath();
        ctx->Rect(20, 20, 100, 100);
        ctx->Stroke();

        ctx->Translate(100, 100);

        ctx->Save();
        ctx->MoveTo(200, 200);
        ctx->LineTo(250, 250);

        ctx->MoveTo(250, 250);
        ctx->LineTo(200, 0);

        ctx->MoveTo(250, 250);
        //Rotate
        ctx->Rotate(M_PI * 0.5);
        ctx->LineTo(200, 0);

        ctx->Restore();

        ctx->Stroke();
    };

    testCases["tc_sample_path_scale"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetLineWidth(10);

        ctx->BeginPath();
        ctx->Translate(100, 100);

        ctx->Save();
        ctx->MoveTo(40, 40);
        ctx->LineTo(50, 50);

        //before Scale
        ctx->LineTo(40, 0);
        ctx->MoveTo(50, 50);

        //Scale
        ctx->Scale(2, 2);
        ctx->LineTo(40, 0);

        ctx->Restore();

        ctx->Stroke();
    };

    testCases["tc_sample_path_transform_fill2"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Save();
        ctx->Scale(0.3, 0.3);
        ctx->BeginPath();
        ctx->Arc(411, 411, 394, 0, 6.28);
        ctx->SetFillStyle("#fff");
        ctx->SetStrokeStyle("#d5d5d5");
        ctx->Fill();

        ctx->Stroke();
        ctx->Save();
        ctx->BeginPath();
        ctx->Translate(414, 414);
        ctx->Arc(0, 0, 374, 3.94, 4.47);
        ctx->Save();
        ctx->Rotate(4.47);
        ctx->MoveTo(374, 0);
        ctx->LineTo(0, 0);
        ctx->Restore();
        ctx->Rotate(3.94);
        ctx->LineTo(374, 0);
        ctx->SetFillStyle("#EE5461");
        ctx->Fill();
        ctx->Stroke();
        ctx->Restore();
    };

    testCases["tc_sample_path_transform_fill"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        float startAngle = M_PI;
        float endAngle = M_PI * 1.5;
        ctx->Save();
        ctx->BeginPath();
        ctx->Translate(414, 414);
        ctx->Arc(0, 0, 374, startAngle, endAngle);

        ctx->Save();
        ctx->Rotate(endAngle);
        ctx->MoveTo(374, 0);
        ctx->LineTo(0, 0);

        ctx->Restore();
        ctx->Stroke();

        ctx->Restore();
    };

    testCases["tc_sample_path_translate"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetLineWidth(10);

        ctx->BeginPath();
        ctx->Rect(20, 20, 100, 100);
        ctx->Stroke();

        ctx->Translate(100, 100);
        ctx->Save();
        ctx->MoveTo(200, 200);
        ctx->LineTo(250, 250);

        ctx->LineTo(200, 0);

        //Translate1
        ctx->Translate(100, 0);
        ctx->LineTo(200, 0);

        ctx->Restore();
        ctx->Stroke();
    };

    testCases["tc_sample_path_transform"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->SetLineWidth(10);

        ctx->BeginPath();
        ctx->Translate(100, 100);

        ctx->Save();
        ctx->MoveTo(200, 200);
        ctx->LineTo(250, 250);

        //Rotate
        ctx->SetTransform(1, 0.5, -0.5, 1, 30 * ratio, 10 * ratio);
        ctx->LineTo(200, 0);

        ctx->Restore();

        ctx->Stroke();
    };

    testCases["tc_2d_arc_circle"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetLineWidth(10);

        ctx->SetFillStyle("#000000 ");
        ctx->FillRect(0, 0, width, height);

        ctx->BeginPath();
        ctx->SetStrokeStyle("yellow");
        ctx->Arc(75 * ratio, 75 * ratio, 50 * ratio, 0, 2 * M_PI);
        ctx->Stroke();
    };

    testCases["tc_2d_resetClip"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Save();
        ctx->Rect(10 * ratio, 10 * ratio, 100 * ratio, 120 * ratio);
        ctx->Stroke();
        ctx->Clip();
        ctx->SetFillStyle("green");
        ctx->FillRect(0, 0, 150 * ratio, 100 * ratio);
        ctx->Restore();

        ctx->ResetClip();
        ctx->SetFillStyle("green");
        ctx->FillRect(200 * ratio, 0, 150 * ratio, 100 * ratio);
    };

    testCases["tc_2d_text_textBaseLine"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->SetFillStyle("#000000");
        ctx->SetStrokeStyle("blue");
        ctx->MoveTo(10 * ratio, 125 * ratio);
        ctx->LineTo(250 * ratio, 125 * ratio);
        ctx->Stroke();
        ctx->SetFont("20px Arial");
        ctx->SetTextBaseline(TEXT_BASELINE_TOP);
        ctx->DrawText("Top", 10 * ratio, 125 * ratio);
        ctx->SetTextBaseline(TEXT_BASELINE_BOTTOM);
        ctx->DrawText("Bottom", 50 * ratio, 125 * ratio);
        ctx->SetTextBaseline(TEXT_BASELINE_MIDDLE);
        ctx->DrawText("Middle", 100 * ratio, 125 * ratio);
        ctx->SetTextBaseline(TEXT_BASELINE_ALPHABETIC);
        ctx->DrawText("Alphabetic", 150 * ratio, 125 * ratio);
        ctx->SetTextBaseline(TEXT_BASELINE_HANGING);
        ctx->DrawText("Hanging", 200 * ratio, 125 * ratio);
    };

    testCases["tc_2d_shadow_alpha"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Translate(100, 100);

        ctx->SetFillStyle("rgba(0, 0, 255, 0.5)");
        ctx->SetShadowColor("rgba(0, 0, 0, 0.2)");
        ctx->SetShadowBlur(20);
        ctx->SetShadowOffsetX(20);
        ctx->SetShadowOffsetY(20);
        ctx->FillRect(10, 10, 100, 100);
    };

    testCases["tc_2d_shadow_fill"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;

        ctx->Translate(100, 100);

        ctx->SetFillStyle("rgba(0,0,255, 0.5)");
        ctx->SetShadowColor("black");

        ctx->SetShadowBlur(15);
        ctx->SetShadowOffsetX(20);
        ctx->SetShadowOffsetY(20);

        ctx->BeginPath();
        ctx->MoveTo(50, 50);
        ctx->LineTo(100, 200);
        ctx->LineTo(100, 50);
        ctx->ClosePath();

        //ctx->Rect(50, 50, 50, 150);
        ctx->Fill();
    };

    testCases["tc_2d_shadow_fillrect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Translate(100, 100);
        ctx->SetFillStyle("blue");
        ctx->SetShadowColor("black");
        ctx->SetShadowBlur(15);
        ctx->SetShadowOffsetX(20);
        ctx->SetShadowOffsetY(20);
        ctx->FillRect(10, 10, 100, 100);
    };

    testCases["tc_2d_shadow_stroke"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Translate(100, 100);

        ctx->SetStrokeStyle("red");
        ctx->SetShadowColor("black");

        ctx->SetShadowBlur(15);
        ctx->SetShadowOffsetX(20);
        ctx->SetShadowOffsetY(20);

        ctx->BeginPath();
        ctx->MoveTo(50, 50);
        ctx->LineTo(100, 200);
        ctx->LineTo(100, 50);
        ctx->ClosePath();

        ctx->Stroke();
    };

    testCases["tc_2d_shadow_strokerect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
        int ratio = 1;
        ctx->Translate(100, 100);
        ctx->SetStrokeStyle("red");
        ctx->SetShadowColor("black");
        ctx->SetShadowBlur(15);
        ctx->SetShadowOffsetX(10);
        ctx->SetShadowOffsetY(10);

        ctx->StrokeRect(10, 10, 90, 90);
    };
}