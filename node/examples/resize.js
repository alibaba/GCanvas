const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('../export');

var img = new Image()

img.onerror = err => {
  console.log("Image onload error!!!")
  throw err
}



img.onload = () => {
  console.log("Image onload success!!!")
  var width = 100
  var height = 100
  var canvas = createCanvas(width, height)
  var ctx = canvas.getContext('2d')

  // ctx.imageSmoothingEnabled = true
  ctx.drawImage(img, 0, 0, width, height)
  
  canvas.createPNG("resize")
}

img.src = path.join(__dirname, 'images', 'squid.png')
