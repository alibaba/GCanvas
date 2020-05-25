const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('bindings')('canvas');

const canvas1 = createCanvas(150, 150);
const ctx1 = canvas1.getContext('2d');

const canvas2 = createCanvas(150, 150);
const ctx2 = canvas2.getContext('2d');

ctx2.fillStyle = '#ffec30' 
ctx1.fillStyle="#00ff32"
ctx1.fillRect(0,0,150,150);
ctx2.fillRect(0, 0, 150, 150) 



console.log("canvas one export");
canvas1.createPNG("canvas1")
console.log("canvas two export");
canvas2.createPNG("canvas2")