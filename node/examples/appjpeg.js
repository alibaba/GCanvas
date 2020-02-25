const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
ctx.fillStyle = "#ff0000"
ctx.fillRect(0, 0, canvas.width, canvas.height) // Draw a rectangle with default settings

canvas.createPNG("demo1")

