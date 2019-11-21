### I. Install JS Library

GCanvas requires a js library.  Install it from npm:

`npm install weex-gcanvas --save`
`npm install react-native-gcanvas --save`

or find the .js fild in our project:

`https://github.com/alibaba/GCanvas/tree/master/GCanvas/js/src`

### II. Getting Context

To use gcanvas, you need to get a context(2d or 3d) before doing anything else.  There is a few differences between the platforms, but after getting the context, the code should be same.

#### weex

For weex, a "gcanvas" element was added to weex runtime. `this.$refs.<canvas ref>` should return the element. Note for some techinal reason the element is not the GCanvas instance itself. We need to call `enable` with width and height on this element.

```HTML5
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
        var gcanvas = enable(ref, {width:750, height:750});
        var ctx = gcanvas.getContext('2d');
        ctx.drawText("Hello world!");
    }
};
</script>
```

#### React native
```javascript


import {
  Platform,
  StyleSheet,
  Text,
  View,
  TouchableHighlight
} from 'react-native';

import {
  requireNativeComponent,
  findNodeHandle,
  NativeModules
} from 'react-native';

import {
  GCanvasView,
} from 'react-native-gcanvas';
```

Then, import  `enable`, `ReactNativeBridge` and `image` from  `@gcanvas/core`,  initialize `ReactNativeBridge` with native module and platform.

```Javascript
const { enable, ReactNativeBridge, Image: GImage } = require('@gcanvas/core');

ReactNativeBridge.GCanvasModule = NativeModules.GCanvasModule;
ReactNativeBridge.Platform = Platform;
```

Finally, you can use GCanvas in ReactNative App. Notice that before you `enable` GCanvas, convert`GCanvasView` reactTag to a string.
```JavaScript
export default class App extends Component<{}> {

  //draw something with gcanvas
  draw() => {
	var ref = this.refs.canvas_holder;

	//must convert canvas tag to a string
    var canvas_tag = findNodeHandle(ref);
    var el = { ref:""+canvas_tag, style:{width:414, height:376}};
    ref = enable(el, {bridge: ReactNativeBridge});

    //TODO get context by yourself
    var ctx = ref.getContext('2d');
    //rect
    ctx.fillStyle = 'green';
    ctx.fillRect(0, 0, 100, 100);

    //rect
    ctx.fillStyle = 'black';
    ctx.fillRect(100, 100, 100, 100);
    ctx.fillRect(25, 205, 414-50, 5);

    //circle
    ctx.arc(200, 315, 100, 0, Math.PI * 2, true);
    ctx.fill();

    var image = new GImage();
    image.onload = function(){
      ctx.drawImage(image, 150, 0);
      ctx.drawImage(image, 150, 450);
    }
    image.src = '//gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png';
  }

  //render
  render(){
      return  <GCanvasView ref='canvas_holder' style={top: 20,width: 414,height :700,backgroundColor: '#FF000030'}></GCanvasView>  
  }

}
```

### III. Using GCanvas 2d API
To know which API could be used on gcanvas, check [reference]().

The following sample shows how we could use gcanvas.

Drawing text:

Drawing geometry:

```JavaScript
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
```

Drawing image:

```Javascript
var image = new GImage();
image.src = 'https://gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png';
image.onload = function(){
  ctx.drawImage(image, 100, 300);
};
```

### IV. Using GCanvas WebGL
To know which API could be used on gcanvas, check [reference]().

The following sample shows how we could use gcanvas.

```JavaScript
var gl = gcanvas.getContext('webgl');
gl.clearColor(0.0, 0.0, 0.0, 1.0);
gl.enable(gl.DEPTH_TEST);
gl.depthFunc(gl.LEQUAL);
gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);  
```
