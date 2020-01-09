### GCanvas-node  源代码与运行文档

  * [开发以及运行环境搭建](./env/readme.md)
  * [源代码构建以及测试运行](./src/readme.md)

### GCanvas-node 使用文档

  * quick start
``` javascript
  
const { createCanvas, Image } = require('bindings')('canvas');
let canvas = createCanvas(400, 400);
var ctx = canvas.getContext('2d')

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

       -[] 复选框   
       -[x]复选框  



   
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
  img.src = 'https://www.baidu.com/img/superlogo_c4d7df0a003d3db9b65e9ef0fe6da1ec.png?where=super'

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