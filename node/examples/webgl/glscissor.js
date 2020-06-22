//var stats;

var that;
var width;
var height;
const { createCanvas, Image } = require('../../export')
const canvas = createCanvas(400, 400);
var gl = canvas.getContext("3d");

width = canvas.width;
height =canvas.height;


function getRandomColor() {
  return [Math.random(), Math.random(), Math.random()];
}



var color = getRandomColor();
var position;

// Variables to hold the size and velocity of the square.
var size = [60, 60];
var velocity = 9.0;

function drawAnimation() {
//  stats.begin();

  gl.scissor(0, 0, width, height);
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);

  gl.scissor(position[0], position[1], size[0], size[1]);
  gl.clearColor(color[0], color[1], color[2], 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
  // Every frame the vertical position of the square is
  // decreased, to create the illusion of movement.
  position[1] -= 30;
  // When the sqaure hits the bottom of the drawing buffer,
  // we override it with new square of different color and
  // velocity.
  if (position[1] < 0) {
    // Horizontal position chosen randomly, and vertical
    // position at the top of the drawing buffer.
    position = [
      Math.random() * (width - size[0]),
      height
    ];
    // Random velocity between 1.0 and 7.0
    velocity = 6.0 + 3.0;// * Math.random();
    color = getRandomColor();
  }

  // canvasLog("scissor:" + position[1]);

//  stats.end();
}


gl.enable(gl.SCISSOR_TEST);
position = [0, height];
drawAnimation();
canvas.createPNG("glsissor")