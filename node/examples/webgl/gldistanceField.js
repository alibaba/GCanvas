const { createCanvas, Image } = require('../../export')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..","..")+ '/distanceField.png');

const canvas = createCanvas(900, 2100);
var gl = canvas.getContext("webgl");

var vs_source = "attribute vec2 aVertexPosition;\
void main()\
{\
   gl_Position = vec4(aVertexPosition, 0.0, 1.0);\
}";

var fs_source = "precision highp float;\
\
uniform float time;\
uniform vec2 resolution;\
uniform vec3 cameraPos;\
uniform vec3 cameraLookat;\
uniform vec3 lightDir;\
uniform vec3 lightColour;\
uniform float specular;\
uniform float specularHardness;\
uniform vec3 diffuse;\
uniform float ambientFactor;\
uniform bool ao;\
uniform bool shadows;\
uniform bool postEffects;\
uniform bool rotateWorld;\
uniform bool moveCamera;\
uniform bool antialias;\
\
const float PI = 3.14159265;\
const float GAMMA = 0.8;\
const float CONTRAST = 1.1;\
const float SATURATION = 1.3;\
const float BRIGHTNESS = 1.3;\
const int AO_SAMPLES = 5;\
const int RAY_DEPTH = 256;\
const float MAX_DEPTH = 100.0;\
const int SHADOW_RAY_DEPTH = 32;\
const float DISTANCE_MIN = 0.001;\
\
const vec2 delta = vec2(0.001, 0.);\
\
\
vec3 RotateZ(vec3 p, float a)\
{\
   float c,s;\
   vec3 q=p;\
   c = cos(a);\
   s = sin(a);\
   p.x = c * q.x - s * q.y;\
   p.y = s * q.x + c * q.y;\
   return p;\
}\
\
float smax(float a, float b, float k)\
{\
   float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);\
   return mix(a, b, h) - k*h*h;\
}\
\
float Sphere(vec3 p, float s)\
{\
   return length(p)-s;\
}\
\
float HexPrism(vec3 p, vec2 h)\
{\
   vec3 q = abs(p);\
   return max(q.z-h.y,max(q.x+q.y*0.57735,q.y*1.1547)-h.x);\
}\
\
float Plane(vec3 p, vec3 n)\
{\
   return dot(p, n);\
}\
\
float ReplicateXZ(vec3 p, vec3 c)\
{\
   vec3 q = vec3(mod(p.x, c.x) - 0.5 * c.x, p.y, mod(p.z, c.z) - 0.5 * c.z);\
   return\
      min(\
         Plane(q-vec3(0.,-0.85,0.), vec3(0.,1.,0.)),\
\
         smax(\
\
            max(\
               HexPrism(q, vec2(1.0,0.5)),\
               -HexPrism(q-vec3(0.,0.,0.25), vec2(0.8,1.0))\
            ),\
\
            -min(\
               Sphere(q-vec3(0.,0.85,0.), 0.333),\
               min(\
                  Sphere(q-vec3(0.,-0.85,0.), 0.333),\
                  min(\
                     Sphere(q-vec3(0.75,0.45,0.), 0.333),\
                     min(\
                        Sphere(q-vec3(0.75,-0.45,0.), 0.333),\
                        min(\
                           Sphere(q-vec3(-0.75,0.45,0.), 0.333),\
                           Sphere(q-vec3(-0.75,-0.45,0.), 0.333)\
                        )\
                     )\
                  )\
               )\
            ),0.05\
         )\
      );\
}\
\
float Dist(vec3 pos)\
{\
   if (rotateWorld) pos = RotateZ(pos, sin(time)*0.5);\
   return ReplicateXZ(pos, vec3(4.,0.,4.));\
}\
\
float CalcAO(vec3 p, vec3 n)\
{\
   float r = 0.0;\
   float w = 1.0;\
   for (int i=1; i<=AO_SAMPLES; i++)\
   {\
      float d0 = float(i) * 0.3;\
      r += w * (d0 - Dist(p + n * d0));\
      w *= 0.5;\
   }\
   return 1.0 - r;\
}\
\
float SoftShadow(vec3 ro, vec3 rd, float k)\
{\
   float res = 1.0;\
   float t = 0.05;\
   for (int i=0; i<SHADOW_RAY_DEPTH; i++)\
   {\
      if (t < 20.0)\
      {\
         float h = Dist(ro + rd * t);\
         res = min(res, k*h/t);\
         t += h;\
      }\
   }\
   return clamp(res, 0.0, 1.0);\
}\
\
vec3 GetNormal(vec3 pos)\
{\
   vec3 n;\
   n.x = Dist( pos + delta.xyy ) - Dist( pos - delta.xyy );\
   n.y = Dist( pos + delta.yxy ) - Dist( pos - delta.yxy );\
   n.z = Dist( pos + delta.yyx ) - Dist( pos - delta.yyx );\
   \
   return normalize(n);\
}\
\
vec3 Sky(in vec3 rd)\
{\
   float sunAmount = max(dot(rd, lightDir), 0.0);\
   float v = pow(1.0 - max(rd.y,0.0),6.);\
   vec3 sky = mix(vec3(.1, .2, .3), vec3(.32, .32, .32), v);\
   sky += lightColour * sunAmount * sunAmount * .25 + lightColour * min(pow(sunAmount, 800.0)*1.5, .3);\
   return sky;\
}\
\
vec3 Fog(vec3 rgb, vec3 rd, float distance)\
{\
   const float b = 0.185;\
   float fogAmount = 1.0 - exp(-distance*b*b);\
   vec3 fogColor = Sky(rd);\
   return mix(rgb, fogColor, fogAmount);\
}\
\
vec4 Shading(vec3 pos, vec3 rd, vec3 norm, vec3 ro)\
{\
   vec3 heading = normalize(-rd + lightDir);\
   float spec = pow(max(0.0, dot(heading, norm)), specularHardness);\
   \
   float sun = clamp(dot(norm, lightDir), 0.0, 1.0);\
   vec3 light = (sun * lightColour * diffuse) + (spec * specular * lightColour);\
   \
   if (shadows) light *= SoftShadow(pos, lightDir, 32.0);\
   if (ao) light += CalcAO(pos, norm) * ambientFactor;\
   \
   light = Fog(light, rd, length(ro-pos));\
   \
   return vec4(light, 1.0);\
}\
\
vec3 PostEffects(vec3 rgb, vec2 xy)\
{\
	rgb = pow(rgb, vec3(GAMMA));\
	rgb = mix(vec3(.5), mix(vec3(dot(vec3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);\
	rgb *= .4+0.5*pow(40.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), 0.2 );	\
	return rgb;\
}\
\
vec3 GetRay(vec3 dir, vec2 pos)\
{\
   pos = pos - 0.5;\
   pos.x *= resolution.x/resolution.y;\
   \
   dir = normalize(dir);\
   vec3 right = normalize(cross(vec3(0.,1.,0.),dir));\
   vec3 up = normalize(cross(dir,right));\
   \
   return dir + right*pos.x + up*pos.y;\
}\
\
vec4 March(vec3 ro, vec3 rd)\
{\
   float t = 0.0;\
   float d = 1.0;\
   for (int i=0; i<RAY_DEPTH; i++)\
   {\
      vec3 p = ro + rd * t;\
      d = Dist(p);\
      if (abs(d) < DISTANCE_MIN)\
      {\
         return vec4(p, 1.0);\
      }\
      t += d;\
      if (t >= MAX_DEPTH) break;\
   }\
   return vec4(0.0);\
}\
\
void main()\
{\
   const int ANTIALIAS_SAMPLES = 4;\
   \
   vec4 res = vec4(0.0);\
   \
   vec3 off = vec3(0.0);\
   if (moveCamera) off.z -= time*10.0;\
   \
   if (antialias)\
   {\
      vec2 p;\
      float d_ang = 2.*PI / float(ANTIALIAS_SAMPLES);\
      float ang = d_ang * 0.33333;\
      float r = 0.3;\
      for (int i = 0; i < ANTIALIAS_SAMPLES; i++)\
      {\
         p = vec2((gl_FragCoord.x + cos(ang)*r) / resolution.x, (gl_FragCoord.y + sin(ang)*r) / resolution.y);\
         vec3 ro = cameraPos + off;\
         vec3 rd = normalize(GetRay(cameraLookat-cameraPos, p));\
         vec4 _res = March(ro, rd);\
         if (_res.a == 1.0) res.rgb += Shading(_res.rgb, rd, GetNormal(_res.rgb), ro).rgb;\
         else res.rgb += Sky(rd);\
         ang += d_ang;\
      }\
      res.rgb /= float(ANTIALIAS_SAMPLES);\
      if (postEffects) res.rgb = PostEffects(res.rgb, p);\
   }\
   else\
   {\
      vec2 p = gl_FragCoord.xy / resolution.xy;\
      vec3 ro = cameraPos + off;\
      vec3 rd = normalize(GetRay((cameraLookat-off)-cameraPos+off, p));\
      \
      res = March(ro, rd);\
      if (res.a == 1.0) res.rgb = Shading(res.rgb, rd, GetNormal(res.rgb), ro).rgb;\
      else res.rgb = Sky(rd);\
      if (postEffects) res.rgb = PostEffects(res.rgb, p);\
   }\
   \
   gl_FragColor = vec4(res.rgb, 1.0);\
}";


