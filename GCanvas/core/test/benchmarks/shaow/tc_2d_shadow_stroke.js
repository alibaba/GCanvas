ctx.translate(100, 100);

ctx.strokeStyle = "red";
ctx.shadowColor = "black";

ctx.shadowBlur = 15;
ctx.shadowOffsetX = 20;
ctx.shadowOffsetY = 20;

ctx.beginPath()
ctx.moveTo(50,50)
ctx.lineTo(100,200);
ctx.lineTo(100,50);
ctx.closePath();

ctx.stroke()
