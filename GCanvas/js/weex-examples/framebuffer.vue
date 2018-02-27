

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

function start(ref, size, image) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }
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

    fillElements(createElementsBuffer([0, 1, 2, 0, 2, 3]));
    attributes.aPosition.fill(
      attributes.aPosition.createBuffer([-1, 1, -1, -1, 1, -1, 1, 1])
    );
    attributes.aTexCoord.fill(
      attributes.aTexCoord.createBuffer([0, 1, 0, 0, 1, 0, 1, 1])
    );
    uniforms.uSample.fill(uniforms.uSample.createTexture(image));

    gl.clearColor(0.5, 0.8, 0.5, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
    gl.viewport(0, 0, 64, 64);
    drawElements(6);
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
    gl.viewport(0, 0, size.width, size.height);

    // setTimeout 注释掉就出不来了
    setTimeout(function() {
      drawElements(6);
    }, 1000);
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

    var size = isWeex
      ? {
          width: 750,
          height: 750
        }
      : {
          width: parseInt(ref.style.width),
          height: parseInt(ref.style.height)
        };
    if (!isWeex) {
      ref.width = size.width;
      ref.height = size.height;
    }

    if (isWeex) {
      ref = enable(ref, { debug: true, bridge: WeexBridge });
    }

    loadImage(
      "https://img.alicdn.com/tfs/TB1apiEb8HH8KJjy0FbXXcqlpXa-1024-1024.png",
      function(image) {
        start(ref, size, image);
      }
    );
  }
};
</script>