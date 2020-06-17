const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400)
const ctx = canvas.getContext('2d')
const c=canvas
ctx.fillStyle="#ffffff"
ctx.fillRect(0,0,c.width,c.height)
ctx.shadowOffsetX = 6;
ctx.shadowOffsetY = 6;
ctx.fillStyle="#ff0000"
ctx.shadowColor = "#00ff00";
ctx.fillRect(0, 0, 200, 200);
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/pathShadow.png');
var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});
