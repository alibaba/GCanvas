GCanvas is a  cross-platform canvas solution, which implemented both graphics 2d and WebGL API. GCanvas gives its user ability to access OpenGL ES API directly.

GCanvas is designed to solve performance issue and compatibility issue of canvas for JavaScript developers. It is also a feature extention for "native frameworks" like [weex](https://weex.apache.org/) and [react native](https://facebook.github.io/react-native/), those who do not have their own canvas elements.

Currently GCanvas is working well with weex and react native. Also you could [create your own bridge]() and make it work with your UI system.

### Use GCanvas
GCanvas API is designed to be like html canvas. Here is a sample

```HTML5
<template>
  <div>
    <gcanvas v-if="isWeex" ref="canvas_holder" v-bind:style="{width:width,height:height,backgroundColor:'rgba(255,255,255,1)'}"></gcanvas>
    <canvas v-if="!isWeex" ref="canvas_holder" v-bind:style="{width:width+'px',height:height+'px',backgroundColor:'rgba(255,255,255,1)'}"></canvas>
  </div>
</template>

<script>
const isWeex = typeof callNative === "function";

const dom = weex.requireModule("dom");
import { enable, WeexBridge, Image as GImage } from "@gcanvas/core";
let EnvImage = isWeex ? GImage : Image;

function run(canvas, { requestAnimationFrame }) {
    var img = new EnvImage();
    let cxt = canvas.getContext('2d');
    img.onload = function(){
      cxt.drawImage(img, 0, 0, img.width, img.height);
    }  
    img.src = 'https://c1.staticflickr.com/3/2388/5800134409_83345951ed_b.jpg';
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
```





Try GCanvas in our [playground](https://alibaba.github.io/GCanvas/playground.html).
### Integrate GCanvas to your app
See [Integrate GCanvas on weex](https://alibaba.github.io/GCanvas/docs/Integrate%20GCanvas%20on%20Weex.html), [Integrate GCanvas on RN](https://alibaba.github.io/GCanvas/docs/Integrate%20GCanvas%20on%20ReactNative.html), or [create your own bridge](https://alibaba.github.io/GCanvas/docs/Custom%20Bridge.html).
