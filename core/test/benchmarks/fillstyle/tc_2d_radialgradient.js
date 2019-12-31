
ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

var grd1 = ctx.createRadialGradient(75*ratio, 50*ratio, 5*ratio, 90*ratio, 60*ratio, 100*ratio);
grd1.addColorStop(0, "red");
grd1.addColorStop(1, "white");
ctx.fillStyle = grd1;
ctx.fillRect(10*ratio, 10*ratio, 100*ratio, 100*ratio);
