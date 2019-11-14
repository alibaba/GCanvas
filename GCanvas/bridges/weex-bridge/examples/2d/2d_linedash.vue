<template>
	<div ref="test">
		<gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
		<canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
	</div>
</template>
<script>
const isWeex = typeof WXEnvironment !== 'undefined';

import { enable, WeexBridge } from "../../../../js/src/index.js";

function start(ref, size) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }

  var ctx = ref.getContext("2d");

  function draw() {
    ctx.save()
    ctx.setLineDash([5, 15, 25]);
    ctx.beginPath();
    ctx.moveTo(0, 100);
    ctx.lineTo(400, 100);
    ctx.stroke();
    ctx.restore()
    ctx.strokeRect(10, 10, 100, 50);
    ctx.draw()
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