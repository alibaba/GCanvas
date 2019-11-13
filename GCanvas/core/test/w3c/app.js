
const puppeteer = require('puppeteer');
(async () => {
    const browser = await puppeteer.launch();
    const page = await browser.newPage();
    let src = `file://${process.cwd()}/canvasPage.html`;
    await page.goto(src);
    const canvas = await page.$('#myCanvas');
    const boundingBox4Canvas = await canvas.boundingBox();
    if (canvas != null) {
        await canvas.screenshot({
            path: "./build/mycanvasPage.png",
            clip: {
                x: boundingBox4Canvas.x,
                y: boundingBox4Canvas.y,
                width: Math.min(boundingBox4Canvas.width, page.viewport().width),
                height: Math.min(boundingBox4Canvas.height, page.viewport().height),
            }
        })
    }else{
        console.log("canvas element not exist");
    }
    console.log("finish the canvas page");
    await browser.close();
})();