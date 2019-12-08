ctx.lineWidth = 10;

ctx.beginPath();
ctx.rect(20, 20, 100, 100);
ctx.stroke();

ctx.translate(100, 100);
ctx.save();
ctx.moveTo(200, 200);
ctx.lineTo(250, 250);

ctx.lineTo(200, 0);

//translate1
ctx.translate(100, 0)
ctx.lineTo(200, 0);


ctx.restore();
ctx.stroke();


