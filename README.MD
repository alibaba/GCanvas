# [GCanvas](https://alibaba.github.io/GCanvas/)

GCanvas is a cross-platform rendering engine for mobile devices developed by Taobao. It is written with C++ based on OpenGL ES,  so it can provide high performance 2D/WebGL rendering capabilities for Javascript runtime. It also  has browser-like canvas APIs, so it's very convinent and flexiable for use, espcially for web developers.

Supported operating systems are Android 4.0+ (API 14) and  iOS 8.0+.

- [Getting Started](#getting-started)
- [Documentation](#documentation)
- [Examples](#examples)
- [Built With](#build-with)
- [Opening Issues](#opening-issues)
- [Contributing](#contributing)
- [License](#license)

## Features
- Cross-platform, support popular iOS and Android.
- High performance, accelerate graphic draw by OpenGL ES.
- Provide javascript runtime,  such as [Weex](https://github.com/apache/incubator-weex) and [ReactNative](https://github.com/facebook/react-native/).  convenient to use Javascript API like HTML canvas.
- Scalable Architecture, easy to implement a GCanvas bridge by yourself following the guide [Custom Native Bridge](./docs/Guide_Custom_GCanvas_Bridge.md) and [Custom Javascript Bridge](./docs/Guide_JS_Use.md) .
- Small size.


## Introduction

See the  [Introduction to GCanvas](./docs/An_Introduction_to_GCanvas.md) for a detailed  introduction to GCanvas.

## Getting Started

### Weex
Follow these guides to integrate GCanvas on Weex
* [Weex Guide Android](./docs/Guide_Android_Setup_Weex.md)
* [Weex Guide iOS](./docs/Guide_iOS_Setup_Weex.md)

### ReactNative
Follow this guide [ReactNative Guide](./docs/Guide_Setup_ReactNative.md)  to integrate GCanvas on ReactNative.

### Javascript
Follow this [Javascript Guide](./docs/Guide_JS_Use.md) to setup Javascript runtime environment.


## Documentation

GCanvas has browser-like canvas APIs, so almost all of the APIs are exactly same as HTML5 canvas. At this moment, we have already supported 90% of 2D APIs and 99% of WebGL APIs. You can find out those informations in [APIs document](./docs/APIs.md).

## Examples
We take [Weex](https://github.com/apache/incubator-weex) as example, code snippet of context 2d using `GCanvas`.
```javascript
import { enable, WeexBridge, Image as GImage } from "../../../../js/src/index.js";

var gcanvas = enable(this.$refs.canvas_holder, {bridge: WeexBridge});
var ctx = gcanvas.getContext("2d");
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
```

## Built With

* [Freetype](https://www.freetype.org/) - Used for font rendering on Android


## Open Issues
If you encounter a bug with GCanvas we would like to hear about it. Search the [existing issues]() and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of GCanvas  and OS you’re using. Please include a stack trace and reduced repro case when appropriate, too.

## Contributing

Please read [CONTRIBUTING](./docs/Contributing.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

GCanvas Open Source Team


## License

This project is licensed under the Apache License - see the [LICENSE](./docs/LICENSE.md) file for details
