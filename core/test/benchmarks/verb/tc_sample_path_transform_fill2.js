ctx.save();
ctx.scale(0.3,0.3);
ctx.beginPath();
ctx.arc(411, 411, 394, 0, 6.28);
ctx.fillStyle = "#fff";
ctx.strokeStyle = "#d5d5d5";
ctx.fill();

ctx.stroke();
ctx.save();
ctx.beginPath();
ctx.translate(414, 414);
ctx.arc(0, 0, 374, 3.94, 4.47);
ctx.save();
ctx.rotate(4.47);
ctx.moveTo(374, 0);
ctx.lineTo(0, 0);
ctx.restore();
ctx.rotate(3.94);
ctx.lineTo(374, 0);
ctx.fillStyle = "#EE5461";
ctx.fill();
ctx.stroke();

ctx.restore();


