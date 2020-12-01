const fs = require('fs')
const path = require('path')
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/image.png');
const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d');
const img = new Image()
const img2 = new Image()

img.onload = () => {
    ctx.fillStyle = "#000000";
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    ctx.drawImage(img, 0, 0,100,100);
    img2.onerror = err => {
        console.log(err)
    }
    img2.onload = () => {
        ctx.drawImage(img2, 150, 150, 100, 100);
        var stream = canvas.createPNGStream();
        stream.pipe(out);
    }
    img2.src = "https://img.alicdn.com/imgextra/i3/80/O1CN011CSgGWymNlCrNO3_!!80-2-luban.png"
}
img.onerror = err => {
    console.log(err)
}
img.src = "https://img.alicdn.com/imgextra/i3/80/O1CN011CSgGWymNlCrNO3_!!80-2-luban.png"
