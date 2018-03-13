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

            ctx.globalCompositeOperation="destination-over";
            // ctx.globalCompositeOperation="source-over";


            ctx.fillStyle = 'rgba(155, 187, 89, 0.7)';
            ctx.fillRect(0, 0, 500, 300);

            ctx.fillStyle = 'rgba(192, 80, 77, 0.7)';
            ctx.fillRect(0, 400, 500, 300);

            ctx.fillStyle = 'rgba(128, 100, 162, 0.7)';
            ctx.fillRect(0, 800, 500, 300);
        }
    }
</script>