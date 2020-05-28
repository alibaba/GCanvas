### GCanvas-node

### 简介

GCanvasNode是以GCanvas为backend的node渲染库

### Brief Introduction

GCanvasNode is a node rendering library with GCanvas as backend

### 安装

```bash
npm install @gcanvas/node
```
要求:Nodejs版本 >=10.0。


### Installation

```bash
npm install @gcanvas/node
```
Note:Nodejs version >=10.0。


### 编译


因为GCanvas依赖OpenGL进行图形渲染，所以需要在对应系统上安装OpenGL运行所需要的依赖库。同时，GCanvas的使用CMake进行构建，故需要系统安装CMake版本>=3.8.0。下面列出在各个操作系统中安装GCanvas运行所需要的依赖库。

OS | Command
----- | -----
OS X | 即将支持
Ubuntu 18.04 |  `sudo apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev libfreetype6-dev libcurl4-openssl-dev libjpeg-dev`  && install [freetype字体库](http://www.linuxfromscratch.org/blfs/view/svn/general/freetype2.html) && install cmake
Windows  | 暂不支持
Docker   | [Dockerfile](./docker/Dockerfile)


### Compiling

Since GCanvas relies on OpenGL for graphics rendering, you need to install the dependency libraries needed for OpenGL operation on the corresponding system. Meanwhile, the GCanvas is built with CMake, so the system needs to install the CMake version >=3.8.0. The dependency libraries required to install the GCanvas run on each operating system are listed below.

OS | Command
----- | -----
OS X | To support
Ubuntu 18.04 |  `sudo apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev libfreetype6-dev libcurl4-openssl-dev libjpeg-dev`  && install [freetype字体库](http://www.linuxfromscratch.org/blfs/view/svn/general/freetype2.html) && install cmake
Windows  | Temporarily not supported
Docker   | [Dockerfile](./docker/Dockerfile)

### 快速上手
``` javascript
  
const { createCanvas, Image } = require('bindings')('canvas');
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

canvas.createPNG("demo1")
 ```

### Quick Start Example
``` javascript
  
const { createCanvas, Image } = require('bindings')('canvas');
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

canvas.createPNG("demo1")
 ```

### API 文档
#### 标准API
   * GCanvas对标 Web Canvas API的实现，尽可能地贴近Web Canvas的 API。有关API文档，请访问[Mozilla Web Canvas API](https://developer.mozilla.org/zh-CN/docs/Web/API/CanvasRenderingContext2D)。(有关当前API的兼容性查看此链接)
   * 标准API支持情况
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

#### 非标准API
   ##### createCanvas
 ```javascript
   createCanvas(width: number, height: number) => Canvas
 ```    
 ``` javascript

   let canvas = createCanvas(400, 400);//
 ``` 
  
  ##### Image
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
const { createCanvas, Image } = require('bindings')('canvas');
const img = new Image()
const canvas = createCanvas(500, 500)
const ctx = canvas.getContext('2d')

img.onload = () => {
  ctx.drawImage(img, 0, 0)
  canvas.createPNG("local")
}
img.onerror = err => {
  throw err
}

img.src = path.join(__dirname,'images', 'image.png')
 

 ```   
 * ps:img的src现在仅仅支持png或者jpeg格式图片

##### createPNG
 ```javascript
   createPNG(name:string) => void
 ```  
  ```javascript
   let canvas = createCanvas(400, 400);
  var ctx = canvas.getContext('2d');
  ctx.fillStyle="#ff0000"
   ctx.fillRect(0, 0, 150, 150) 
   canvas.createPNG("demo1") //生成一张png的图片
 ```  

##### createJPEG
 ```javascript
   createPNG(name:string) => void
 ```  
  ```javascript
  const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.fillStyle = '#09F' 
ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
canvas.createJPEG("demo2")

 ```  

##### createPNGStream
 ```javascript
   createPNGStream(void) => void
 ```  
  ```javascript
const { createCanvas, Image } = require('../export')
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/demo1.png');
ctx.fillStyle = '#09F' 
ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});
 ```  
##### createJPEGStream
 ```javascript
   createJPEGStream(void) => void
 ```  
  ```javascript
const { createCanvas, Image } = require('../export')
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/demo1.jpg');
ctx.fillStyle = '#09F' 
ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
var stream = canvas.createJPEGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});
 ```  

### API Documentation
#### Standard APIs
   * GCanvas benchmarking Web Canvas API implementation, as close as possible to the Web Canvas API. The API documentation, please visit the Mozilla Web Canvas API (https://developer.mozilla.org/zh-CN/docs/Web/API/CanvasRenderingContext2D). (see this link for current API compatibility)
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


#### Non-Standard APIs
   ##### createCanvas
 ```javascript
   createCanvas(width: number, height: number) => Canvas
 ```    
 ``` javascript

   let canvas = createCanvas(400, 400);
 ``` 
  
  ##### Image
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
const { createCanvas, Image } = require('bindings')('canvas');
const img = new Image()
const canvas = createCanvas(500, 500)
const ctx = canvas.getContext('2d')

img.onload = () => {
  ctx.drawImage(img, 0, 0)
  canvas.createPNG("local")
}
img.onerror = err => {
  throw err
}

img.src = path.join(__dirname,'images', 'image.png')
 

 ```   
 * Note:img.src now only supports PNG or jpeg images

##### createPNG
 ```javascript
   createPNG(name:string) => void
 ```  
  ```javascript
   let canvas = createCanvas(400, 400);
  var ctx = canvas.getContext('2d');
  ctx.fillStyle="#ff0000"
   ctx.fillRect(0, 0, 150, 150) 
   canvas.createPNG("demo1") //export a png picture
 ```  

##### createJPEG
 ```javascript
   createPNG(name:string) => void
 ```  
  ```javascript
  const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.fillStyle = '#09F' 
ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
canvas.createJPEG("demo2") // export a jpeg picture

 ```  



 ### Node-GCanvas  源代码构建以及测试运行
如果你想基于源代码进行构建以及开发等工作， 这份文档会告诉你如何基于原代码构建以及运行和测试。
Node-canvas使用node-addon的binding的api以及使用cmake-js进行构建。 

构建步骤如下
  
 ``` 
  npm install cmake-js -g //安装cmake-js
  local=true npm install // 安装项目所需要的依赖,local=true表示基于源代码安装
  cmake-js compile  // 构建工程
 ``` 

运行步骤

 ``` 
   case=app.js npm run test //运行example下的js文件，输出运行结果，可以从导出的png图片中看
   case=app.js npm run test-headless//如果没有diplay(系统视窗)环境，可以运行这行
 ```

### Node-GCanvas Source Code Build & Test Running 
If you want to build and develop based on source code, this document will show you how to build and run and test based on source code.

Node-canvas USES node-addon's binding API and builds with cmake-js.

Step Of Build
  
 ``` 
  npm install cmake-js -g //install cmake-js
  local=true npm install // install dependency ,local=true means install with source
  cmake-js compile // build project
 ``` 

Step Of Running

 ``` 
   case=app.js npm run test //run *.js of example folder，output the runing result like png picture
   case=app.js npm run test-headless// if you don't have system display,you can use this command
 ```