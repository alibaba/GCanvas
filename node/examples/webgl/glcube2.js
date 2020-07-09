

const { createCanvas, Image } = require('../../export')
let canvas = createCanvas(400, 400);
const VSHADER_SOURCE = "attribute vec4 a_Position;"+
       "attribute vec4 a_Color;"+
     "uniform mat4 u_viewMatrix;"+
     "uniform mat4 u_projMatrix;"+
     "varying vec4 v_Color;"+
     "void main(){"+
     "gl_Position=u_projMatrix*u_viewMatrix*a_Position;"+
     "v_Color=a_Color;"+
    "}"


const FSHADER_SOURCE = "precision mediump float;\n" +
    "varying vec4 v_Color;\n" +
    "void main(){\n" +
    "gl_FragColor=v_Color;\n" +
    "}\n";

var Matrix4=function(t){var e,r,n;if(t&&"object"==typeof t&&t.hasOwnProperty("elements")){for(r=t.elements,n=new Float32Array(16),e=0;e<16;++e)n[e]=r[e];this.elements=n}else this.elements=new Float32Array([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])};Matrix4.prototype.setIdentity=function(){var t=this.elements;return t[0]=1,t[4]=0,t[8]=0,t[12]=0,t[1]=0,t[5]=1,t[9]=0,t[13]=0,t[2]=0,t[6]=0,t[10]=1,t[14]=0,t[3]=0,t[7]=0,t[11]=0,t[15]=1,this},Matrix4.prototype.set=function(t){var e,r,n;if((r=t.elements)!==(n=this.elements)){for(e=0;e<16;++e)n[e]=r[e];return this}},Matrix4.prototype.concat=function(t){var e,r,n,o,i,s,a,u;if(r=this.elements,n=this.elements,r===(o=t.elements))for(o=new Float32Array(16),e=0;e<16;++e)o[e]=r[e];for(e=0;e<4;e++)i=n[e],s=n[e+4],a=n[e+8],u=n[e+12],r[e]=i*o[0]+s*o[1]+a*o[2]+u*o[3],r[e+4]=i*o[4]+s*o[5]+a*o[6]+u*o[7],r[e+8]=i*o[8]+s*o[9]+a*o[10]+u*o[11],r[e+12]=i*o[12]+s*o[13]+a*o[14]+u*o[15];return this},Matrix4.prototype.multiply=Matrix4.prototype.concat,Matrix4.prototype.multiplyVector3=function(t){var e=this.elements,r=t.elements,n=new Vector3,o=n.elements;return o[0]=r[0]*e[0]+r[1]*e[4]+r[2]*e[8]+e[12],o[1]=r[0]*e[1]+r[1]*e[5]+r[2]*e[9]+e[13],o[2]=r[0]*e[2]+r[1]*e[6]+r[2]*e[10]+e[14],n},Matrix4.prototype.multiplyVector4=function(t){var e=this.elements,r=t.elements,n=new Vector4,o=n.elements;return o[0]=r[0]*e[0]+r[1]*e[4]+r[2]*e[8]+r[3]*e[12],o[1]=r[0]*e[1]+r[1]*e[5]+r[2]*e[9]+r[3]*e[13],o[2]=r[0]*e[2]+r[1]*e[6]+r[2]*e[10]+r[3]*e[14],o[3]=r[0]*e[3]+r[1]*e[7]+r[2]*e[11]+r[3]*e[15],n},Matrix4.prototype.transpose=function(){var t,e;return e=(t=this.elements)[1],t[1]=t[4],t[4]=e,e=t[2],t[2]=t[8],t[8]=e,e=t[3],t[3]=t[12],t[12]=e,e=t[6],t[6]=t[9],t[9]=e,e=t[7],t[7]=t[13],t[13]=e,e=t[11],t[11]=t[14],t[14]=e,this},Matrix4.prototype.setInverseOf=function(t){var e,r,n,o,i;if(r=t.elements,n=this.elements,(o=new Float32Array(16))[0]=r[5]*r[10]*r[15]-r[5]*r[11]*r[14]-r[9]*r[6]*r[15]+r[9]*r[7]*r[14]+r[13]*r[6]*r[11]-r[13]*r[7]*r[10],o[4]=-r[4]*r[10]*r[15]+r[4]*r[11]*r[14]+r[8]*r[6]*r[15]-r[8]*r[7]*r[14]-r[12]*r[6]*r[11]+r[12]*r[7]*r[10],o[8]=r[4]*r[9]*r[15]-r[4]*r[11]*r[13]-r[8]*r[5]*r[15]+r[8]*r[7]*r[13]+r[12]*r[5]*r[11]-r[12]*r[7]*r[9],o[12]=-r[4]*r[9]*r[14]+r[4]*r[10]*r[13]+r[8]*r[5]*r[14]-r[8]*r[6]*r[13]-r[12]*r[5]*r[10]+r[12]*r[6]*r[9],o[1]=-r[1]*r[10]*r[15]+r[1]*r[11]*r[14]+r[9]*r[2]*r[15]-r[9]*r[3]*r[14]-r[13]*r[2]*r[11]+r[13]*r[3]*r[10],o[5]=r[0]*r[10]*r[15]-r[0]*r[11]*r[14]-r[8]*r[2]*r[15]+r[8]*r[3]*r[14]+r[12]*r[2]*r[11]-r[12]*r[3]*r[10],o[9]=-r[0]*r[9]*r[15]+r[0]*r[11]*r[13]+r[8]*r[1]*r[15]-r[8]*r[3]*r[13]-r[12]*r[1]*r[11]+r[12]*r[3]*r[9],o[13]=r[0]*r[9]*r[14]-r[0]*r[10]*r[13]-r[8]*r[1]*r[14]+r[8]*r[2]*r[13]+r[12]*r[1]*r[10]-r[12]*r[2]*r[9],o[2]=r[1]*r[6]*r[15]-r[1]*r[7]*r[14]-r[5]*r[2]*r[15]+r[5]*r[3]*r[14]+r[13]*r[2]*r[7]-r[13]*r[3]*r[6],o[6]=-r[0]*r[6]*r[15]+r[0]*r[7]*r[14]+r[4]*r[2]*r[15]-r[4]*r[3]*r[14]-r[12]*r[2]*r[7]+r[12]*r[3]*r[6],o[10]=r[0]*r[5]*r[15]-r[0]*r[7]*r[13]-r[4]*r[1]*r[15]+r[4]*r[3]*r[13]+r[12]*r[1]*r[7]-r[12]*r[3]*r[5],o[14]=-r[0]*r[5]*r[14]+r[0]*r[6]*r[13]+r[4]*r[1]*r[14]-r[4]*r[2]*r[13]-r[12]*r[1]*r[6]+r[12]*r[2]*r[5],o[3]=-r[1]*r[6]*r[11]+r[1]*r[7]*r[10]+r[5]*r[2]*r[11]-r[5]*r[3]*r[10]-r[9]*r[2]*r[7]+r[9]*r[3]*r[6],o[7]=r[0]*r[6]*r[11]-r[0]*r[7]*r[10]-r[4]*r[2]*r[11]+r[4]*r[3]*r[10]+r[8]*r[2]*r[7]-r[8]*r[3]*r[6],o[11]=-r[0]*r[5]*r[11]+r[0]*r[7]*r[9]+r[4]*r[1]*r[11]-r[4]*r[3]*r[9]-r[8]*r[1]*r[7]+r[8]*r[3]*r[5],o[15]=r[0]*r[5]*r[10]-r[0]*r[6]*r[9]-r[4]*r[1]*r[10]+r[4]*r[2]*r[9]+r[8]*r[1]*r[6]-r[8]*r[2]*r[5],0===(i=r[0]*o[0]+r[1]*o[4]+r[2]*o[8]+r[3]*o[12]))return this;for(i=1/i,e=0;e<16;e++)n[e]=o[e]*i;return this},Matrix4.prototype.invert=function(){return this.setInverseOf(this)},Matrix4.prototype.setOrtho=function(t,e,r,n,o,i){var s,a,u,h;if(t===e||r===n||o===i)throw"null frustum";return a=1/(e-t),u=1/(n-r),h=1/(i-o),(s=this.elements)[0]=2*a,s[1]=0,s[2]=0,s[3]=0,s[4]=0,s[5]=2*u,s[6]=0,s[7]=0,s[8]=0,s[9]=0,s[10]=-2*h,s[11]=0,s[12]=-(e+t)*a,s[13]=-(n+r)*u,s[14]=-(i+o)*h,s[15]=1,this},Matrix4.prototype.ortho=function(t,e,r,n,o,i){return this.concat((new Matrix4).setOrtho(t,e,r,n,o,i))},Matrix4.prototype.setFrustum=function(t,e,r,n,o,i){var s,a,u,h;if(t===e||n===r||o===i)throw"null frustum";if(o<=0)throw"near <= 0";if(i<=0)throw"far <= 0";return a=1/(e-t),u=1/(n-r),h=1/(i-o),(s=this.elements)[0]=2*o*a,s[1]=0,s[2]=0,s[3]=0,s[4]=0,s[5]=2*o*u,s[6]=0,s[7]=0,s[8]=(e+t)*a,s[9]=(n+r)*u,s[10]=-(i+o)*h,s[11]=-1,s[12]=0,s[13]=0,s[14]=-2*o*i*h,s[15]=0,this},Matrix4.prototype.frustum=function(t,e,r,n,o,i){return this.concat((new Matrix4).setFrustum(t,e,r,n,o,i))},Matrix4.prototype.setPerspective=function(t,e,r,n){var o,i,s,a;if(r===n||0===e)throw"null frustum";if(r<=0)throw"near <= 0";if(n<=0)throw"far <= 0";if(t=Math.PI*t/180/2,0===(s=Math.sin(t)))throw"null frustum";return i=1/(n-r),a=Math.cos(t)/s,(o=this.elements)[0]=a/e,o[1]=0,o[2]=0,o[3]=0,o[4]=0,o[5]=a,o[6]=0,o[7]=0,o[8]=0,o[9]=0,o[10]=-(n+r)*i,o[11]=-1,o[12]=0,o[13]=0,o[14]=-2*r*n*i,o[15]=0,this},Matrix4.prototype.perspective=function(t,e,r,n){return this.concat((new Matrix4).setPerspective(t,e,r,n))},Matrix4.prototype.setScale=function(t,e,r){var n=this.elements;return n[0]=t,n[4]=0,n[8]=0,n[12]=0,n[1]=0,n[5]=e,n[9]=0,n[13]=0,n[2]=0,n[6]=0,n[10]=r,n[14]=0,n[3]=0,n[7]=0,n[11]=0,n[15]=1,this},Matrix4.prototype.scale=function(t,e,r){var n=this.elements;return n[0]*=t,n[4]*=e,n[8]*=r,n[1]*=t,n[5]*=e,n[9]*=r,n[2]*=t,n[6]*=e,n[10]*=r,n[3]*=t,n[7]*=e,n[11]*=r,this},Matrix4.prototype.setTranslate=function(t,e,r){var n=this.elements;return n[0]=1,n[4]=0,n[8]=0,n[12]=t,n[1]=0,n[5]=1,n[9]=0,n[13]=e,n[2]=0,n[6]=0,n[10]=1,n[14]=r,n[3]=0,n[7]=0,n[11]=0,n[15]=1,this},Matrix4.prototype.translate=function(t,e,r){var n=this.elements;return n[12]+=n[0]*t+n[4]*e+n[8]*r,n[13]+=n[1]*t+n[5]*e+n[9]*r,n[14]+=n[2]*t+n[6]*e+n[10]*r,n[15]+=n[3]*t+n[7]*e+n[11]*r,this},Matrix4.prototype.setRotate=function(t,e,r,n){var o,i,s,a,u,h,p,c,l,f,m,M;return t=Math.PI*t/180,o=this.elements,i=Math.sin(t),s=Math.cos(t),0!==e&&0===r&&0===n?(e<0&&(i=-i),o[0]=1,o[4]=0,o[8]=0,o[12]=0,o[1]=0,o[5]=s,o[9]=-i,o[13]=0,o[2]=0,o[6]=i,o[10]=s,o[14]=0,o[3]=0,o[7]=0,o[11]=0):0===e&&0!==r&&0===n?(r<0&&(i=-i),o[0]=s,o[4]=0,o[8]=i,o[12]=0,o[1]=0,o[5]=1,o[9]=0,o[13]=0,o[2]=-i,o[6]=0,o[10]=s,o[14]=0,o[3]=0,o[7]=0,o[11]=0):0===e&&0===r&&0!==n?(n<0&&(i=-i),o[0]=s,o[4]=-i,o[8]=0,o[12]=0,o[1]=i,o[5]=s,o[9]=0,o[13]=0,o[2]=0,o[6]=0,o[10]=1,o[14]=0,o[3]=0,o[7]=0,o[11]=0):(1!==(a=Math.sqrt(e*e+r*r+n*n))&&(e*=u=1/a,r*=u,n*=u),h=1-s,p=e*r,c=r*n,l=n*e,f=e*i,m=r*i,M=n*i,o[0]=e*e*h+s,o[1]=p*h+M,o[2]=l*h-m,o[3]=0,o[4]=p*h-M,o[5]=r*r*h+s,o[6]=c*h+f,o[7]=0,o[8]=l*h+m,o[9]=c*h-f,o[10]=n*n*h+s,o[11]=0,o[12]=0,o[13]=0,o[14]=0),o[15]=1,this},Matrix4.prototype.rotate=function(t,e,r,n){return this.concat((new Matrix4).setRotate(t,e,r,n))},Matrix4.prototype.setLookAt=function(t,e,r,n,o,i,s,a,u){var h,p,c,l,f,m,M,y,x,v,w,A;return p=n-t,c=o-e,l=i-r,m=(c*=f=1/Math.sqrt(p*p+c*c+l*l))*u-(l*=f)*a,M=l*s-(p*=f)*u,y=p*a-c*s,v=(M*=x=1/Math.sqrt(m*m+M*M+y*y))*l-(y*=x)*c,w=y*p-(m*=x)*l,A=m*c-M*p,(h=this.elements)[0]=m,h[1]=v,h[2]=-p,h[3]=0,h[4]=M,h[5]=w,h[6]=-c,h[7]=0,h[8]=y,h[9]=A,h[10]=-l,h[11]=0,h[12]=0,h[13]=0,h[14]=0,h[15]=1,this.translate(-t,-e,-r)},Matrix4.prototype.lookAt=function(t,e,r,n,o,i,s,a,u){return this.concat((new Matrix4).setLookAt(t,e,r,n,o,i,s,a,u))},Matrix4.prototype.dropShadow=function(t,e){var r=new Matrix4,n=r.elements,o=t[0]*e[0]+t[1]*e[1]+t[2]*e[2]+t[3]*e[3];return n[0]=o-e[0]*t[0],n[1]=-e[1]*t[0],n[2]=-e[2]*t[0],n[3]=-e[3]*t[0],n[4]=-e[0]*t[1],n[5]=o-e[1]*t[1],n[6]=-e[2]*t[1],n[7]=-e[3]*t[1],n[8]=-e[0]*t[2],n[9]=-e[1]*t[2],n[10]=o-e[2]*t[2],n[11]=-e[3]*t[2],n[12]=-e[0]*t[3],n[13]=-e[1]*t[3],n[14]=-e[2]*t[3],n[15]=o-e[3]*t[3],this.concat(r)},Matrix4.prototype.dropShadowDirectionally=function(t,e,r,n,o,i,s,a,u){var h=n*t+o*e+i*r;return this.dropShadow([t,e,r,-h],[s,a,u,0])};var Vector3=function(t){var e=new Float32Array(3);t&&"object"==typeof t&&(e[0]=t[0],e[1]=t[1],e[2]=t[2]),this.elements=e};Vector3.prototype.normalize=function(){var t=this.elements,e=t[0],r=t[1],n=t[2],o=Math.sqrt(e*e+r*r+n*n);return o?1==o||(o=1/o,t[0]=e*o,t[1]=r*o,t[2]=n*o):(t[0]=0,t[1]=0,t[2]=0),this};var Vector4=function(t){var e=new Float32Array(4);t&&"object"==typeof t&&(e[0]=t[0],e[1]=t[1],e[2]=t[2],e[3]=t[3]),this.elements=e};

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
    let n = prepare(gl);
    draw(gl, n);
}

