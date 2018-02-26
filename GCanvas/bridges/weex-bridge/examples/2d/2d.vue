<template>
	<div ref="test">
		<gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
		<canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
	</div>
</template>
<script>
const isWeex = typeof callNative === "function";

// import { enable, WeexBridge, Image as GImage } from "../../../../js/src/index.js";


const enable     = require('../../../../js/dist/gcanvas.min.js').enable;
const WeexBridge = require('../../../../js/dist/gcanvas.min.js').WeexBridge;
const GImage     = require('../../../../js/dist/gcanvas.min.js').Image;


var EnvImage = isWeex ? GImage : Image;

function start(ref, size) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }

  var ctx = ref.getContext("2d");

  var imageLoaded = false;

  var image = new EnvImage();
  image.src = "//gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png";
  image.onload = function() {
    imageLoaded = true;
  };

  function draw() {
    const offsetX = ((Date.now() / 3000) % 1) * size.width * 0.8;
    ctx.save();
    ctx.translate(10, 50);
    ctx.clearRect(0, 0, size.width, size.height);

    ctx.fillStyle = "blue";
    ctx.fillRect(offsetX, 0, size.width * 0.2, size.height * 0.2);

    ctx.fillStyle = "red";
    ctx.beginPath();
    ctx.arc(
      size.width * 0.1 + offsetX,
      size.height * 0.5,
      size.height * 0.1,
      0,
      Math.PI * 2,
      true
    );
    ctx.closePath();
    ctx.fill();

    //image
    if (imageLoaded) {
      ctx.drawImage(
        image,
        offsetX,
        size.height * 0.8,
        225,
        75
      );
    }

    ctx.restore();
  }

  setInterval(draw, 16);
  // draw();
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