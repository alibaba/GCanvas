const fs = require('fs')
const path = require('path')
const out1 = fs.createWriteStream(path.join(__dirname, "..")+ '/imageCanvas1.png');
const out2 = fs.createWriteStream(path.join(__dirname, "..")+ '/imageCanvas2.png');

const { createCanvas, Image } = require('../export');

function localFunctionCreateCanvas() {
    let canvas = createCanvas(400, 400);
    let canvas2 = createCanvas(400, 400);
    let canvas3 = createCanvas(400, 400);
    let canvas4 = createCanvas(400, 400);
}

localFunctionCreateCanvas();
try {
    if (global.gc) {
        console.log(`called gc force`);
        global.gc();
    } else {
        console.log("not called gc ");
    }
} catch (e) {
    console.log("`node --expose-gc index.js`");
    process.exit();
}

setTimeout(() => {
    const canvas = createCanvas(400, 400);
    const ctx = canvas.getContext('2d');
    const img = new Image()

    img.onload = () => {
        console.log("image1 onload success!");
        ctx.fillStyle = "#000000";
        ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
        ctx.drawImage(img, 0, 0, 100, 100);
        canvas.createPNGStream().pipe(out1);

        nextCanvasDrawImage();
    }
    img.onerror = err => {
        console.log(err)
    }
    img.src = "https://img.alicdn.com/imgextra/i3/80/O1CN011CSgGWymNlCrNO3_!!80-2-luban.png"


    function nextCanvasDrawImage() {
        const canvas2 = createCanvas(400, 400);
        const ctx2 = canvas2.getContext('2d');
        let img2 = new Image();
        img2.onerror = err => {
            console.log(err)
        }
        img2.onload = () => {
            console.log("image2 onload success!");
            ctx2.fillStyle = "#000000";
            ctx2.fillRect(0, 0, ctx2.canvas.width, ctx2.canvas.height);
            ctx2.drawImage(img, 0, 0, 100, 100);
            canvas2.createPNGStream().pipe(out2);

        }
        img2.src = "https://img.alicdn.com/imgextra/i3/80/O1CN011CSgGWymNlCrNO3_!!80-2-luban.png"
    }
}, 100);

