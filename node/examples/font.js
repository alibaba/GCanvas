const { createCanvas, Image } = require('../export');
const canvas = createCanvas(320, 320);
const ctx = canvas.getContext('2d')

ctx.font = 'normal normal 50px Helvetica'

ctx.fillText('Quo Vaids?', 0, 70)

ctx.font = 'bold 50px pfennigFont'
ctx.fillText('Quo Vaids?', 0, 140, 100)

ctx.font = 'italic 50px pfennigFont'
ctx.fillText('Quo Vaids?', 0, 210)

ctx.font = 'bold italic 50px pfennigFont'
ctx.fillText('Quo Vaids?', 0, 280)

const fs = require('fs')
const path = require('path')
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/font.png');
var stream = canvas.createPNGStream();
stream.pipe(out);