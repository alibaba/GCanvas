const { createCanvas, Image } = require('../export');
var canvas=createCanvas(320,320)
var context = canvas.getContext('2d');

context.fillStyle="#ffffff"
context.fillRect(0,0,canvas.width,canvas.height)
context.fillStyle="#000000"
context.font = "18pt Helvetica";
context.fillText("No shadow", 90, 40);

context.shadowOffsetX = 3;
context.shadowOffsetY = 3;
context.fillText("No shadow", 90, 80);

context.shadowColor = "rgba(0,0,0,0.3)";
context.fillText("shadow", 90, 120);

context.shadowBlur = 4;
context.fillText("Smooth shadow", 90, 160);

const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/font-shadow.png');
var stream = canvas.createPNGStream();
stream.pipe(out);