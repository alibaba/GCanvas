// Tangential lines
ctx.beginPath();
ctx.strokeStyle = 'gray';
ctx.moveTo(200 * ratio, 20* ratio);
ctx.lineTo(200* ratio, 130* ratio);
ctx.lineTo(50* ratio, 20* ratio);
ctx.stroke();

// Arc
ctx.beginPath();
ctx.strokeStyle = 'black';
ctx.lineWidth = 5*ratio;
ctx.moveTo(200* ratio, 20* ratio);
ctx.arcTo(200* ratio,130* ratio, 50* ratio,20* ratio, 40* ratio);
ctx.stroke();

// Start point
ctx.beginPath();
ctx.fillStyle = 'blue';
ctx.arc(200* ratio, 20* ratio, 5* ratio, 0, 2 * Math.PI);
ctx.fill();

// Control points
ctx.beginPath();
ctx.fillStyle = 'red';
ctx.arc(200* ratio, 130* ratio, 5* ratio, 0, 2 * Math.PI); // Control point one
ctx.arc(50* ratio, 20* ratio, 5* ratio, 0, 2 * Math.PI);   // Control point two
ctx.fill();