var config = {
   camera: {
      x: 9.0, y: 4.5, z: 0.0
   },
   lookat: {
      x: 0.0, y: -10.0, z: -100.0
   },
   lightDir: {
      x: -1.4, y: 0.8, z: -1.0
   },
   lightColour: {
      r: 3.0, g: 1.4, b: 0.3
   },
   surface: {
      specular: 64.0,
      specularHardness: 256.0,
      diffuse: 0.4,
      ambientFactor: 0.2
   },
   global: {
      ao: true,
      shadows: true,
      postEffects: true,
      antialias: "None",// None|Classic
      rotateWorld: true,
      moveCamera: true
   }
};

var pause = false;
var aspect;
function init() {
  aspect = canvas.width / canvas.height;

  console.log('canvas', canvas.width, canvas.height);

  gl.viewport(0, 0, canvas.width, canvas.height);
  gl.clearColor(0, 0, 0, 1);
  gl.clear(gl.COLOR_BUFFER_BIT);

  // compile and link the shaders
  var vs = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vs, vs_source);
  gl.compileShader(vs);

  var fs = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(fs, fs_source);
  gl.compileShader(fs);

  var program = gl.createProgram();
  gl.attachShader(program, vs);
  gl.attachShader(program, fs);
  gl.linkProgram(program);

  // debug shader compile status
  var error = false;
  if (!gl.getShaderParameter(vs, gl.COMPILE_STATUS)) {
    error = true;
    console.log(gl.getShaderInfoLog(vs));
  }

  if (!gl.getShaderParameter(fs, gl.COMPILE_STATUS)) {
    error = true;
    console.log(gl.getShaderInfoLog(fs));
  }

  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    error = true;
    console.log(gl.getProgramInfoLog(program));
  }
  if (error) return;

  var firstTime = Date.now();

  function render() {
    if (!pause) {
        //stats.begin();

      // create vertices to fill the canvas with a single quad
         var vertices = new Float32Array(
            [
               -1, 1*aspect, 1, 1*aspect, 1, -1*aspect,
               -1, 1*aspect, 1, -1*aspect, -1, -1*aspect
            ]);

         var vbuffer = gl.createBuffer();
         gl.bindBuffer(gl.ARRAY_BUFFER, vbuffer);
         gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);

         var triCount = 2,
             numItems = vertices.length / triCount;

         gl.useProgram(program);

         var time = (Date.now() - firstTime) / 1000.0;
         program.time = gl.getUniformLocation(program, "time");
         gl.uniform1f(program.time, time);

         program.resolution = gl.getUniformLocation(program, "resolution");
         gl.uniform2f(program.resolution, canvas.width, canvas.height);

         program.cameraPos = gl.getUniformLocation(program, "cameraPos");
         gl.uniform3f(program.cameraPos, config.camera.x, config.camera.y, config.camera.z);

         program.cameraLookat = gl.getUniformLocation(program, "cameraLookat");
         gl.uniform3f(program.cameraLookat, config.lookat.x, config.lookat.y, config.lookat.z);

         program.lightDir = gl.getUniformLocation(program, "lightDir");
         // pre normalise light dir
         var x = config.lightDir.x, y = config.lightDir.y, z = config.lightDir.z;
         var len = x*x + y*y + z*z;
         len = 1.0 / Math.sqrt(len);
         gl.uniform3f(program.lightDir, config.lightDir.x*len, config.lightDir.y*len, config.lightDir.z*len);

         program.lightColour = gl.getUniformLocation(program, "lightColour");
         gl.uniform3f(program.lightColour, config.lightColour.r, config.lightColour.g, config.lightColour.b);

         program.specular = gl.getUniformLocation(program, "specular");
         gl.uniform1f(program.specular, config.surface.specular);

         program.specularHardness = gl.getUniformLocation(program, "specularHardness");
         gl.uniform1f(program.specularHardness, config.surface.specularHardness);

         program.diffuse = gl.getUniformLocation(program, "diffuse");
         gl.uniform3f(program.diffuse, config.surface.diffuse,config.surface.diffuse,config.surface.diffuse);

         program.ambientFactor = gl.getUniformLocation(program, "ambientFactor");
         gl.uniform1f(program.ambientFactor, config.surface.ambientFactor);

         program.rotateWorld = gl.getUniformLocation(program, "rotateWorld");
         gl.uniform1f(program.rotateWorld, config.global.rotateWorld);

         program.moveCamera = gl.getUniformLocation(program, "moveCamera");
         gl.uniform1f(program.moveCamera, config.global.moveCamera);

         program.postEffects = gl.getUniformLocation(program, "postEffects");
         gl.uniform1f(program.postEffects, config.global.postEffects);

         program.ao = gl.getUniformLocation(program, "ao");
         gl.uniform1f(program.ao, config.global.ao);

         program.shadows = gl.getUniformLocation(program, "shadows");
         gl.uniform1f(program.shadows, config.global.shadows);

         program.antialias = gl.getUniformLocation(program, "antialias");
         gl.uniform1f(program.antialias, (config.global.antialias === "Classic"));

         program.aVertexPosition = gl.getAttribLocation(program, "aVertexPosition");
         gl.enableVertexAttribArray(program.aVertexPosition);
         gl.vertexAttribPointer(program.aVertexPosition, triCount, gl.FLOAT, false, 0, 0);
            
         gl.drawArrays(gl.TRIANGLES, 0, numItems);

         //stats.end();

         //canvasLog("iterate frame....");
        //  canvas.requestAnimationFrame(render);
    }
  };
  render();
}

console.log("init ...");
init();


var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
      out.write(chunk);
});

//   canvas.requestAnimationFrame(render);