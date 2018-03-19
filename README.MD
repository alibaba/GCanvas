# [GCanvas](https://alibaba.github.io/GCanvas/)

GCanvas is a cross-platform rendering engine for mobile devices developed by Taobao. It is written with C++ based on OpenGL ES,  so it can provide high performance 2D/WebGL rendering capabilities for Javascript runtime. It also  has browser-like canvas APIs, so it's very convenient and flexiable for use, especially for web developers.

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
- Scalable Architecture, easy to implement a GCanvas bridge by yourself following the guide [Custom Native Bridge](https://alibaba.github.io/GCanvas/docs/Custom%20Bridge.html) .
- Small size.


## Introduction

See the  [Introduction to GCanvas](https://alibaba.github.io/GCanvas/docs/Developer's%20Guide.html) for a detailed  introduction to GCanvas.

## Getting Started

### Weex
Follow [Weex Setup Guide](https://alibaba.github.io/GCanvas/docs/Integrate%20GCanvas%20on%20Weex.html) to integrate GCanvas on Weex

### ReactNative
Follow [ReactNative Setup Guide](https://alibaba.github.io/GCanvas/docs/Integrate%20GCanvas%20on%20ReactNative.html)  to integrate GCanvas on ReactNative.

### Javascript
Try our [Playground](https://alibaba.github.io/GCanvas/playground.html).
GCanvas has browser-like canvas APIs, so almost all of the APIs are exactly same as HTML5 canvas. At this moment, we have already supported 90% of 2D APIs and 99% of WebGL APIs. You can find out those informations in [2D  APIs](https://alibaba.github.io/GCanvas/docs/Graphics%202D.html) and [WebGL APIs](https://alibaba.github.io/GCanvas/docs/WebGL.html).


## Documentation
Check [Documentation](https://alibaba.github.io/GCanvas/docs/Introduction.html) for more information.

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

Please read [CONTRIBUTING](./GCanvas/docs/Contributing.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

GCanvas Open Source Team


## License

This project is licensed under the Apache License - see the [LICENSE](./GCanvas/docs/LICENSE.md) file for details
