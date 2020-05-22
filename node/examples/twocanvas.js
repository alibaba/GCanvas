const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('bindings')('canvas');

const canvas1 = createCanvas(150, 150);
const ctx1 = canvas1.getContext('2d');

const canvas2 = createCanvas(150, 150);
const ctx2 = canvas2.getContext('2d');

ctx1.fillStyle="#0000ff"
ctx1.fillRect(0,0,150,150);

ctx2.fillStyle = '#ff00ff' // Make changes to the settings
ctx2.fillRect(0, 0, 120, 120) // Draw a rectangle with new settings


canvas1.createPNG("canvas1")
canvas2.createPNG("canvas2")