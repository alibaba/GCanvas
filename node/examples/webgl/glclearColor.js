const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/clearcolor.png');

const canvas = createCanvas(400, 400);
var gl = canvas.getContext("webgl");

gl.viewport(0,0,canvas.width,canvas.height);
gl.clearColor(1.0,  0.5, 0.5, 1.0);
gl.clear(gl.COLOR_BUFFER_BIT);

var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});