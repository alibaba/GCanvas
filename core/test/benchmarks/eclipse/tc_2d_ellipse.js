ctx.fillStyle = 'red';
ctx.beginPath();
ctx.ellipse(60 * ratio, 75 * ratio, 50 * ratio, 30 * ratio, Math.PI * .25, 0, Math.PI *1.5);
ctx.fill();

ctx.fillStyle = 'blue';
ctx.beginPath();
ctx.ellipse(150 * ratio, 75 * ratio, 50 * ratio, 30 * ratio, Math.PI * .25, 0, Math.PI);
ctx.fill();

ctx.fillStyle = 'green';
ctx.beginPath();
ctx.ellipse(240 * ratio, 75 * ratio, 50 * ratio, 30 * ratio, Math.PI * .25, 0, Math.PI, true);
ctx.fill();