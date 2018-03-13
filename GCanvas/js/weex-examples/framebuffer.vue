

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

  function drawFramebuffer() {
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
    void main() {
      gl_FragColor = texture2D(uSample, vTexCoord);
      // gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
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

    gl.useProgram(program);

    const { framebuffer, texture } = createFramebuffer(64, 64);

    console.log('check framebuffer status');
    console.log(gl.checkFramebufferStatus(gl.FRAMEBUFFER));
    console.log(gl.FRAMEBUFFER_COMPLETE);

    fillElements(createElementsBuffer([0, 1, 2, 0, 2, 3]));
    attributes.aPosition.fill(
      attributes.aPosition.createBuffer([-1, 1, -1, -1, 1, -1, 1, 1])
    );
    attributes.aTexCoord &&
      attributes.aTexCoord.fill(
        attributes.aTexCoord.createBuffer([0, 1, 0, 0, 1, 0, 1, 1])
      );
    uniforms.uSample &&
      uniforms.uSample.fill(uniforms.uSample.createTexture(image));

    gl.clearColor(0.5, 0.8, 0.5, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
    gl.viewport(0, 0, 64, 64);
    drawElements(6);

    var pixels = new Uint8Array(1 * 1 * 4);
    gl.readPixels(0, 0, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
    console.log('---read pixels---');
    console.log(pixels);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    return texture;
  }

  function drawCanvas(texture) {
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
        void main() {
          gl_FragColor = texture2D(uSample, vTexCoord);
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

    gl.useProgram(program);
    fillElements(createElementsBuffer([0, 1, 2, 0, 2, 3]));
    attributes.aPosition.fill(
      attributes.aPosition.createBuffer([-1, 1, -1, -1, 1, -1, 1, 1])
    );
    attributes.aTexCoord.fill(
      attributes.aTexCoord.createBuffer([0, 0, 0, 1, 1, 1, 1, 0])
    );
    uniforms.uSample.fill(texture);

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.viewport(0, 0, ref.width, ref.height);

    // setTimeout(function() {
    drawElements(6);
    // }, 1000);
  }

  const texture = drawFramebuffer();

  if (texture) {
    drawCanvas(texture);
  }
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
        bridge: WeexBridge
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