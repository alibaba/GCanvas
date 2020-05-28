const { createCanvas, Image } = require('../export');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d');
const img = new Image()
const img2 = new Image()
const path = require('path')

img.onerror = err => {
    console.log(err)
}

img.onload = () => {
}
img.src = 'https://192.168.1.1:80/test.png'


img2.onload = () => {
    ctx.drawImage(img, 0, 0)
    console.log("Image onload success!!!")
    canvas.createPNG("passedThroughGrayscale")
}
img2.onerror = err => {
    console.log(err)
}

img2.src = path.join(__dirname, 'images', 'imageNotExist.png')
