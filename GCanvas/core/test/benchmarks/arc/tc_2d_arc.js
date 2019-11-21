
ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

console.log("ratio is ======>" + ratio);

ctx.beginPath();
ctx.strokeStyle = "yellow";
ctx.arc(50*ratio, 75*ratio, 50*ratio, 0, 0.5 * Math.PI);
ctx.stroke();