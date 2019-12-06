
ctx.lineWidth = 10;

ctx.beginPath();
ctx.translate(100, 100);

ctx.save();
ctx.moveTo(200, 200);
ctx.lineTo(250, 250);

//rotate
ctx.setTransform(1, 0.5, -0.5, 1, 30*ratio, 10*ratio);
ctx.lineTo(200, 0);

ctx.restore();

ctx.stroke();




