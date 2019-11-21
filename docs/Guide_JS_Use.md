# GCanvas.js

This module provides a canvas-like object in hybrid enviroment (Weex and ReactNative), so you can use the canvsa-like object to do your own job, just like on Web. Besides canvas, it also provide an Image-like class, so you can use the it  instead of Image on Web.

## Usage in Weex

First, you should install this package from npm:

```
npm install @gcanvas/core --save
```

Put a `gcanvas` element in template. Note that if you want to downgrade weex native to html, you should put a `canvas` element when is in browser enviroment.

```vue
<template>
	<div>
		<gcanvas v-if="isWeex" ref="canvas_holder" style="top: 0; width:750;height:750;position:absolute; background-color:rgba(0,0,0,0.1)" ></gcanvas>
		<canvas v-if="!isWeex" ref="canvas_holder" style="width:750px;height:750px;background-color:rgba(0,0,0,0)"></canvas>
	</div>
</template>
```

And then revolve in your own progress using weex-toolkit.

```javascript
const { enable, WeexBridge, Image: GImage } = require('@gcanvas/core');
const EnvImage = isWeex ? Image : GImage;
```

Notice that we create a EnvImage variable, on web enviroment, it's the native Image class (HTMLImageElement), while on hybrid it's a class defined in gcanvas.js. You can use EnvImage just as the Image object :

```
// this works on both browser and hybrid enviroment
const image = new EnvImage();
image.onload = function(){
    console.log('loaded');
}
image.src = theUrl;
```

Then you should call `enable` before everything starts, and pass WeexBridge as the `bridge` option in.

```
mounted: function() {
let ref = this.$refs.canvas_holder;

// some size operations

if (isWeex) {
    ref = enable(ref, {bridge: WeexBridge});
}

// here ref is a canvas-like object
}
```

After this, ref is a canvas-like object, you can call `ref.getContext('2d')` or `ref.getContext('webgl')` and go on your own application. Everything is familiar, right?

## Usage in ReactNative

First, you should install this package from npm:

```
npm install react-native-gcanvas --save
npm install @gcanvas/core --save
```

Second,  import ReactNative modules, and  `GCanvasView `  component, `GCanvasView`.

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

Third, import  `enable`, `ReactNativeBridge` and `image` from  `@gcanvas/core`,  initialize `ReactNativeBridge` with native module and platform.

```javascript
const { enable, ReactNativeBridge, Image: GImage } = require('@gcanvas/core');

ReactNativeBridge.GCanvasModule = NativeModules.GCanvasModule;
ReactNativeBridge.Platform = Platform;
```

Finally, you can use GCanvas in ReactNative App. Notice that before you `enable` GCanvas, convert`GCanvasView` reactTag to a string.
```
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

## Writing your own bridge

Notice that we pass in the bridge object calling `enable`. Actually in this module we provide WeexBridge and ReactNativeBridge, if you want to define a custom bridge, you need to implement the same functions WeexBridge implemented. These functions are:

| function name          | arguments                   | return              |
| ---------------------- | --------------------------- | ------------------- |
| callEnable             | ref, configArray            | gcanvas             |
| callEnableDebug        |                             |                     |
| callEnableDisableCombo |                             |                     |
| callSetContextType     | componentId, context_type   |                     |
| render                 | componentId                 |                     |
| render2d               | componentId, commands       |                     |
| flushNative            | componentId                 |                     |
| callNative             | componentId, cmdArgs, cache | flush native result |
| texImage2D             | componentId, ...args        |                     |
| texSubImage2D          | componentId, ...args        |                     |
| bindImageTexture       | componentId, src, imageId   |                     |
| preloadImage           | [url, id], callback         |                     |

You can check `src/bridge/bridge-weex.js` for detail.

This module is the javascript part of the js-native bridge. About writing the native part, you can refer to [Custom GCanvas Bridge](https://alibaba.github.io/GCanvas/docs/Custom%20Bridge.html).
