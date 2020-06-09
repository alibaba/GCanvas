const { createCanvas, Image } = require('../export')
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/emptyCanvas.png');
// ctx.fillStyle = "rgba(0.5, 0, 0, 0.2)";
// ctx.fillRect(0,0,ctx.canvas.width,ctx.canvas.height);
var stream=canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});