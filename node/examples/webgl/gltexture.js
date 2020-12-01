const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');

const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/texture.png');
const canvas = createCanvas(900, 900);
var gl = canvas.getContext("webgl");


function setRectangle(gl, x, y, width, height) {
    var x1 = x;
    var x2 = x + width;
    var y1 = y;
    var y2 = y + height;
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        x1, y1,
        x2, y1,
        x1, y2,
        x1, y2,
        x2, y1,
        x2, y2,
    ]), gl.STATIC_DRAW);
}

function drawImage(gl, canvas, image) {

    var fragmentShaderStr = "precision mediump float;" +
        "uniform sampler2D u_image;" +
        "varying vec2 v_texCoord;" +
        "void main() {" +
        "gl_FragColor = texture2D(u_image, v_texCoord);" +
        "}";
    var vertexShaderStr = "attribute vec2 a_position;" +
        "attribute vec2 a_texCoord;" +
        "uniform vec2 u_resolution;" +
        "varying vec2 v_texCoord;" +
        "void main() {" +
        "vec2 zeroToOne = a_position / u_resolution;" +
        "vec2 zeroToTwo = zeroToOne * 2.0;" +
        "vec2 clipSpace = zeroToTwo - 1.0;" +
        "gl_Position = vec4(clipSpace * vec2(1, -1), 0, 1);" +
        "v_texCoord = a_texCoord;" +
        "}";
    var vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexShaderStr);
    gl.compileShader(vertexShader);

    var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentShaderStr);
    gl.compileShader(fragmentShader);

    var program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);

    // gl.bindAttribLocation(gl.program, v3PositionIndex, "a_position");
    gl.linkProgram(program);

    // var samplerIndex = gl.getUniformLocation(gl.program, "a_texCoord");
    // gl.useProgram(gl.program);

    var positionLocation = gl.getAttribLocation(program, "a_position");
    var texcoordLocation = gl.getAttribLocation(program, "a_texCoord");

    var positionBuffer = gl.createBuffer();

    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    setRectangle(gl, 0, 0, image.width, image.height);

    var texcoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
    ]), gl.STATIC_DRAW);

    // Create a texture.
    var texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);

    // Set the parameters so we can render any size image.
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    // Upload the image into the texture.
    console.log(`the gl.RGBA is ${gl.RGBA}`)
    console.log(`TEXTURE_2D is ${gl.TEXTURE_2D}`)
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

    // lookup uniforms
    var resolutionLocation = gl.getUniformLocation(program, "u_resolution");
    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, canvas.width, canvas.height);
    //
    //      // Clear the canvas
    gl.clearColor(0.0, 0.2, 0.0, 0.3);
    gl.clear(gl.COLOR_BUFFER_BIT);
    //
    //      // Tell it to use our program (pair of shaders)
    gl.useProgram(program);
    //
    //      // Turn on the position attribute
    gl.enableVertexAttribArray(positionLocation);
    //
    //      // Bind the position buffer.
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    //
    //      // Tell the position attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    // Tell the position attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(positionLocation, size, type, normalize, stride, offset)

    // Turn on the teccord attribute
    gl.enableVertexAttribArray(texcoordLocation);

    // Bind the position buffer.
    gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);

    // Tell the position attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(texcoordLocation, size, type, normalize, stride, offset)

    // set the resolution
    gl.uniform2f(resolutionLocation, canvas.width, canvas.height);

    // Draw the rectangle.
    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 6;
    gl.drawArrays(primitiveType, offset, count);
}

var image = new Image();
image.onload = function () {
    drawImage(gl, canvas, image);
    var stream = canvas.createPNGStream();
    stream.on('data', function (chunk) {
        out.write(chunk);
    });
}
image.src = "https://img.alicdn.com/tfs/TB1FQDkCEz1gK0jSZLeXXb9kVXa-1200-807.jpg";
