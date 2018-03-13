<template>
	<div ref="test">
		<gcanvas ref="canvas_holder" style="width:750;height:750;background-color:rgba(0,0,0,0.1)"></gcanvas>
	</div>
</template>
<script>
    //for debug
    const enable = require('../../../../js/dist/gcanvas.min.js').enable;
    const WeexBridge = require('../../../../js/dist/gcanvas.min.js').WeexBridge;
    const GImage = require('../../../../js/dist/gcanvas.min.js').Image;


    module.exports = {
        mounted: function () {
            var ref = this.$refs.canvas_holder;
            var size = {width:750, height:750};
            
            var gcanvas = enable(ref, {bridge: WeexBridge});
            gcanvas.width = size.width;
            gcanvas.height = size.height;

            var ctx = gcanvas.getContext('2d');

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
    };
</script>