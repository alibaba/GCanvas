var fs = require("fs");
var path=require("path");
var files = [ 'tc_2d_lineTo'];
var wstream = fs.createWriteStream(path.resolve("../linux/util/test.cpp"));
var prefix=`#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{ \n  ` 

var postfix=` }\n`

wstream.write(prefix);
for (let i = 0; i < files.length; i++) {
    var data = fs.readFileSync(files[i] + ".js");
    var content = data.toString().replace(/ctx./g, "ctx->");
    var str = `testCases["${files[i]}"]= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    { \n    int  ratio=1; \n  ${content}  };\n`;
    console.log(str);
    wstream.write(str);
}
wstream.write(postfix);




