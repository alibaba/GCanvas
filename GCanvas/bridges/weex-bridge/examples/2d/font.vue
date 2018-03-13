<template>
  <div ref="test">
    <gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
  </div>
</template>
<script>
const isWeex = typeof callNative === "function";

// import { enable, WeexBridge } from "../../../../js/src/index.js";

const enable     = require('../../../../js/dist/gcanvas.min.js').enable;
const WeexBridge = require('../../../../js/dist/gcanvas.min.js').WeexBridge;


function start(ref, size) {
  if (isWeex) {
    ref.width = size.width;
    ref.height = size.height;
  }

  var ctx = ref.getContext("2d");
  var offsetX = 0;

  //font
  function draw() {
    ctx.clearRect(0, 0, ref.width, ref.height);

    ctx.save();
    ctx.font="20px Verdana";
    ctx.fillText("GCanvas很牛逼", offsetX, 50);  
    ctx.fillStyle = 'red'
    ctx.restore();

    ctx.save();
    ctx.font="30px Georgia";
    ctx.fillText("Hello GCanvas",offsetX, 100);
    ctx.restore();

    ctx.save();
    ctx.font="40px italic bold arial";
    ctx.fillText("Hello GCanvas",offsetX, 150);
    ctx.restore();
    
    ctx.save();
    ctx.font="bold 50px Verdana";
    ctx.fillText("Hello World",offsetX, 200);
    ctx.restore();

    //gradient
    ctx.save();
    ctx.font="bold 60px Verdana";
    var gradient=ctx.createLinearGradient(offsetX,0,offsetX+750, 0);
    gradient.addColorStop("0", "magenta");
    gradient.addColorStop("0.5", "blue");
    gradient.addColorStop("1.0", "red");
    ctx.fillStyle = gradient;
    ctx.fillText("GCanvas Fast Engine",offsetX, 250);
    ctx.restore();

    ctx.save();
    ctx.font="italic 70px 微软雅黑";
    ctx.fillText("italic 70 号", offsetX, 320);
    ctx.restore();

    ctx.save();
    ctx.font="60px Helvetica";
    ctx.fillText("Helvetica 60号", offsetX, 400);
    ctx.restore();

    ctx.save();
    ctx.font="50px 微软雅黑";
    ctx.fillText("Helvetica 50号", offsetX, 470);
    ctx.restore();

    ctx.save();
    ctx.font="italic bold 40px Verdana";
    ctx.strokeText("Bold Verdana 40号字", offsetX, 530);
    ctx.restore();

    ctx.save();
    ctx.font="bold 30px Verdana";
    ctx.fillText("Bold Verdana 30号字", offsetX, 580);
    ctx.restore();
    offsetX = (offsetX + 1 ) % ref.width;    
  }

  setInterval(draw, 16);
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