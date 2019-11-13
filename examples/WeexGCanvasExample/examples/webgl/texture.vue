<template>
  <div>
    <gcanvas v-if="isWeex" ref="canvas_holder" style="width:750;height:750;background-color:rgba(255,0,0,1)"></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(255,0,0,1)"></canvas>
    <text class="text">isWeex:{{isWeex}}</text>
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
// import hackLog from "./hack-log";

// import { enable, WeexBridge, Image as GImage } from "../../../../js/src/index.js";

const enable     = require('../../../../js/dist/gcanvas.min.js').enable;
const GImage     = require('../../../../js/dist/gcanvas.min.js').Image;

let EnvImage = isWeex ? GImage : Image;

function loadImage(url, callback) {
  var image = new EnvImage();
  image.crossOrigin = true;
  image.onload = function() {
    callback(image);
  };
  image.src = url;
}

function startTexture(ref, size) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }

  var gl = ref.getContext("webgl");

  // hackLog(gl);

  const vShader = `  
attribute vec2 aPosition;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;
void main() {
  vTexCoord = aTexCoord;  
  gl_Position = vec4(aPosition, 0, 1);
}`;
  const fShader = `
precision mediump float;
uniform sampler2D uSample;
varying vec2 vTexCoord;
void main() {
  gl_FragColor = texture2D(uSample, vTexCoord);
}`;

  loadImage(
    "//img.alicdn.com/tfs/TB1apiEb8HH8KJjy0FbXXcqlpXa-1024-1024.png",
    function(image) {
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
      attributes.aPosition.fill(
        attributes.aPosition.createBuffer([-1, 1, -1, -1, 1, -1, 1, 1])
      );
      attributes.aTexCoord.fill(
        attributes.aTexCoord.createBuffer([0, 1, 0, 0, 1, 0, 1, 1])
      );

      gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
      const tex = uniforms.uSample.createTexture(image);
      uniforms.uSample.fill(tex);

      function render() {
        gl.clear(gl.COLOR_BUFFER_BIT);
        drawElements(6);
      }

      function tick() {
        render();
        // setTimeout(tick, 100);
      }
      // tick();
      setTimeout(tick, 1000);
    }
  );
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
    ref = enable(ref, {debug: true, disableComboCommands: true});

    startTexture(ref, size);
  }
};
</script>