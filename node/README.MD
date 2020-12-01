# Node-GCanvas

Node-GCanvas is `GCanvas(OpenGLES) or Cairo` backed Canvas implementation for Node.js. It provide a way use `Canvas` in Node.js environment. So you can use canvas-like object to do your own render task at server side just like on Web. Now, we just support `Linux` and `MacOS`.


## Installation

```bash
npm install @gcanvas/node
```
Note: Node.js version >=10.0。

## Compiling
Since GCanvas relies on OpenGL for graphics rendering, CairoCanvas depedency `cairo` library,  you need to install the dependency libraries needed for operation on the corresponding system. Meanwhile, the `@gcanvas/node` is built with CMake, so the system needs to install the CMake version >=3.8.0. The dependency libraries required to run on each operating system are listed below.

OS | Command
----- | -----
OS X | `sudo brew install jpeg-turbo cairo pango pangocairo` 
Ubuntu 18.04 |  `sudo apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev libfreetype6-dev libcurl4-openssl-dev libjpeg-dev libcairo2-dev  libpangocairo-1.0-0 libpango-1.0 libglib2.0-dev`  && install [freetype字体库](http://www.linuxfromscratch.org/blfs/view/svn/general/freetype2.html) && install cmake
Docker   | [Dockerfile](./node/docker/Dockerfile)

## Quick Start Example

After run `npm install @gcanvas/node`, install npm package success, You can quick start an example like this. 

```javascript
const { createCanvas, Image } = require('@gcanvas/node');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')

ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
ctx.save() // Save the default state

ctx.fillStyle = '#09F' // Make changes to the settings
ctx.fillRect(15, 15, 120, 120) // Draw a rectangle with new settings

ctx.save() // Save the current state
ctx.fillStyle = '#FFF' // Make changes to the settings
ctx.globalAlpha = 0.5
ctx.fillRect(30, 30, 90, 90) // Draw a rectangle with new settings

ctx.restore() // Restore previous state
ctx.fillRect(45, 45, 60, 60) // Draw a rectangle with restored settings

ctx.restore() // Restore original state
ctx.fillRect(60, 60, 30, 30) // Draw a rectangle with restored settings

const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "")+ '/test-output.png');
const stream = canvas.createPNGStream();
stream.pipe(out);
```

Save the example as a `test.js` file, then execute in your terminal `node test.js`, you will get an output put file named `test-output.png`. 
![output](https://gw.alicdn.com/tfs/TB1.tg4sDM11u4jSZPxXXahcXXa-400-400.png)




## API Documentation
### Standard APIs
   * GCanvas benchmarking Web Canvas API implementation, as close as possible to the Web Canvas API. The API documentation, please visit the [Mozilla Web Canvas API] (https://developer.mozilla.org/zh-CN/docs/Web/API/CanvasRenderingContext2D). (see this link for current API compatibility)
   * Standard API Supported
     * function
        - getContext()
        - fillRect() 
        - arc()
        - arcTo()
        - beginPath()
        - bezierCurveTo()
        - quadraticCurveTo()
        - clearRect()
        - clip()
        - closePath()
        - createImageData()
        - createLinearGradient()
        - createPattern()
        - createRadialGradient()
        - drawImage()
        - ellipse:not support
        - fill()
        - fillText() 
        - getImageData()
        - getLineDash()
        - lineTo()
        - measureText()
        - moveTo()
        - putImageData()
        - rect()
        - resetTransform()
        - restore()
        - rotate()
        - save()
        - scale()
        - setLineDash()
        - setTransform()
        - stroke()
        - strokeRect()
        - strokeText()
        - transform()
        - translate()
     * propetry
        - width
        - height
        - fillStyle
        - font
        - globalAlpha
        - globalCompositeOperation
        - lineCap
        - lineDashOffset
        - lineJoin
        - lineWidth
        - miterLimit
        - shadowBlur
        - shadowColor
        - shadowOffsetX
        - shadowOffsetY
        - strokeStyle
        - textAlign
        - textBaseline


### Non-Standard APIs
 
#### createCanvas
```javascript
createCanvas(width: number, height: number) => Canvas
``` 
 
``` javascript
let canvas = createCanvas(400, 400);
``` 
  
#### Image
``` javascript
img.src: string
img.onload:Function
img.onerror:Function 
``` 

```javascript
const img = new Image()
img.onload = () => {
	ctx.drawImage(img, 0, 0);
	canvas.createPNG("demo2");
}
img.onerror = err => {
	console.log(err)
}
img.src = 'https://alibaba.github.io/GCanvas/assets/logo-gcanvas.png'
```   
  
```javascript
const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('@gcanvas/node');
const img = new Image()
const canvas = createCanvas(500, 500)
const ctx = canvas.getContext('2d')
	
img.onload = () => {
	ctx.drawImage(img, 0, 0)
	const out = fs.createWriteStream(path.join(__dirname, "")+ '/localimage.png');
	const stream = canvas.createPNGStream();
	stream.pipe(out);
}
img.onerror = err => {
	throw err
}
	
img.src = path.join(__dirname,'images', 'image.png')
 
```

#### createPNGStream

```javascript
createPNGStream() => void
```  
 
```javascript
const { createCanvas, Image } = require('@gcanvas/node');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d');
ctx.fillStyle="#ff0000"
ctx.fillRect(0, 0, 150, 150) 
canvas.createPNG("demo1") //export a png picture

const out = fs.createWriteStream(path.join(__dirname, "")+ '/localimage.png');
const stream = canvas.createPNGStream();
stream.pipe(out);
```  

#### createJPEGStream
```javascript
createJPEGStream() => void
```  
```javascript
const { createCanvas, Image } = require('@gcanvas/node');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.fillStyle = '#09F' 
ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
const out = fs.createWriteStream(path.join(__dirname, "")+ '/output-jpeg.jpg')
const stream = canvas.createJPEGStream(); // export a jpeg picture
stream.pipe(out);
```

#### toBuffer
```javascript
toBuffer(string?) => ArrayBuffer 
```  
```javascript
const { createCanvas, Image } = require('@gcanvas/node');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
ctx.fillStyle="#ff0000"
ctx.fillRect(0, 0, 150, 150) //

fs.writeFile(`${path.join(__dirname, "..")}/tobuffer.png`, canvas.toBuffer("image/png"), err => {
    if (err) {
        throw err;
    }
});

fs.writeFile(`${path.join(__dirname, "..")}/tobuffer.jpg`, canvas.toBuffer("image/jpeg"), err => {
    if (err) {
        throw err;
    }
});

var buffer=canvas.toBuffer("raw")
console.log(buffer)
``` 

#### toDataURL
```javascript
toDataURL(string?) => ArrayBuffer 
```  
```javascript
const { createCanvas, Image } = require('@gcanvas/node');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.fillStyle="#ff0000";
ctx.fillRect(0, 0, 150, 150);
var base64 = canvas.toDataURL();
console.log(base64)
``` 


### Node-GCanvas Source Code Build & Test Running 
If you want to build and develop based on source code, this document will show you how to build and run and test based on source code.

Node-canvas USES node-addon's binding API and builds with cmake-js.

#### Step of build
  
``` 
npm install cmake-js -g			//install cmake-js
npm install 					// install dependency
npm run dev 					//build project
``` 

#### Step of running

``` 
case=app.js npm run test //run *.js of example folder，output the runing result like png picture
case=app.js npm run test-headless// if you don't have system display,you can use this command
```

## Examples

Examples line in the `examples/` directory. Most produce a png image of the same name.
