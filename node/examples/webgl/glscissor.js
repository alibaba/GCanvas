const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/scissor.png');

const canvas = createCanvas(400, 400);
var gl = canvas.getContext("webgl");


gl.enable(gl.SCISSOR_TEST);
gl.scissor(0, 0, canvas.width, canvas.height);
gl.clearColor(1.0,  0.5, 0.5, 1.0);
gl.clear(gl.COLOR_BUFFER_BIT);
gl.finish();


gl.scissor(60, 60, 60, 60);
var color = [0.0,0.5,0.0,1.0];
gl.clearColor(color[0], color[1], color[2], color[3]);
gl.clear(gl.COLOR_BUFFER_BIT);

var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});