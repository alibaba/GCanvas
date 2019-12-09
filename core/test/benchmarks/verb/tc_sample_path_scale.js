
ctx.lineWidth = 10;

ctx.beginPath();
ctx.translate(100, 100);

ctx.save();
ctx.moveTo(40, 40);
ctx.lineTo(50, 50);

//before scale
ctx.lineTo(40, 0);
ctx.moveTo(50, 50)

//scale
ctx.scale(2, 2);
ctx.lineTo(40, 0);

ctx.restore();

ctx.stroke();



