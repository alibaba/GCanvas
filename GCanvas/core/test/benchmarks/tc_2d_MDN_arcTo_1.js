// Tangential lines
ctx.beginPath();
ctx.strokeStyle = 'gray';
ctx.moveTo(200 * 1, 20* 1);
ctx.lineTo(200* 1, 130* 1);
ctx.lineTo(50* 1, 20* 1);
ctx.stroke();

// Arc
ctx.beginPath();
ctx.strokeStyle = 'black';
ctx.lineWidth = 5*1;
ctx.moveTo(200* 1, 20* 1);
ctx.arcTo(200* 1,130* 1, 50* 1,20* 1, 40* 1);
ctx.stroke();

// Start point
ctx.beginPath();
ctx.fillStyle = 'blue';
ctx.arc(200* 1, 20* 1, 5* 1, 0, 2 * Math.PI);
ctx.fill();

// Control points
ctx.beginPath();
ctx.fillStyle = 'red';
ctx.arc(200* 1, 130* 1, 5* 1, 0, 2 * Math.PI); // Control point one
ctx.arc(50* 1, 20* 1, 5* 1, 0, 2 * Math.PI);   // Control point two
ctx.fill();