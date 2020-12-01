const { createCanvas, Image } = require('../export')
var canvas = createCanvas(400, 400);
var ctx = canvas.getContext('2d')

ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
ctx.save() // Save the default state

ctx.fillStyle = '#09F' // Make changes to the settings
ctx.fillRect(15, 15, 120, 120) // Draw a rectangle with new settings

ctx.save() // Save the current state
ctx.fillStyle = '#FFF' // Make changes to the settings
ctx.globalAlpha = 0.5
ctx.fillRect(30, 30, 90, 90) // Draw a rectangle with new settings

ctx.restore() // Restore previous state
ctx.fillRect(45, 45, 60, 60) // Draw a rectangle with restored settings

ctx.restore() // Restore original state
ctx.fillRect(60, 60, 30, 30) // Draw a rectangle with restored settings

canvas.width = 600;
canvas.height = 600;
ctx = canvas.getContext('2d')

ctx.fillRect(0, 0, 600, 600) // Draw a rectangle with default settings
ctx.save() // Save the default state

ctx.fillStyle = '#09F' // Make changes to the settings
ctx.fillRect(15, 15, 570, 570) // Draw a rectangle with new settings

ctx.save() // Save the current state
ctx.fillStyle = '#FFF' // Make changes to the settings
ctx.globalAlpha = 0.5
ctx.fillRect(30, 30, 540, 540) // Draw a rectangle with new settings

ctx.restore() // Restore previous state
ctx.fillRect(45, 45, 510, 510) // Draw a rectangle with restored settings

ctx.restore() // Restore original state
ctx.fillRect(60, 60, 460, 460) // Draw a rectangle with restored settings


const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/canvas-size.png');
const stream = canvas.createPNGStream();
stream.pipe(out);
