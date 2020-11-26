const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');

const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/triangle.png');
const canvas = createCanvas(400, 400);
const gl = canvas.getContext('webgl')

function draw() {

    var vertices = [
        -1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0,1.0,0.0,
    ];

    var indices = [0,1,2];

    var vertex_buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    var Index_Buffer = gl.createBuffer();


    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);

    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);


    var vertCode =
    "attribute vec4 vPosition;\n"+
    "void main()\n"+
    "{\n"+
    "  gl_Position = vPosition;\n"+
    "}\n";
    var vertShader = gl.createShader(gl.VERTEX_SHADER);

    gl.shaderSource(vertShader, vertCode);


    gl.compileShader(vertShader);

    var fragCode ="void main()\n"+
    "{\n"+
    "  gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"+
    "}\n";

    var fragShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragShader, fragCode);


    gl.compileShader(fragShader);

    var shaderProgram = gl.createProgram();

    gl.attachShader(shaderProgram, vertShader);

    gl.attachShader(shaderProgram, fragShader);

    gl.linkProgram(shaderProgram);
    gl.useProgram(shaderProgram);


    gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);

    var coord = gl.getAttribLocation(shaderProgram, "vPosition");
    gl.vertexAttribPointer(coord, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(coord);
    gl.enable(gl.DEPTH_TEST);
    gl.viewport(0,0, canvas.width, canvas.height);
    gl.clearColor(1, 0.5, 0.4, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);

}

draw();
 
 var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});