const fs = require('fs')
const path = require('path')
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/image-jpeg.jpeg');

const { createCanvas, Image } = require('../export');
const canvas = createCanvas(600, 600);
const ctx = canvas.getContext('2d');
const img = new Image()
const img2 = new Image()

img.onload = () => {
    ctx.fillStyle = "#000000";
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    ctx.drawImage(img, 0, 0);

    img2.onerror = err => {
        console.log(err)
    }
    img2.onload = () => {
        ctx.drawImage(img2, 300, 300, 100, 100);
        const stream = canvas.createJPEGStream();
        stream.pipe(out);
    }
    img2.src = "https://gw.alicdn.com/tfs/TB1cHyouXT7gK0jSZFpXXaTkpXa-240-180.jpg"
}
img.onerror = err => {
    console.log(err)
}
img.src = 'https://gw.alicdn.com/tfs/TB1cHyouXT7gK0jSZFpXXaTkpXa-240-180.jpg'
