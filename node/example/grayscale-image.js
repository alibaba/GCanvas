const fs = require('fs')
const path = require('path')
// const Canvas = require('..')
const { createCanvas, Image } = require('bindings')('canvas');
const img = new Image()
const canvas = createCanvas(500, 500)
const ctx = canvas.getContext('2d')

img.onload = () => {
  ctx.drawImage(img, 0, 0)
  canvas.createPNG("local")
  // canvas.createJPEGStream().pipe(fs.createWriteStream(path.join(__dirname, 'passedThroughGrayscale.jpg')))
}
img.onerror = err => {
  throw err
}

img.src = path.join(__dirname, 'images', 'image.png')
