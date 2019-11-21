
const puppeteer = require('puppeteer');
let caseNames = {
    "path": ["tc_2d_beginPath.js", "tc_2d_closePath.js"],
    "fill": ["tc_2d_clearRect.js", "tc_2d_fillRect.js"],
    "verb": ["tc_2d_rotate.js"],
    "stroke": ["tc_2d_strokeRect.js"]
};
const fs =require('fs');
var path = require("path");

async function genPage(folder,fileName) {
    let scriptTagDynamic = `<script src="../benchmarks/${folder}/${fileName}"></script>`;
    let pageContent=`<!DOCTYPE HTML>
    <html>
    <body>
     <canvas id="myCanvas" style="width:300px;height:300px;"
        width="300px" height="300px">
    Your browser does not support the canvas element.
    </canvas>
    <script type="text/javascript" src="./env.js">
    </script>
    ${scriptTagDynamic}
    </body>
    </html>`;
    var wstream = fs.createWriteStream(path.resolve("canvasPage.html"));
    wstream.write(pageContent);
    console.log(pageContent);
}


async function genStandard(caseName) {
    const browser = await puppeteer.launch();
    const page = await browser.newPage();
    let src = `file://${process.cwd()}/canvasPage.html`;
    await page.goto(src);
    const canvas = await page.$('#myCanvas');
    const boundingBox4Canvas = await canvas.boundingBox();
    if (canvas != null) {
        await canvas.screenshot({
            path: `./build/${caseName}.png`,
            clip: {
                x: boundingBox4Canvas.x,
                y: boundingBox4Canvas.y,
                width: Math.min(boundingBox4Canvas.width, page.viewport().width),
                height: Math.min(boundingBox4Canvas.height, page.viewport().height),
            }
        })
    } else {
        console.log("canvas element not exist");
    }
    console.log("finish the canvas page"+caseName);
    await browser.close();
}

(async () => {
    for (let path in caseNames) {
        let folder=caseNames[path];
        for (let i in folder) {
            let caseName=folder[i];
            genPage(path,caseName);
            if(caseName.endsWith(".js")){
               caseName=caseName.slice(0,caseName.length-3);
            }
            await genStandard(caseName);
        }
    }
})();