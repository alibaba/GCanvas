#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp"
void prepareCases(std::unordered_map<std::string, std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *mCanvasContext, int width, int height)>> &testCases)
{
  testCases["tc_2d_fillRect"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    ctx->fillStyle = "#ff0000";
    ctx->fillRect(0, 0, 100, 100);
  };
  testCases["tc_2d_arc"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    ctx->fillStyle = '#000000';
    ctx->fillRect(0, 0, c.width, c.height);

    console.log("ratio is ======>" + ratio);

    ctx->beginPath();
    ctx->strokeStyle = "yellow";
    ctx->arc(50 * ratio, 75 * ratio, 50 * ratio, 0, 0.5 * Math.PI);
    ctx->stroke();
  };
  testCases["tc_2d_arcTo"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    ctx->fillStyle = '#000000';
    ctx->fillRect(0, 0, c.width, c.height);

    ctx->beginPath();
    ctx->strokeStyle = "yellow";
    ctx->moveTo(20 * ratio, 20 * ratio);                                      // 创建开始点
    ctx->lineTo(100 * ratio, 20 * ratio);                                     // 创建水平线
    ctx->arcTo(150 * ratio, 20 * ratio, 150 * ratio, 70 * ratio, 50 * ratio); // 创建弧
    ctx->lineTo(150 * ratio, 120 * ratio);                                    // 创建垂直线
    ctx->stroke();
  };
  testCases["tc_2d_MDN_arc"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    for (let i = 0; i <= 3; i++)
    {
      for (let j = 0; j <= 2; j++)
      {
        ctx->beginPath();
        let x = 25 + j * 50 * 1;                    // x coordinate
        let y = 25 + i * 50 * 1;                    // y coordinate
        let radius = 20 * 1;                        // Arc radius
        let startAngle = 0;                         // Starting point on circle
        let endAngle = Math.PI + (Math.PI * j) / 2; // End point on circle
        let anticlockwise = i % 2 == 1;             // Draw anticlockwise

        ctx->arc(x, y, radius, startAngle, endAngle, anticlockwise);

        if (i > 1)
        {
          ctx->fill();
        }
        else
        {
          ctx->stroke();
        }
      }
    }
  };
  testCases["tc_2d_MDN_arcTo_1"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    // Tangential lines
    ctx->beginPath();
    ctx->strokeStyle = 'gray';
    ctx->moveTo(200 * 1, 20 * 1);
    ctx->lineTo(200 * 1, 130 * 1);
    ctx->lineTo(50 * 1, 20 * 1);
    ctx->stroke();

    // Arc
    ctx->beginPath();
    ctx->strokeStyle = 'black';
    ctx->lineWidth = 5 * 1;
    ctx->moveTo(200 * 1, 20 * 1);
    ctx->arcTo(200 * 1, 130 * 1, 50 * 1, 20 * 1, 40 * 1);
    ctx->stroke();

    // Start point
    ctx->beginPath();
    ctx->fillStyle = 'blue';
    ctx->arc(200 * 1, 20 * 1, 5 * 1, 0, 2 * Math.PI);
    ctx->fill();

    // Control points
    ctx->beginPath();
    ctx->fillStyle = 'red';
    ctx->arc(200 * 1, 130 * 1, 5 * 1, 0, 2 * Math.PI); // Control point one
    ctx->arc(50 * 1, 20 * 1, 5 * 1, 0, 2 * Math.PI);   // Control point two
    ctx->fill();
  };
  testCases["tc_2d_MDN_arcTo_2"] = [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx, int width, int height) {
    ctx->beginPath();
    ctx->moveTo(180 * 1, 90 * 1);
    ctx->arcTo(180 * 1, 130 * 1, 110 * 1, 130 * 1, 130 * 1);
    ctx->lineTo(110 * 1, 130 * 1);
    ctx->stroke();
  };
}