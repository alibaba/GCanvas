const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400)
const ctx = canvas.getContext('2d')
const c=canvas
ctx.fillStyle = "#ffffff"
ctx.fillRect(0, 0, c.width, c.height)
ctx.shadowColor = "black";
ctx.shadowBlur = 6;
ctx.shadowOffsetX = 6;
ctx.shadowOffsetY = 6;
ctx.shadowColor = "orange";
ctx.strokeRect(25, 25, 200, 200);
ctx.shadowColor = "green";
ctx.strokeRect(50, 50, 200, 200);
ctx.shadowColor = "blue";
ctx.strokeRect(75, 75, 200, 200);
ctx.shadowColor = "red";
ctx.strokeRect(100, 100, 200, 200);
const out = fs.createWriteStream(path.join(__dirname, "..") + '/pathShadowStroke.png');
var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});
