ctx.save();
ctx.rect(10*ratio, 10*ratio, 100*ratio, 120*ratio);
ctx.stroke();
ctx.clip();
ctx.fillStyle = "green";
ctx.fillRect(0, 0, 150*ratio, 100*ratio);
ctx.restore();


ctx.resetClip();
ctx.fillStyle = "green";
ctx.fillRect(200*ratio, 0, 150*ratio, 100*ratio);
