const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(320, 320);
const ctx = canvas.getContext('2d')

ctx.font = 'normal normal 20px HuXiaoBoKuHei'
ctx.fillText('Quo Vaids?1', 0, 70)

ctx.font ="normal normal 40px HuXiaoBoKuHei"
ctx.fillText('Quo Vaids?4', 0, 280)

canvas.createPNG("fontFamily");