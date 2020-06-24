//var stats;

var that;
var width;
var height;
const { createCanvas, Image } = require('../../export')
const canvas = createCanvas(400, 400);
var gl = canvas.getContext("webgl");

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
  gl.scissor(0, 0, width, height);
  gl.clearColor(1.0, 0.5, 0.5, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);

  gl.scissor(position[0], position[1], size[0], size[1]);
  gl.clearColor(color[0], color[1], color[2], 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
  position[1] -= 30;
  if (position[1] < 0) {
    position = [
      Math.random() * (width - size[0]),
      height
    ];
    velocity = 6.0 + 3.0;// * Math.random();
    color = getRandomColor();
  }

}


gl.enable(gl.SCISSOR_TEST);
position = [0, height];
drawAnimation();
canvas.createPNG("sissor")