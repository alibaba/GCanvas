const { createCanvas, Image } = require('../../export')
const canvas = createCanvas(400, 400);
const gl = canvas.getContext('webgl')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/triagnleWebGL.png');
console.log("canvas=" + canvas);
console.log("gl=" + gl);


function draw() {

    var vertices = [
        -0.5,0.5,0.0,
        -0.5,-0.5,0.0,
        0.5,-0.5,0.0,
    ];

    var indices = [0,1,2];

    var vertex_buffer = gl.createBuffer();
    console.log("gl createbuffer: " + vertex_buffer);
    gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);

    console.log("gl bufferData");
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    console.log("gl bindBuffer 2");
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    var Index_Buffer = gl.createBuffer();

    console.log("gl bindBuffer 3");

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);

    console.log("gl bufferData 2");
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);


    var vertCode =
    'attribute vec3 coordinates;' +

    'void main(void) {' +
       ' gl_Position = vec4(coordinates, 1.0);' +
    '}';

    var vertShader = gl.createShader(gl.VERTEX_SHADER);

    gl.shaderSource(vertShader, vertCode);


    gl.compileShader(vertShader);

    var fragCode =
    'void main(void) {' +
       ' gl_FragColor = vec4(0.0, 1.0, 1.0, 0.6);' +
    '}';

    var fragShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragShader, fragCode);
    console.log("shader source:" + fragCode);


    gl.compileShader(fragShader);

    var shaderProgram = gl.createProgram();
    console.log("create program:" + shaderProgram);

    gl.attachShader(shaderProgram, vertShader);

    gl.attachShader(shaderProgram, fragShader);

    gl.linkProgram(shaderProgram);
    gl.useProgram(shaderProgram);

    console.log("step 4----------------: ");

    gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);

    var coord = gl.getAttribLocation(shaderProgram, "coordinates");
    console.log("getAttribLocation: " + coord);

    gl.vertexAttribPointer(coord, 3, gl.FLOAT, false, 0, 0);

    gl.enableVertexAttribArray(coord);
    console.log(`canvas width is ${canvas.width} height is ${canvas.height}`)
    gl.viewport(0,0, canvas.width, canvas.height);
    gl.clearColor(1, 0.5, 0.4, 0.9);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);
    console.log("gl.drawElements : " + gl.UNSIGNED_SHORT);

}

draw();
 
 var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});