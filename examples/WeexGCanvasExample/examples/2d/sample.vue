<template>
  <div ref="test">
    <gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
  </div>
</template>
<script>
const isWeex = typeof WXEnvironment !== 'undefined';

// import { enable, WeexBridge, Image as GImage } from "../../../../js/src/index.js";

const enable     = require('gcanvas.js').enable;
const WeexBridge = require('gcanvas.js').WeexBridge;
const GImage     = require('gcanvas.js').Image;


var EnvImage = isWeex ? GImage : Image;

function start(ref, size) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }

  var ctx = ref.getContext("2d");

  function draw() {
    //rect
    ctx.fillStyle = 'red';
    ctx.fillRect(0, 0, 100, 100);

    //rect
    ctx.fillStyle = 'black';
    ctx.fillRect(100, 100, 100, 100);
    ctx.fillRect(25, 210, 700, 5);

    //circle
    ctx.arc(450, 200, 100, 0, Math.PI * 2, true);
    ctx.fill();
        
    var image = new GImage();
    image.src = 'https://gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png';
    image.onload = function(){
      ctx.drawImage(image, 100, 300);
    };
  }

  draw();
}

module.exports = {
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
      ref = enable(ref, {bridge: WeexBridge});
    }

    start(ref, size);
  }
};
</script>