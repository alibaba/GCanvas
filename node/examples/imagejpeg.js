const { createCanvas, Image } = require('../export');
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
        canvas.createPNG("imagejpeg");
    }
    img2.src = "http://192.168.2.101:3000/demo1.jpg"
}
img.onerror = err => {
    console.log(err)
}
img.src = 'http://192.168.2.101:3000/demo1.jpg'
