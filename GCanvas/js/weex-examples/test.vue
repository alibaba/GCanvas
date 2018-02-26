<template>
	<div ref="test">
		<gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
		<canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
	</div>
</template>
<script>
const isWeex = typeof callNative === "function";

import { enable, WeexBridge, Image as GImage } from "../src/index.js";
// var { enable, WeexBridge, Image: GImage } = require("../dist/gcanvas.min.js");

var EnvImage = isWeex ? GImage : Image;

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
  }
};
</script>