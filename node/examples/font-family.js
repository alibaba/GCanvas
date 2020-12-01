const { createCanvas, Image } = require('../export');
const canvas = createCanvas(320, 320);
const ctx = canvas.getContext('2d')

if( canvas  ){
    console.log(  "canvas is not null")
}

ctx.font = 'normal normal 20px HuXiaoBoKuHei'
ctx.fillText('Quo Vaids?1', 0, 70)

ctx.font ="normal normal 40px HuXiaoBoKuHei"
ctx.fillText('Quo Vaids?4', 0, 280)



const fs = require('fs')
const path = require('path')
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/font-family.png');
var stream = canvas.createPNGStream();
stream.pipe(out);