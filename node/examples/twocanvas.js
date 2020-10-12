const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('../export');

const canvas1 = createCanvas(150, 150);
const ctx1 = canvas1.getContext('2d');

const canvas2 = createCanvas(150, 150);
const ctx2 = canvas2.getContext('2d');
ctx1.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
ctx1.save() // Save the default state

ctx2.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
ctx2.save() // Save the default state

ctx1.fillStyle = '#09F' // Make changes to the settings
ctx1.fillRect(15, 15, 120, 120) // Draw a rectangle with new settings

ctx2.fillStyle = '#09F' // Make changes to the settings
ctx2.fillRect(15, 15, 120, 120) // Draw a rectangle with new settings

ctx1.save() // Save the current state
ctx1.fillStyle = '#FFF' // Make changes to the settings
ctx1.globalAlpha = 0.5
ctx1.fillRect(30, 30, 90, 90) // Draw a rectangle with new settings

ctx2.save() // Save the current state
ctx2.fillStyle = '#FFF' // Make changes to the settings
ctx2.globalAlpha = 0.5
ctx2.fillRect(30, 30, 90, 90) // Draw a rectangle with new settings

ctx1.restore() // Restore previous state
ctx1.fillRect(45, 45, 60, 60) // Draw a rectangle with restored settings

ctx2.restore() // Restore previous state
ctx2.fillRect(45, 45, 60, 60) // Draw a rectangle with restored settings

ctx1.restore() // Restore original state
ctx1.fillRect(60, 60, 30, 30) // Draw a rectangle with restored settings

ctx2.restore() // Restore original state
ctx2.fillRect(60, 60, 30, 30) // Draw a rectangle with restored settings


canvas1.createPNG("canvas1")
canvas2.createPNG("canvas2")
