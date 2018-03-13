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

			//drawImage
			var image = new EnvImage();
			image.onload = function(){
				ctx.drawImage(image, 100, 330);
				ctx.drawImage(image, 100+300, 330, 225, 75);
			}
			image.src = 'https://gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png';
		}
	};
</script>