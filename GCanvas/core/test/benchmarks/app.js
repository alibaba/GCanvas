var fs = require("fs");
var files = ['tc_2d_fill', 'tc_2d_fillRect'];
var wstream = fs.createWriteStream('test.cpp');
var prefix=`#include <unordered_map>
#include <functional>
#include <string>
#include "GCanvas.hpp" 
void prepareCases( std::unordered_map< std::string,std::function<void(std::shared_ptr<gcanvas::GCanvas> canvas,  GCanvasContext *mCanvasContext,int width,int height)>>  &testCases)
{`

var postfix=`}`

wstream.write(prefix);
for (let i = 0; i < files.length; i++) {
    var data = fs.readFileSync(files[i] + ".js");
    var content = data.toString().replace(/ctx./g, "ctx->");
    var str = `testCases['${files[i]}']= [](std::shared_ptr<gcanvas::GCanvas> canvas, GCanvasContext *ctx,int width,int height)
    {${content}};`;
    console.log(str);
    wstream.write(str);
}
wstream.write(postfix);




