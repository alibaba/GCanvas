const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('../export');
const img = new Image()
const canvas = createCanvas(500, 500)
const ctx = canvas.getContext('2d')

img.onload = () => {
  ctx.drawImage(img, 0, 0)
  console.log("Image onload success!!!")
  canvas.createPNG("passedThroughGrayscale")
}
img.onerror = err => {
  console.log(err)
  throw err
}

img.src = path.join(__dirname, 'images', 'image.png')
