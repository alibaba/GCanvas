const { createCanvas, Image } = require('../export');
const canvas = createCanvas(500, 500);
const ctx = canvas.getContext('2d')

ctx.save();
ctx.rect(10, 10, 100, 120);
ctx.stroke();
ctx.clip();
ctx.fillStyle = "green";
ctx.fillRect(0, 0, 150, 100);
ctx.restore();

ctx.fillStyle = "green";
ctx.fillRect(200, 0, 150, 100);

const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/clip.png');
var stream = canvas.createPNGStream();
stream.pipe(out);