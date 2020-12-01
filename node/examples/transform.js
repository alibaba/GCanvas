const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.transform(1,0,0,1,0,0)
ctx.save();
ctx.transform(1,0,0,1,100.5,100.5);
var grd=ctx.createLinearGradient(0,0,200,0);
grd.addColorStop(0,"red");
grd.addColorStop(1,"blue");
ctx.fillStyle=grd;
ctx.arc(0, 0, 100, 0, 2 * Math.PI);
ctx.save();
ctx.transform(1,0,0,1,-100.5,-100.5);
ctx.fill();
ctx.restore();
ctx.restore();
ctx.restore();

const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/transfrom.png');
var stream = canvas.createPNGStream();
stream.pipe(out);
