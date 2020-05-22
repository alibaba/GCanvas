const fs = require('fs')
const path = require('path')
const { createCanvas, Image } = require('bindings')('canvas');

const canvas1 = createCanvas(150, 150);
const ctx1 = canvas1.getContext('2d');

const canvas2 = createCanvas(150, 150);
const ctx2 = canvas2.getContext('2d');



ctx1.fillStyle="#00ff00"
console.log("ctx1 fillrect");
ctx1.fillRect(0,0,150,150);


ctx2.fillStyle = '#ffec00' 
console.log("ctx2 fillrect");
ctx2.fillRect(0, 0, 150, 150) 


console.log("canvas one export");
canvas1.createPNG("canvas1")
console.log("canvas two export");
canvas2.createPNG("canvas2")