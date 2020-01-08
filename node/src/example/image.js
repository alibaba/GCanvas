const { createCanvas, Image } = require('bindings')('canvas');
let canvas = createCanvas(400, 400);
let ctx = canvas.getContext('2d');
const img = new Image()

img.onload = () => {
    ctx.fillStyle = "#000000";
    ctx.fillRect(0,0,ctx.canvas.width,ctx.canvas.height);
    ctx.drawImage(img, 0, 0);
    canvas.createPNG("image");
}
img.onerror = err => {
    console.log(err)
}
img.src = 'https://www.baidu.com/img/superlogo_c4d7df0a003d3db9b65e9ef0fe6da1ec.png?where=super'