<template>
    <div>
        <gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:1000;position:absolute;"></gcanvas>
        <canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:1000px;"></canvas>
    </div>
</template>
<script>

    const isWeex = weex.config.env.platform !== 'Web'
    const { enable, WeexBridge, Image: GImage } = require('gcanvas.js');
    const EnvImage = !isWeex ? Image : GImage;

    export default {
        data() {
            return {
                isWeex: isWeex ? 1 : 0
            }
        },
        mounted: function () {
            let ref = this.$refs.canvas_holder;
            if (isWeex) {
                ref = enable(ref, {bridge: WeexBridge});
            }
            
            var ctx = ref.getContext('2d');
            ctx.lineWidth = 1;
			ctx.strokeRect(20,20,150,100);

			/*
			ctx.beginPath();
			  ctx.moveTo(20,20);
			  ctx.lineTo(20,100);
			  ctx.lineTo(80,100);
			  ctx.closePath();
			  ctx.stroke();
			  */

		}
	};
</script>