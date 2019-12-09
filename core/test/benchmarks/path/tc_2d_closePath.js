
ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

ctx.beginPath();
ctx.strokeStyle = "yellow";
ctx.moveTo(10*ratio , 10 *ratio);
ctx.lineTo(100*ratio , 100 *ratio);
ctx.lineTo(70 *ratio, 200*ratio );
ctx.closePath();
ctx.stroke();
