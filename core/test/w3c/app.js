
const puppeteer = require('puppeteer');
let caseNames = {"arc":["tc_2d_MDN_arc.js","tc_2d_MDN_arcTo_1.js","tc_2d_MDN_arcTo_2.js","tc_2d_arc.js","tc_2d_arcTo.js","tc_2d_arc_circle.js",],"clip":["tc_2d_resetClip.js","td_2d_clip.js",],"composite":["compite_desover.js","composite_sourceover.js",],"eclipse":["tc_2d_ellipse.js",],"fill":["tc_2d_clearRect.js","tc_2d_fill.js","tc_2d_fillRect.js","tc_2d_text_fillText.js",],"fillstyle":["tc_2d_createRadialGradient.js","tc_2d_globalAlpha.js","tc_2d_lineargradient.js","tc_2d_radialgradient.js",],"font":["tc_2d_font.js","tc_2d_text_fillText.js","tc_2d_text_fillText_alpha.js","tc_2d_text_measureText.js","tc_2d_text_strokeText.js","tc_2d_text_textAlign.js","tc_2d_text_textBaseLine.js",],"image":["tc_2d_get_put_ImageData.js",],"line":["tc_2d_MDN_lineTo.js","tc_2d_bezierCurveTo.js","tc_2d_lineGap_butt.js","tc_2d_lineGap_round.js","tc_2d_lineGap_square.js","tc_2d_lineJoin_bevel.js","tc_2d_lineJoin_miter.js","tc_2d_lineJoin_round.js","tc_2d_lineTo.js","tc_2d_line_dash.js","tc_2d_quadraticCurveTo.js",],"path":["tc_2d_beginPath.js","tc_2d_closePath.js","tc_2d_lineTo.js","tc_2d_moveTo.js",],"shaow":["tc_2d_shadow.js","tc_2d_shadowOffsetX.js","tc_2d_shadowOffsetY.js","tc_2d_shadow_alpha.js","tc_2d_shadow_fill.js","tc_2d_shadow_fillrect.js","tc_2d_shadow_image.js","tc_2d_shadow_stroke.js","tc_2d_shadow_strokerect.js","tc_2d_shaow_blur.js",],"stroke":["tc_2d_stroke.js","tc_2d_strokeRect.js","tc_2d_strokeStyle.js","tc_2d_stroke_alpha.js",],"verb":["tc_2d_MDN_scale.js","tc_2d_MDN_transform.js","tc_2d_MDN_translate.js","tc_2d_resetTransform.js","tc_2d_rotate.js","tc_2d_save.js","tc_2d_scale.js","tc_2d_transform.js","tc_2d_translate.js","tc_sample_path_arc_rotate.js","tc_sample_path_rotate.js","tc_sample_path_scale.js","tc_sample_path_transform.js","tc_sample_path_transform_fill.js","tc_sample_path_transform_fill2.js","tc_sample_path_translate.js",], };
const fs = require('fs');
var path = require("path");

async function genPage(folder, fileName) {
    let scriptTagDynamic = `<script src="../benchmarks/${folder}/${fileName}"></script>`;
    let pageContent = `<!DOCTYPE HTML>
    <html>
    <body>
     <canvas id="myCanvas" style="width:400px;height:400px;"
        width="400px" height="400px">
    Your browser does not support the canvas element.
    </canvas>
    <script type="text/javascript" src="./env.js">
    </script>
    ${scriptTagDynamic}
    </body>
    </html>`;
    var wstream = fs.createWriteStream(path.resolve("canvasPage.html"));
    wstream.write(pageContent);
    // console.log(pageContent);
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
    console.log("finish the canvas page  " + caseName);
    await browser.close();
}

(async () => {
    for (let path in caseNames) {
        let folder = caseNames[path];
        for (let i in folder) {
            let caseName = folder[i];
            genPage(path, caseName);
            if (caseName.endsWith(".js")) {
                caseName = caseName.slice(0, caseName.length - 3);
            }
            await genStandard(caseName);
        }
    }
})();