function prepare(gl) {
    let vertices = new Float32Array([
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, //v0 white
        -1.0, 1.0, 1.0, 1.0, 0.0, 1.0, //v1 品红
        -1.0, -1.0, 1.0, 1.0, 0.0, 0.0, //v2 红色
        1.0, -1.0, 1.0, 1.0, 1.0, 0.0, //v3 黄色
        1.0, -1.0, -1.0, 0.0, 1.0, 0.0, //v4
        1.0, 1.0, -1.0, 0.0, 1.0, 1.0, //v5
        -1.0, 1.0, -1.0, 0.0, 0.0, 1.0, //v6
        -1.0, -1.0, -1.0, 0.0, 0.0, 0.0 //v7 
    ]);

    var indices = new Uint8Array([
        0, 1, 2, 0, 2, 3, //front
        0, 3, 4, 0, 4, 5, //right
        0, 5, 6, 0, 6, 1, //up
        1, 6, 7, 1, 7, 2, //left
        7, 4, 3, 7, 3, 2, //bottom
        4, 7, 6, 4, 6, 5 //behind
    ]);
    let FSIZE = vertices.BYTES_PER_ELEMENT;
    let vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
    let a_Position = gl.getAttribLocation(gl.program, 'a_Position');
    gl.vertexAttribPointer(a_Position, 3, gl.FLOAT, false, FSIZE * 6, 0);
    gl.enableVertexAttribArray(a_Position);
    let a_Color = gl.getAttribLocation(gl.program, 'a_Color');
    gl.vertexAttribPointer(a_Color, 3, gl.FLOAT, false, FSIZE * 6, FSIZE * 3);
    gl.enableVertexAttribArray(a_Color);

    let indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);


    gl.clearColor(0.3, 0.3, 0.3, 1.0);
    gl.enable(gl.DEPTH_TEST);
    let viewMatrix = new Matrix4();
    viewMatrix.setLookAt(15, 10, 15, 0, 0, 0, 0, 0, 0);
    let projMatrix = new Matrix4();
    projMatrix.setPerspective(30.0, 1, 1, 100);
    let u_viewMatrix = gl.getUniformLocation(gl.program, 'u_viewMatrix');
    gl.uniformMatrix4fv(u_viewMatrix, false, viewMatrix.elements);
    let u_projMatrix = gl.getUniformLocation(gl.program, 'u_projMatrix');
    gl.uniformMatrix4fv(u_projMatrix, false, projMatrix.elements);
    return indices.length;
}

function draw(gl, n) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.drawElements(gl.TRIANGLES, n, gl.UNSIGNED_BYTE, 0);
}

init(VSHADER_SOURCE, FSHADER_SOURCE)
canvas.createPNG("cube2")