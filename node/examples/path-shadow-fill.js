const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400)
const ctx = canvas.getContext('2d')
const c = canvas
ctx.fillStyle = "#ffffff"
ctx.shadowBlur = 10;
ctx.fillRect(0, 0, c.width, c.height)
ctx.shadowOffsetX = 6;
ctx.shadowOffsetY = 6;
ctx.fillStyle = "#ff00ff"
ctx.shadowColor = "orange";
ctx.fillRect(0, 0, 200, 200);

const fs = require('fs')
const path = require('path')
const out = fs.createWriteStream(path.join(__dirname, "..") + '/path-shadow-fill.png');
var stream = canvas.createPNGStream();
stream.pipe(out);
