const { createCanvas, Image } = require('../export')
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')
const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/emptyCanvas.png');
var stream=canvas.createPNGStream();
stream.pipe(out);