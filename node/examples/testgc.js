const { createCanvas, Image } = require('../export')


const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.resolve(__dirname, '..') + '/testgc.png');

function localFunctionCreateCanvas() {
    let canvas = createCanvas(400, 400);
    let canvas2 = createCanvas(400, 400);
    let canvas3 = createCanvas(400, 400);
}

localFunctionCreateCanvas();
try {
    if (global.gc) { 
        console.log(`called gc force`);
        global.gc(); 
    }else{
        console.log("not called gc ");
    }
} catch (e) {
    console.log("`node --expose-gc index.js`");
    process.exit();
}
setTimeout(() => {
    const canvasGlobal = createCanvas(400, 400);
    const ctx = canvasGlobal.getContext('2d')
    ctx.fillStyle = "#ff0000"
    ctx.fillRect(0, 0, canvasGlobal.width, canvasGlobal.height);
    var stream = canvasGlobal.createPNGStream();
    stream.pipe(out);
}, 100);
