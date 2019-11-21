var fs = require("fs");
var path = require("path");
var fileName = process.argv[2];
var caseName =process.argv[3];
var wstream = fs.createWriteStream(path.resolve("./build/test.cc"));
var prefix = `#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{ \n  `

var postfix = ` }\n`


var dict = {
    "ctx.": "ctx->",
    "Math.PI": "M_PI",
    "beginPath": "BeginPath",
    "moveTo": "MoveTo",
    "lineTo": "LineTo",
    "stroke(": "Stroke(",
    "fill(": "Fill(",
    "fillRect": "FillRect",
    "arc": "Arc",
    "arcto": "ArcTo",
    "bezierCurveTo": "BezierCurveTo",
    "clip": "Clip",
    "closePath":"ClosePath",
    "font": "SetFont(",
    "save": "Save",
    "restore": "Restore",
    "rotate": "Rotate",
    "scale": "Scale",
    "translate": "Translate",
    "strokeRect": "StrokeRect",
    "fillText": "FillText",
    "clearRect":"ClearRect",
    "strokeStyle": "SetStrokeStyle(",
    "fillStyle": "SetFillStyle(",
    "lineWidth": "SetLineWidth(",
    "c.width": "width",
    "c.height": "height",
    "quadraticCurveTo": "QuadraticCurveTo",
    "shadowBlur": "SetShadowBlur(",
    "shadowOffsetX": "SetShadowOffsetX(",
    "shadowOffsetY": "SetShadowOffsetY(",
    "shadowColor": "SetShadowColor(",
    "setTransform":"SetTransform",
     "lineCap":"SetLineCap(",
     "lineJoin":"SetLineJoin(",
     "miterLimit":"SetMiterLimit(",
    "\'":" \""
}

wstream.write(prefix);

var data = fs.readFileSync(fileName);
var content = dealFileContent(data.toString());
var str = `testCases["${caseName}"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { \n    int  ratio=1; \n  ${content}  \n };\n`;
console.log(str);
wstream.write(str);


var regex2 = new RegExp('\\w+');
function dealFileContent(content) {
    return data.toString().replace(
        /ctx.|beginPath|moveTo|lineTo|stroke\(|fill\(|fillStyle|fillRect|c.width|c.height|strokeStyle|arc|arcTo|bezierCurveTo|clip|font|fillText|lineWidth|shadowBlur|shadowOffsetX|shadowOffsetY|shadowColor|save|strokeRect|rotate|restore|scale|translate|Math.PI|closePath|'|setTransform|lineCap|lineJoin|miterLimit|clearRect/g, (matched) => {
            return dict[matched];
        });
}
wstream.write(postfix);




