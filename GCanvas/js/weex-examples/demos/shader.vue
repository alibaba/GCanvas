<template>
  <div>
    <gcanvas v-if="isWeex" ref="canvas_holder" v-bind:style="{width:width,height:height,backgroundColor:'rgba(255,0,0,1)'}"></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" v-bind:style="{width:width+'px',height:height+'px',backgroundColor:'rgba(255,0,0,1)'}"></canvas>
  </div>
</template>

<script>
const isWeex = typeof WXEnvironment !== 'undefined';

const dom = weex.requireModule("dom");
import { enable, WeexBridge, Image as GImage } from "../../src/index.js";
import { compile } from "shader-program-compiler";

function run(canvas, { requestAnimationFrame }) {
  const fShader = `
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 uResolution;
uniform float uTime;

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(0.129898,0.78233)))* 
        43.7585453123);
}

float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + 
            (c - a)* u.y * (1.0 - u.x) + 
            (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5

float fbm ( in vec2 _st) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), 
                    -sin(0.5), cos(0.50));
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

void main() {
    vec2 st = gl_FragCoord.xy/uResolution.xy*3.;
    // st += st * abs(sin(uTime*0.1)*3.0);
    vec3 color = vec3(0.0);

    vec2 q = vec2(0.);
    q.x = fbm( st + 0.00*uTime);
    q.y = fbm( st + vec2(1.0));

    vec2 r = vec2(0.);
    r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*uTime );
    r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*uTime);

    float f = fbm(st+r);

    color = mix(vec3(0.101961,0.619608,0.666667),
                vec3(0.666667,0.666667,0.498039),
                clamp((f*f)*4.0,0.0,1.0));

    color = mix(color,
                vec3(0,0,0.164706),
                clamp(length(q),0.0,1.0));

    color = mix(color,
                vec3(0.666667,1,1),
                clamp(length(r.x),0.0,1.0));

    gl_FragColor = vec4((f*f*f+.6*f*f+.5*f)*color,1.);
}
`;

  runFragShader(canvas, fShader);

  function runFragShader(canvas, fShader) {
    const vShader = `
        attribute vec2 aPosition;
        void main() {
          gl_Position = vec4(aPosition, 0, 1);
        }`;

    const gl = canvas.getContext("webgl");
    gl.getExtension("OES_standard_derivatives");

    const {
      attributes,
      uniforms,
      fillElements,
      drawElements,
      createElementsBuffer
    } = compile({
      vShader,
      fShader,
      gl
    });

    fillElements(createElementsBuffer([0, 1, 2]));
    attributes.aPosition.fill(
      attributes.aPosition.createBuffer([0, 3, 3, -3, -3, -3])
    );

    let offsetX = 0;
    let offsetY = 0;

    uniforms.uResolution &&
      uniforms.uResolution.fill([canvas.width, canvas.height]);

    let startTime;
    function render() {
      gl.clearColor(0.0, 0.0, 0.0, 1.0);
      gl.clear(gl.COLOR_BUFFER_BIT);

      if (uniforms.uTime) {
        if (!startTime) {
          startTime = Date.now();
        }
        const uTime = ((Date.now() - startTime) / 1000) % 65532;
        uniforms.uTime.fill([uTime]);
      }
      if (uniforms.uMouse) {
        uniforms.uMouse.fill([offsetX, offsetY]);
      }

      drawElements(3);

      requestAnimationFrame(render);
    }
    render();
  }
}

export default {
  data() {
    return {
      isWeex,
      width: 750,
      height: 3000
    };
  },

  mounted: function() {

    const start = () => {

      var ref = this.$refs.canvas_holder;

      var size = isWeex
        ? {
            width: parseInt(this.width),
            height: parseInt(this.height)
          }
        : {
            width: parseInt(ref.style.width),
            height: parseInt(ref.style.height)
          };

      if (isWeex) {
        ref = enable(ref, { debug: true, bridge: WeexBridge });
      }

      ref.width = size.width;
      ref.height = size.height;

      run(ref, {
        requestAnimationFrame: isWeex ? setTimeout : requestAnimationFrame
      });
    };

    dom.getComponentRect("viewport", e => {
      this.height = e.size.height;

      this.isReady = true;

      setTimeout(start, 1000);
    });
  }
};
</script>