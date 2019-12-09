
ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

var grd1 = ctx.createLinearGradient(10*ratio, 0, 100*ratio, 0);
grd1.addColorStop(0, "white");
grd1.addColorStop(1, "red");
ctx.fillStyle = grd1;
ctx.fillRect(10*ratio, 10*ratio, 100*ratio, 100*ratio);
