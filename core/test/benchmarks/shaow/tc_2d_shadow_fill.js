
ctx.translate(100, 100);

ctx.fillStyle = "rgba(0,0,255, 0.5)";
ctx.shadowColor = "black";

ctx.shadowBlur = 15;
ctx.shadowOffsetX = 20;
ctx.shadowOffsetY = 20;

ctx.beginPath();
ctx.moveTo(50,50)
ctx.lineTo(100,200);
ctx.lineTo(100,50);
ctx.closePath();

//ctx.rect(50, 50, 50, 150);

ctx.fill();

