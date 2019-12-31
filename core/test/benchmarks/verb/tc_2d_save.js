ctx.fillStyle = '#000000';
ctx.save();
ctx.fillStyle = 'red';
ctx.fillRect(10*ratio, 10*ratio, 150*ratio, 100*ratio);
ctx.restore();
ctx.fillRect(50*ratio, 50*ratio, 150*ratio, 100*ratio);
