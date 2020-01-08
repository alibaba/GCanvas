const { createCanvas, Image } = require('bindings')('canvas');
let canvas = createCanvas(400, 400);
let ctx = canvas.getContext('2d');
const img = new Image()

img.onload = () => {
    ctx.drawImage(img, 0, 0);
    canvas.createPNG("demo2");
}
img.onerror = err => {
    console.log(err)
  }
img.src = 'https://img.alicdn.com/imgextra/i2/826192/O1CN01JPTxW41vbz1K3Cx9v_!!826192-0-lubanu.jpg_400x400q90.jpg_.webp'