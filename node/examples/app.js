const { createCanvas, Image } = require('../export')
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/demo1.png');
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
var stream = canvas.createPNGStream();
stream.on('data', function (chunk) {
    out.write(chunk);
});