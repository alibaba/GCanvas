const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/triangle2.png');


const canvas = createCanvas(400, 400);
const gl = canvas.getContext('webgl')

function draw() {

    function initShaders(e, r, a) { var t = createProgram(e, r, a); return t ? (e.useProgram(t), e.program = t, !0) : (console.log("Failed to create program"), !1) } function createProgram(e, r, a) { var t = loadShader(e, e.VERTEX_SHADER, r), o = loadShader(e, e.FRAGMENT_SHADER, a); if (!t || !o) return null; var l = e.createProgram(); if (!l) return null; if (e.attachShader(l, t), e.attachShader(l, o), e.linkProgram(l), !e.getProgramParameter(l, e.LINK_STATUS)) { var n = e.getProgramInfoLog(l); return console.log("Failed to link program: " + n), e.deleteProgram(l), e.deleteShader(o), e.deleteShader(t), null } return l } function loadShader(e, r, a) { var t = e.createShader(r); if (null == t) return console.log("unable to create shader"), null; if (e.shaderSource(t, a), e.compileShader(t), !e.getShaderParameter(t, e.COMPILE_STATUS)) { var o = e.getShaderInfoLog(t); return console.log("Failed to compile shader: " + o), e.deleteShader(t), null } return t } function getWebGLContext(e, r) { var a = WebGLUtils.setupWebGL(e); return a ? ((arguments.length < 2 || r) && (a = WebGLDebugUtils.makeDebugContext(a)), a) : null }
    
    function init(VSHADER_SOURCE, FSHADER_SOURCE) {    
        if (!initShaders(gl, VSHADER_SOURCE, FSHADER_SOURCE)) {
            console.log("Failed to init the shaders ");
            return;
        }


        let vertices = new Float32Array([0.0, 0.5, -0.5, -0.5, 0.5, -0.5]);
        let n = 3;
        let vertexBuffer = gl.createBuffer();
        if (!vertexBuffer) {
            console.log("Failed to create buffer\n");
        }
        gl.viewport(0,0,canvas.width,canvas.height);
        gl.clearColor(0.0, 0.0, 1.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
        
        let a_Position = gl.getAttribLocation(gl.program, 'a_Position');
        gl.vertexAttribPointer(a_Position, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(a_Position);
        
        let u_FragColor = gl.getUniformLocation(gl.program, 'u_FragColor');
        gl.uniform4f(u_FragColor, 1.0, 1.0, 0.0, 1.0);
      
        gl.drawArrays(gl.TRIANGLES, 0, n);
    
    }
    let vertexShader="attribute vec4 a_Position; \n"+
    "void main(){ \n"+
        "gl_Position=a_Position;\n"+
    "}\n";
  
    let fragmentShader=
    "precision mediump float; \n"+
    "uniform vec4 u_FragColor; \n"+
    "void main(){\n"+
        "gl_FragColor=u_FragColor;\n"+
    "}\n";

    init(vertexShader,fragmentShader);

}

draw();
 
 var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});