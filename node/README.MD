### GCanvas-node  源代码与运行文档
### 环境搭建

  * node 版本 >=10.0
  * 系统环境：ubuntu >=18:04
 *  工具链:cmake
 *  依赖安装
       * sudo apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev libfreetype6-dev
       * [freetype字体库](http://www.linuxfromscratch.org/blfs/view/svn/general/freetype2.html)

### Node-canvas  源代码构建以及测试运行
 默认已经使用docker环境进行node-canvas的源代码构建和运行，node-canvas使用node-addon的binding的api以及使用cmake-js进行构建。  

构建步骤如下
  
 ``` 
  npm install cmake-js -g //安装cmake-js
  local=true npm install // 安装项目所需要的依赖,local=true表示本地构建
  npm run dev // 构建node-addon的binding中间键
 ``` 

运行步骤

 ``` 
   case=app.js npm run test //运行example下的js文件，输出运行结果，可以从导出的png图片中看
   case=app.js npm run test-headless//如果没有diplay(系统视窗)环境，可以运行这行
 ```

### GCanvas-node 使用文档

  * quick start
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

 * 标准API
   * 此项目是Web Canvas API的实现，并尽可能紧密地实现该API。有关API文档，请访问[Mozilla Web Canvas API](https://developer.mozilla.org/zh-CN/docs/Web/API/CanvasRenderingContext2D)。(有关当前API遵从性，请参阅兼容性状态)
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

   
### 非标准API
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
 * ps:img的src现在仅仅支持png格式图片

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