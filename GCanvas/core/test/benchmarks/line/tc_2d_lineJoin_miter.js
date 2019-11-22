ctx.beginPath();
ctx.lineWidth = 10*ratio;
ctx.lineJoin = 'miter';
ctx.miterLimit = 5*ratio;
ctx.moveTo(10*ratio, 10*ratio);
ctx.lineTo(200*ratio, 100*ratio);
ctx.lineTo(50*ratio, 200*ratio);
ctx.stroke();

