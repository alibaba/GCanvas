ctx.fillStyle = "#000";
ctx.fillRect(0, 0, c.width, c.height);

ctx.beginPath();
ctx.arc(150, 150, 120, 0, 6.28);
ctx.fillStyle = "#fff";
ctx.fill();

ctx.save();
ctx.beginPath();
ctx.translate(150, 150);
ctx.fillStyle = "#ff0000";
var degree = 1.0;
ctx.arc(0, 0, 100, 0, degree);
ctx.save();
ctx.rotate(degree);
ctx.moveTo(100, 0);
ctx.lineTo(0, 0);
ctx.restore();
ctx.rotate(0);
ctx.lineTo(100, 0);
ctx.fill();

ctx.restore();

