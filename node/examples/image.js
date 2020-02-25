const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d');
const img = new Image()
const img2 = new Image()

img.onload = () => {
    ctx.fillStyle = "#000000";
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    ctx.drawImage(img, 0, 0, 100, 100);
    img2.onerror = err => {
        console.log(err)
    }
    img2.onload = () => {
        ctx.drawImage(img2, 150, 150, 100, 100);
        canvas.createPNG("image");
    }
    img2.src = "https://alibaba.github.io/GCanvas/assets/logo-gcanvas.png"
}
img.onerror = err => {
    console.log(err)
}
img.src = "https://alibaba.github.io/GCanvas/assets/logo-gcanvas.png"
