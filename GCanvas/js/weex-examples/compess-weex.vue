<template>
  <div>
    <gcanvas v-if="isWeex" ref="canvas_holder" v-bind:style="{width:width,height:height,backgroundColor:'rgba(255,255,255,1)'}"></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" v-bind:style="{width:width+'px',height:height+'px',backgroundColor:'rgba(255,255,255,1)'}"></canvas>
  </div>
</template>

<script>
const isWeex = typeof callNative === "function";

const dom = weex.requireModule("dom");
import { enable, WeexBridge, Image as GImage } from "gcanvas.js";

var centerX = 0;
var centerY = 0;
var centerLineLength = 0;

var lineLength = 0;
var lineDistance = 0;
var current = 15;

function run(canvas, { requestAnimationFrame }) {

	let ctx = canvas.getContext('2d');
  function draw()
  {
      ctx.fillStyle = 'black';
      ctx.fillRect(0, 0, canvas.width, canvas.height);
      ctx.fillStyle = 'white';
      ctx.strokeStyle = 'white';

      //cross
      ctx.beginPath();
      ctx.lineWidth = 2;// TODO 没生效，被下面设置的顶掉了
      ctx.moveTo(centerX - centerLineLength, centerY);
      ctx.lineTo(centerX + centerLineLength, centerY);
      ctx.moveTo(centerX, centerY - centerLineLength);
      ctx.lineTo(centerX, centerY + centerLineLength);
      ctx.stroke();

      //northBar
      ctx.beginPath();
      ctx.lineWidth = 10;
      ctx.moveTo(centerX, centerY + lineDistance - lineLength);
      ctx.lineTo(centerX, centerY + lineDistance + lineLength);
      ctx.stroke();

      ctx.beginPath();
      ctx.strokeStyle = "red";
      ctx.lineWidth = 50;
      var circle = {
          x : centerX,    //圆心的x轴坐标值
          y : centerY,    //圆心的y轴坐标值
          r : lineDistance + lineLength + 25       //圆的半径
      };
      ctx.arc(circle.x, circle.y, circle.r, Math.PI / 2 ,Math.PI / 2 + Math.PI * 2 * ( (current % 360) / 360), current > 180);    
      ctx.stroke();


      var strokeWidth;
      for (var i = current; i < 360 + current; i = i + 2) {
          ctx.save();
          ctx.translate(centerX, centerY);
          ctx.rotate(Math.PI * 2 * (i / 360));

          if (i % 30 === current % 30) {
            strokeWidth = 6;
             ctx.font = "48px";
             var num = parseInt((i - current)%360);

             if (num === 0) {

                 //arc
                 ctx.beginPath();
                 var height = 50 * Math.sin(Math.PI / 3);
                 ctx.moveTo(0, lineDistance - height - 5);
                 ctx.lineTo(-25, lineDistance - 5);
                 ctx.lineTo(+25, lineDistance - 5);
                 ctx.fillStyle = 'red';
                 ctx.fill();
                 ctx.fillStyle = 'white';
             }

             ctx.save();
             ctx.translate(0,lineDistance - lineLength - 34);
             ctx.rotate(-Math.PI * 2 * (i / 360));
             ctx.textBaseline="middle";
             ctx.textAlign="center";
             ctx.fillText(num + '', 0, 0);
             ctx.restore();
          } else {
              strokeWidth = 2;
          }

          if (i % 360 === 0) {
              ctx.strokeStyle = 'black';
          } else {
              ctx.strokeStyle = 'white';
          }

          ctx.lineWidth = strokeWidth;
          ctx.beginPath();
          ctx.moveTo(0, lineDistance);
          ctx.lineTo(0, lineDistance + lineLength);
          ctx.stroke();
          ctx.restore();
      }

      
      ['南', '西', '北', '东'].forEach(function(direction, index) {
       ctx.save();
       ctx.translate(centerX, centerY);
       ctx.rotate(Math.PI * 2 * ((current%360 + index*90) / 360));
       ctx.save();
       ctx.translate(0, centerLineLength + 30);
       ctx.rotate(-Math.PI * 2 * ((current%360 + index*90) / 360));
       ctx.textAlign="center";
       ctx.textBaseline="middle";
       ctx.font = "60px";
       ctx.fillText(direction + '', 0, 0);
       ctx.restore();
       ctx.restore();
       })

      	current = (current + 1)%360;
      	requestAnimationFrame(draw);
    }

    draw();
}

export default {
  data() {
    return {
      isWeex,
      width: 750,
      height: 980
    };
  },

  mounted: function() {

    const start = () => {

      var ref = this.$refs.canvas_holder;

      var size = isWeex
        ? {
            width: parseInt(this.width),
            height: parseInt(this.height)
          }
        : {
            width: parseInt(ref.clientWidth),
            height: parseInt(ref.clientHeight)
          };

      if (isWeex) {
        ref = enable(ref, { debug: true, bridge: WeexBridge });
      }

      ref.width = size.width;
      ref.height = size.height;

    centerX = parseInt(ref.width / 2);
	centerY = parseInt(ref.height / 2);
	centerLineLength = parseInt(ref.width / 5);

	lineLength = parseInt(ref.width / 20);
	lineDistance = parseInt(-ref.width / 2.8);

      run(ref, {
        requestAnimationFrame: isWeex ? setTimeout : requestAnimationFrame
      });
    };

    setTimeout(function(){
      dom.getComponentRect("viewport", e => {
      this.height = e.size.height;

      this.isReady = true;

      setTimeout(start, 1000);
    })
    }, 50);
  }
};
</script>