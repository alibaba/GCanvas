

<template>
  <div>
    <gcanvas v-if="isWeex" ref="canvas_holder" style="width:750;height:750;background-color:rgba(255,0,0,1);"></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;"></canvas>
    <text class="text">isWeex0:{{isWeex}}</text>
  </div>
</template>
<style>
.text {
  font-size: 20;
}
</style>
<script>
const isWeex = typeof callNative === "function";
import { compile } from "./compile-shader";
import hackLog from "./hack-log";

import { enable, WeexBridge, Image as GImage } from "../src/index.js";
const EnvImage = isWeex ? GImage : Image;

function loadImage(src, callback) {
  const image = new EnvImage();
  image.crossOrigin = true;
  image.onload = function() {
    callback(image);
  };
  image.src = src;
}

function start(ref, image) {
  var gl = ref.getContext("webgl");

  // hackLog(gl);

  function initFramebuffer() {
    const vShader = `
    attribute vec2 aPosition;
    attribute vec2 aTexCoord;
    varying vec2 vTexCoord;
    void main() {
      gl_Position = vec4(aPosition, 0, 1);
      vTexCoord = aTexCoord;
    }`;
    const fShader = `
    precision mediump float;
    uniform sampler2D uSample;
    varying vec2 vTexCoord;
    uniform float uTime;
    void main() {
      gl_FragColor = texture2D(uSample, fract( vTexCoord + uTime/100.0));
    }`;

    const {
      program,
      attributes,
      uniforms,
      fillElements,
      drawArrays,
      drawElements,
      createFramebuffer,
      createElementsBuffer
    } = compile({
      vShader,
      fShader,
      gl
    });

    const { framebuffer, texture } = createFramebuffer(64, 64);
    // fillElements(createElementsBuffer([0, 1, 2, 0, 2, 3]));
    // const aPositionBuffer = attributes.aPosition.createBuffer([-1, 1, -1, -1, 1, -1, 1, 1]);
    // const aTexCoordBuffer = attributes.aTexCoord.createBuffer([0, 1, 0, 0, 1, 0, 1, 1]);
    // const uSampleTexture = uniforms.uSample.createTexture(image);

    // var pixels = new Uint8Array(1 * 1 * 4);
    // gl.readPixels(0, 0, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
    // console.log("---read pixels---");
    // console.log(pixels);

    return {
      // texture,
      // draw: function() {
      //   gl.useProgram(program);
      //   gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
      //   gl.viewport(0, 0, 64, 64);
      //   gl.clearColor(1.0, 1.0, 1.0, 1.0);
      //   gl.clear(gl.COLOR_BUFFER_BIT);
      //   const t = (Date.now()/1000)%1;
      //   uniforms.uTime && uniforms.uTime.fill([t%1]);
      //   uniforms.uSample.fill(uSampleTexture);
      //   attributes.aPosition.fill(aPositionBuffer);
      //   attributes.aTexCoord.fill(aTexCoordBuffer);
      //   drawElements(6);
      //   gl.bindFramebuffer(gl.FRAMEBUFFER, null);
      // }
    };
  }

  function initCanvas(texture) {
    const vShader = `
        attribute vec2 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        void main() {
          gl_Position = vec4(aPosition, 0, 1);
          vTexCoord = aTexCoord;
        }`;
    const fShader = `
        precision mediump float;
        // uniform sampler2D uSample;
        varying vec2 vTexCoord;
        void main() {
          // gl_FragColor = texture2D(uSample, vTexCoord);
          gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        }`;

    const {
      program,
      attributes,
      uniforms,
      fillElements,
      drawArrays,
      drawElements,
      createElementsBuffer
    } = compile({
      vShader,
      fShader,
      gl
    });

    fillElements(createElementsBuffer([0, 1, 2, 0, 2, 3]));
    const aPositionBuffer = attributes.aPosition.createBuffer([-1, 0.5, -1, -1, 1, -1, 1, 1]);
    const aTexCoordBuffer = attributes.aTexCoord.createBuffer([0, 0, 0, 1, 1, 1, 1, 0]);

    function draw() {
      gl.useProgram(program);
      attributes.aPosition.fill(aPositionBuffer);
      attributes.aTexCoord.fill(aTexCoordBuffer);
      // uniforms.uSample.fill(texture);
      gl.viewport(0, 0, ref.width, ref.height);
      gl.clearColor(0.0, 0.5, 0.5, 1.0);
      gl.clear(gl.COLOR_BUFFER_BIT);
      drawElements(6);
    }

    return {
      draw
    };
  }

  const { texture, draw: drawFramebuffer } = initFramebuffer();
  const { draw: drawCanvas } = initCanvas(/*texture*/null);

  // setInterval(function() {
    // drawFramebuffer();
    drawCanvas();
    if (isWeex && ref._swapBuffers) {
      // ref._swapBuffers();
    }
  // }, 16);
}

export default {
  data() {
    return {
      isWeex: isWeex
    };
  },

  mounted: function() {
    var ref = this.$refs.canvas_holder;

    if (isWeex) {
      ref = enable(ref, {
        debug: true,
        bridge: WeexBridge,
        disableAutoSwap: false
      });
    }

    ref.width = WXEnvironment.deviceWidth;
    ref.height = WXEnvironment.deviceWidth;

    loadImage(
      "https://img.alicdn.com/tfs/TB1apiEb8HH8KJjy0FbXXcqlpXa-1024-1024.png",
      function(image) {
        start(ref, image);
      }
    );
  }
};
</script>