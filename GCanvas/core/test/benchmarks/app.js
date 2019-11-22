var fs = require("fs");
var path = require("path");
// var fileName = process.argv[2];
// var caseName =process.argv[3];
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
    "rect":"Rect",
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

let caseNames = {
    "arc" :["tc_2d_arc.js","tc_2d_arcTo.js","tc_2d_MDN_arc.js","tc_2d_MDN_arcTo_1.js","tc_2d_MDN_arcTo_2.js",],
    "clip" :["td_2d_clip.js",],
    "composite" :["compite_desover.js","composite_sourceover.js",],
    "eclipse" :["tc_2d_ellipse.js",],
    "fill" :["tc_2d_clearRect.js","tc_2d_fill.js","tc_2d_fillRect.js","tc_2d_text_fillText.js",],
    "fillstyle" :["tc_2d_createRadialGradient.js","tc_2d_globalAlpha.js","tc_2d_lineargradient.js","tc_2d_radialgradient.js",],
    "font" :["tc_2d_font.js","tc_2d_textBaseline.js","tc_2d_text_fillText_alpha.js","tc_2d_text_fillText.js","tc_2d_text_measureText.js","tc_2d_text_strokeText.js","tc_2d_text_textAlign.js",],
    "image" :["tc_2d_get_put_ImageData.js",],
    "line" :["tc_2d_bezierCurveTo.js","tc_2d_line_dash.js","tc_2d_lineGap_butt.js","tc_2d_lineGap_round.js","tc_2d_lineGap_square.js","tc_2d_lineJoin_bevel.js","tc_2d_lineJoin_miter.js","tc_2d_lineJoin_round.js","tc_2d_lineTo.js","tc_2d_MDN_lineTo.js","tc_2d_quadraticCurveTo.js",],
    "path" :["tc_2d_beginPath.js","tc_2d_closePath.js","tc_2d_lineTo.js","tc_2d_moveTo.js",],
    "shaow" :["tc_2d_shadow.js","tc_2d_shadowOffsetX.js","tc_2d_shadowOffsetY.js","tc_2d_shaow_blur.js",],
    "stroke" :["tc_2d_stroke_alpha.js","tc_2d_stroke.js","tc_2d_strokeRect.js","tc_2d_strokeStyle.js",],
    "verb" :["tc_2d_MDN_scale.js","tc_2d_MDN_transform.js","tc_2d_MDN_translate.js","tc_2d_resetTransform.js","tc_2d_rotate.js","tc_2d_save.js","tc_2d_scale.js","tc_2d_transform.js","tc_2d_translate.js",],    
};

for (let path in caseNames) {
    let folder = caseNames[path];
    for (let i in folder) {
        let caseName = folder[i];
        writetoFile( path, caseName,caseName.slice(0, caseName.length - 3));
    }
}

function writetoFile(path,fileName,caseName){
 console.log(path);
 console.log(fileName);
var data = fs.readFileSync(path+"/"+fileName);
var content = dealFileContent(data.toString());
console.log(content);
var str = `testCases["${caseName}"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { \n    int  ratio=1; \n  ${content}  \n };\n`;
console.log(str);
wstream.write(str);
}


function dealFileContent(content) {
    return content.toString().replace(
        /ctx.|beginPath|moveTo|lineTo|stroke\(|fill\(|fillStyle|fillRect|rect|c.width|c.height|strokeStyle|arc|arcTo|quadraticCurveTo|bezierCurveTo|clip|font|fillText|lineWidth|shadowBlur|shadowOffsetX|shadowOffsetY|shadowColor|save|strokeRect|rotate|restore|scale|translate|Math.PI|closePath|'|setTransform|lineCap|lineJoin|miterLimit|clearRect/g, (matched) => {
            return dict[matched];
        });
}
wstream.write(postfix);




