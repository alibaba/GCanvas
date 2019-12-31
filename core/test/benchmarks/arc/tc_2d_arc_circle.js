ctx.lineWidth = 10;

ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

ctx.beginPath();
ctx.strokeStyle = "yellow";
ctx.arc(75*ratio, 75*ratio, 50*ratio, 0, 2 * Math.PI);
ctx.stroke();
