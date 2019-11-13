<template>
	<div ref="test">
		<gcanvas v-if="isWeex" class="gcanvas1" ref="gcanvas1" style="width:750;height:400;background-color:rgba(255,0,0,0.1)"></gcanvas>
		<canvas v-if="!isWeex" class="gcanvas1" ref="gcanvas1" style="width:750;height:400;background-color:rgba(255,0,0,0.1)"></canvas>
	  <gcanvas v-if="isWeex" class="gcanvas2" ref="gcanvas2" style="width:750;height:400;background-color:rgba(0,255,0,0.1)"></gcanvas>
    <canvas v-if="!isWeex" class="gcanvas2" ref="gcanvas2" style="width:750;height:400;background-color:rgba(0,255,0,0.1)"></canvas>
  </div>
</template>

<style scoped>
  .gcanvas1 {
    position: absolute; 
    top: 0; 
    left: 0; 
    right: 0; 
    bottom: 400;
  },
  .gcanvas2 {
    position: absolute; 
    top: 450; 
    left: 0; 
    right: 0; 
    bottom: 850;
  }
</style>

<script>
const isWeex = typeof WXEnvironment !== 'undefined';

// import { enable, WeexBridge, Image as GImage } from "../../../../js/src/index.js";

const enable     = require('gcanvas.js').enable;
const WeexBridge = require('gcanvas.js').WeexBridge;
const GImage     = require('gcanvas.js').Image;


var EnvImage = isWeex ? GImage : Image;

function start(ref1, size1, ref2, size2) {
  if (isWeex) {
    ref1.width = size1.width;
    ref1.height = size1.height;
    ref2.width = size2.width;
    ref2.height = size2.height;
  }

  function draw() {    
    var ctx1 = ref1.getContext('2d');
    //rect
    ctx1.fillStyle = 'red';
    ctx1.fillRect(0, 0, 100, 100);

    //rect
    ctx1.fillStyle = 'black';
    ctx1.fillRect(100, 100, 100, 100);
    ctx1.fillRect(25, 210, 700, 5);

    //circle
    ctx1.arc(450, 200, 100, 0, Math.PI * 2, true);
    ctx1.fill();        


    //GCanvas2
    var ctx2 = ref2.getContext('2d');
    //rect
    ctx2.fillStyle = 'blue';
    ctx2.fillRect(0, 0, 100, 100);

    //rect
    ctx2.fillStyle = 'green';
    ctx2.fillRect(100, 100, 100, 100);
    ctx2.fillRect(25, 210, 700, 5);

    //circle
    ctx2.arc(450, 200, 100, 0, Math.PI * 2, true);
    ctx2.fill();    
  }

  // setInterval(draw, 16);
  draw();
}

module.exports = {
  data() {
    return {
      isWeex: isWeex
    };
  },

  mounted: function() {
    var ref1 = this.$refs.gcanvas1;
    var ref2 = this.$refs.gcanvas2;

    var size1 = isWeex
      ? {
          width: 750,
          height: 750
        }
      : {
          width: parseInt(ref1.style.width),
          height: parseInt(ref1.style.height)
        };
    if (!isWeex) {
      ref1.width = size1.width;
      ref1.height = size1.height;
    }

    var size2 = isWeex
      ? {
          width: 750,
          height: 750
        }
      : {
          width: parseInt(ref2.style.width),
          height: parseInt(ref2.style.height)
        };
    if (!isWeex) {
      ref2.width = size2.width;
      ref2.height = size2.height;
    }


    if (isWeex) {
      ref1 = enable(ref1, {bridge: WeexBridge});
      ref2 = enable(ref2, {bridge: WeexBridge});
    }

    start(ref1, size1, ref2, size2);
  }
};
</script>