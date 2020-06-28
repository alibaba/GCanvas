//var stats;

var that;
var width;
var height;
const { createCanvas, Image } = require('../../export')
const canvas = createCanvas(400, 400);
var gl = canvas.getContext("webgl");

width = canvas.width;
height =canvas.height;


var color = [0.0,0.5,0.0,1.0];
var position;

// Variables to hold the size and velocity of the square.
var size = [60, 60];
var velocity = 9.0;

function draw() {
  gl.scissor(0, 0, width, height);
  gl.clearColor(1.0, 0.5, 0.5, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);

  gl.scissor(60, 60, 60, 60);
  gl.clearColor(color[0], color[1], color[2], 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
}

gl.viewport(0,0,width,height);
gl.enable(gl.SCISSOR_TEST);
position = [0, height];
draw();
canvas.createPNG("sissor")