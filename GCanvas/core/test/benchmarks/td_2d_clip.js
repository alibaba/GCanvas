ctx.fillStyle = "#000000";
ctx.fillRect(0, 0, c.width, c.height);
ctx.fillStyle = "#ff0000";
ctx.strokeStyle = "#fff";

ctx.beginPath();
ctx.arc(150*ratio, 150*ratio, 75*ratio, 0*ratio, 3.14 * 2, false);
ctx.clip();

ctx.fillRect(0, 0, 200*ratio, 200*ratio);
ctx.fillStyle = "#00ff00";
ctx.fillRect(180*ratio, 180*ratio, 200*ratio, 200*ratio);