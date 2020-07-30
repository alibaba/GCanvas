const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');

const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/stencil.png');
const canvas = createCanvas(600, 600);
var gl = canvas.getContext("webgl");
var width;
var height;
var program = null;
var program2 = null;
var samplerUniform = null;
var maskTexture;

var shader_fs_2 = "precision highp float;\
varying vec2 vTextureCoord;\
uniform sampler2D uSampler;\
void main(void) {\
    gl_FragColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));\
    if (gl_FragColor.a == 0.0) {\
        discard;\
    }\
}"

var shader_vs_2 = "precision highp float;\
attribute vec3 aPos;\
attribute vec2 aTextureCoords;\
varying vec2 vTextureCoord;\
void main(void){\
    gl_Position = vec4(aPos, 1.0);\
    vTextureCoord = aTextureCoords;\
}"

var shader_vs = "precision highp float;\
attribute vec3 aPos;\
attribute vec4 aColor;\
varying vec4 vColor;\
void main(void){\
    gl_Position = vec4(aPos, 1);\
    vColor = aColor;\
}"

var shader_fs = "precision highp float;\
varying vec4 vColor;\
void main(void) {\
    gl_FragColor = vColor;\
}"

function getGLContext() {
  if (gl) {
    gl.clearColor(74 / 255, 115 / 255, 94 / 255, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.enable(gl.STENCIL_TEST);
  }
}

function initShaders(vs_source, fs_source) {
  //compile shaders
  var vertexShader = makeShader(vs_source, gl.VERTEX_SHADER);
  var fragmentShader = makeShader(fs_source, gl.FRAGMENT_SHADER);

  //create program
  var glProgram = gl.createProgram();

  //attach and link shaders to the program
  gl.attachShader(glProgram, vertexShader);
  gl.attachShader(glProgram, fragmentShader);
  gl.linkProgram(glProgram);

  if (!gl.getProgramParameter(glProgram, gl.LINK_STATUS)) {
    alert("Unable to initialize the shader program.");
  }

  //use program
  // gl.useProgram(glProgram);
  return glProgram;
}

function makeShader(src, type) {
  //compile the vertex shader
  var shader = gl.createShader(type);
  gl.shaderSource(shader, src);
  gl.compileShader(shader);

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert("Error compiling shader: " + gl.getShaderInfoLog(shader));
  }
  return shader;
}
// vertex representing the triangle
var vertex = [
  -.5, -.2, 0,
  .5, -.2, 0,
  0, .6, 0
];
var stencilVertex = [
  -.2, -.5, 0,
  .4, -.5, 0,
  .3, .6, 0
];
function setupBufferAndDraw() {
  // draw the mask image as stencil
  gl.useProgram(program2);
  var maskVertex = [
    -1, -1, 0,
    1, -1, 0,
    1, 1, 0,
    -1, -1, 0,
    1, 1, 0,
    -1, 1, 0
  ];
  var maskTexCoords = [
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1
  ];
  var maskBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, maskBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(maskVertex), gl.STATIC_DRAW);

  var aMaskVertexPosition = gl.getAttribLocation(program2, 'aPos');
  gl.vertexAttribPointer(aMaskVertexPosition, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(aMaskVertexPosition);

  // texture coordinate data
  var maskTexCoordBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, maskTexCoordBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(maskTexCoords), gl.STATIC_DRAW);

  var vertexTexCoordAttribute = gl.getAttribLocation(program2, "aTextureCoords");
  gl.enableVertexAttribArray(vertexTexCoordAttribute);
  gl.vertexAttribPointer(vertexTexCoordAttribute, 2, gl.FLOAT, false, 0, 0);

  // Always pass test
  gl.stencilFunc(gl.ALWAYS, 1, 0xff);
  gl.stencilOp(gl.KEEP, gl.KEEP, gl.REPLACE);
  gl.stencilMask(0xff);
  gl.clear(gl.STENCIL_BUFFER_BIT);
  // No need to display the triangle
  gl.colorMask(0, 0, 0, 0);

  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, maskTexture);
  gl.uniform1i(samplerUniform, 0);

  gl.drawArrays(gl.TRIANGLES, 0, maskVertex.length / 3);
  // return;
  gl.useProgram(program);
  // Pass test if stencil value is 1
  gl.stencilFunc(gl.EQUAL, 1, 0xFF);
  gl.stencilMask(0x00);
  gl.colorMask(1, 1, 1, 1);
  // draw the clipped triangle
  var color = [
    1, 0, 0, 1,
    0, 1, 0, 1,
    0, 0, 1, 1
  ];
  var colorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(color), gl.STATIC_DRAW);

  var aColorPosition = gl.getAttribLocation(program, 'aColor');
  gl.vertexAttribPointer(aColorPosition, 4, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(aColorPosition);

  var vertexBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertex), gl.STATIC_DRAW);

  var aVertexPosition = gl.getAttribLocation(program, 'aPos');
  gl.vertexAttribPointer(aVertexPosition, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(aVertexPosition);
  gl.clear(gl.COLOR_BUFFER_BIT);
  gl.drawArrays(gl.TRIANGLES, 0, vertex.length / 3);
}

function createTexture(source) {
  var texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);
  gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, source);
  gl.bindTexture(gl.TEXTURE_2D, null);
  return texture;
}

height = canvas.height;
width = canvas.width;

getGLContext();
program = initShaders(shader_vs, shader_fs);
program2 = initShaders(shader_vs_2, shader_fs_2);
samplerUniform = gl.getUniformLocation(program2, 'uSampler');

var img = new Image();
img.onload = function() {
  maskTexture = createTexture(img);
  setupBufferAndDraw();
  var stream = canvas.createPNGStream();
  stream.on('data', function (chunk) {
      out.write(chunk);
  });
};
img.crossOrigin = "";
img.src = 'https://img.alicdn.com/tfs/TB1edrqL7Y2gK0jSZFgXXc5OFXa-128-128.png';