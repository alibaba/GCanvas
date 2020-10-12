const { createCanvas, Image } = require('../../export')
let canvas = createCanvas(400, 400);
let vFile = "vDrawTexture.glsl";
let fgFile = "fDrawTexture.glsl";
let OFFSCREEN_WIDTH = 512;
let OFFSCREEN_HEIGHT = 512;

const VSHADER_SOURCE = "attribute vec4 a_Position;" +
    "attribute vec2 a_TexCoord;" +
    "uniform mat4 u_xformMatix;" +
    "varying vec2 v_TexCoord;" +
    "void main(){" +
    "gl_Position=a_Position;" +
    "v_TexCoord=a_TexCoord;" +
    "}"

const FSHADER_SOURCE = "precision mediump float;" +
    "uniform sampler2D u_Sampler0;" +
    "varying vec2 v_TexCoord;" +
    "void main() {" +
    "gl_FragColor = texture2D(u_Sampler0, v_TexCoord);" +
    "}"

function initShaders(e, r, a) { var t = createProgram(e, r, a); return t ? (e.useProgram(t), e.program = t, !0) : (console.log("Failed to create program"), !1) } function createProgram(e, r, a) { var t = loadShader(e, e.VERTEX_SHADER, r), o = loadShader(e, e.FRAGMENT_SHADER, a); if (!t || !o) return null; var l = e.createProgram(); if (!l) return null; if (e.attachShader(l, t), e.attachShader(l, o), e.linkProgram(l), !e.getProgramParameter(l, e.LINK_STATUS)) { var n = e.getProgramInfoLog(l); return console.log("Failed to link program: " + n), e.deleteProgram(l), e.deleteShader(o), e.deleteShader(t), null } return l } function loadShader(e, r, a) { var t = e.createShader(r); if (null == t) return console.log("unable to create shader"), null; if (e.shaderSource(t, a), e.compileShader(t), !e.getShaderParameter(t, e.COMPILE_STATUS)) { var o = e.getShaderInfoLog(t); return console.log("Failed to compile shader: " + o), e.deleteShader(t), null } return t } function getWebGLContext(e, r) { var a = WebGLUtils.setupWebGL(e); return a ? ((arguments.length < 2 || r) && (a = WebGLDebugUtils.makeDebugContext(a)), a) : null }

function init(VSHADER_SOURCE, FSHADER_SOURCE) {
    if (!canvas) {
        console.log("failed to get canvas");
        return;
    }
    let gl = canvas.getContext('webgl');

    if (!gl) {
        console.log("Failed to get the rendring context for WebGl");
        return;
    }
    if (!initShaders(gl, VSHADER_SOURCE, FSHADER_SOURCE)) {
        console.log("Failed to init the shaders ");
        return;
    }

    let n = 4;

    let vertexTexCoords = new Float32Array([
        -0.5, 0.5, 0.0, 1.0,
        -0.5, -0.5, 0.0, 0.0,
        0.5, 0.5, 1.0, 1.0,
        0.5, -0.5, 1.0, 0.0
    ]);

    let vertexTexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexTexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertexTexCoords, gl.STATIC_DRAW);

    let FSIZE = vertexTexCoords.BYTES_PER_ELEMENT;

    let a_Position = gl.getAttribLocation(gl.program, 'a_Position');
    let a_TexCoord = gl.getAttribLocation(gl.program, 'a_TexCoord');

    gl.vertexAttribPointer(a_Position, 2, gl.FLOAT, false, FSIZE * 4, 0);
    gl.enableVertexAttribArray(a_Position);
    gl.vertexAttribPointer(a_TexCoord, 2, gl.FLOAT, false, FSIZE * 4, FSIZE * 2);
    gl.enableVertexAttribArray(a_TexCoord);

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    if (!initTexture(gl, n)) {

    }

}

function initTexture(gl, n) {
    let texture = gl.createTexture();

    let img = new Image();
    img.onload = () => {
        let u_Sampler = gl.getUniformLocation(gl.program, 'u_sampler');
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);

        //将已经创建好的纹理对象开启和纹理单元绑定，
        gl.bindTexture(gl.TEXTURE_2D, texture);
        //设置纹理的类型和参数
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        //绑定图像数据到纹理中
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, img);

        gl.uniform1i(u_Sampler, 0);
        gl.bindTexture(gl.TEXTURE_2D, null);
        let fbo = initFramebufferObject(gl);
        //开启fbo
        gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);              // Change the drawing destination to FBO
        gl.viewport(0, 0, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT); // Set a viewport for FBO

        gl.clearColor(0.2, 0.2, 0.4, 1.0); // Set clear color (the color is slightly changed)
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);  // Clear FBO

        //在fbo 中进行绘制 创建纹理单元
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.drawArrays(gl.TRIANGLE_STRIP, 0, n);

        //unbindfbo 在颜色缓冲区中进行绘制
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        gl.bindTexture(gl.TEXTURE_2D, null);
        gl.viewport(0, 0, 400, 400);  // Set the size of viewport back to that of <canvas>

        gl.clearColor(1.0, 1.0, 1.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        //传入刚刚在fbo中绘制好的纹理,进行绘制
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.drawArrays(gl.TRIANGLE_STRIP, 0, n);

    }
    img.src = "https://img.alicdn.com/tfs/TB1xmJJMEY1gK0jSZFMXXaWcVXa-256-256.jpg";
}

function initFramebufferObject(gl) {
    var framebuffer, texture, depthBuffer;

    // Define the error handling function
    var error = function () {
        if (framebuffer) gl.deleteFramebuffer(framebuffer);
        if (texture) gl.deleteTexture(texture);
        if (depthBuffer) gl.deleteRenderbuffer(depthBuffer);
        return null;
    }

    // Create a frame buffer object (FBO)
    framebuffer = gl.createFramebuffer();
    if (!framebuffer) {
        console.log('Failed to create frame buffer object');
        return error();
    }

    // Create a texture object and set its size and parameters
    texture = gl.createTexture(); // Create a texture object
    if (!texture) {
        console.log('Failed to create texture object');
        return error();
    }
    gl.bindTexture(gl.TEXTURE_2D, texture); // Bind the object to target
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    framebuffer.texture = texture; // Store the texture object

    // Create a renderbuffer object and Set its size and parameters
    depthBuffer = gl.createRenderbuffer(); // Create a renderbuffer object
    if (!depthBuffer) {
        console.log('Failed to create renderbuffer object');
        return error();
    }
    gl.bindRenderbuffer(gl.RENDERBUFFER, depthBuffer); // Bind the object to target
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT);

    // Attach the texture and the renderbuffer object to the FBO
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depthBuffer);

    // Check if FBO is configured correctly
    var e = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (gl.FRAMEBUFFER_COMPLETE !== e) {
        console.log('Frame buffer object is incomplete: ' + e.toString());
        return error();
    }

    // Unbind the buffer object
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindRenderbuffer(gl.RENDERBUFFER, null);
    return framebuffer;
}

init(VSHADER_SOURCE,FSHADER_SOURCE)
canvas.createPNG("drawfbo");