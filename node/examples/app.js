const { createCanvas, Image } = require('bindings')('canvas');
const canvas = createCanvas(400, 400);
const ctx = canvas.getContext('2d')

// ctx.fillRect(0, 0, 150, 150) // Draw a rectangle with default settings
// ctx.save() // Save the default state

// ctx.fillStyle = '#09F' // Make changes to the settings
// ctx.fillRect(15, 15, 120, 120) // Draw a rectangle with new settings

// ctx.save() // Save the current state
// ctx.fillStyle = '#FFF' // Make changes to the settings
// ctx.globalAlpha = 0.5
// ctx.fillRect(30, 30, 90, 90) // Draw a rectangle with new settings

// ctx.restore() // Restore previous state
// ctx.fillRect(45, 45, 60, 60) // Draw a rectangle with restored settings

// ctx.restore() // Restore original state
// ctx.fillRect(60, 60, 30, 30) // Draw a rectangle with restored settings


var grd=ctx.createLinearGradient(0,0,170,0);
grd.addColorStop(0,"red");
grd.addColorStop(0.2,"orange");
grd.addColorStop(0.4,"yellow");
grd.addColorStop(0.6,"green");
grd.addColorStop(0.8,"blue");
grd.addColorStop(1,"purple");


ctx.fillStyle=grd;
ctx.fillRect(0,0,170,100);
canvas.createPNG("demo1")

