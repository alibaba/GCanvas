ctx.fillStyle = '#000000';
ctx.strokeStyle = "blue";
ctx.moveTo(10*ratio, 125*ratio);
ctx.lineTo(250*ratio, 125*ratio);
ctx.stroke();
ctx.font = 20*ratio+"px Arial"
ctx.textBaseline = "top";
ctx.fillText("Top", 10*ratio, 125*ratio);
ctx.textBaseline = "bottom";
ctx.fillText("Bottom", 50*ratio, 125*ratio);
ctx.textBaseline = "middle";
ctx.fillText("Middle", 100*ratio, 125*ratio);
ctx.textBaseline = "alphabetic";
ctx.fillText("Alphabetic", 150*ratio, 125*ratio);
ctx.textBaseline = "hanging";
ctx.fillText("Hanging", 200*ratio, 125*ratio);
