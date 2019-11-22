ctx.fillStyle = '#000000';
ctx.font = 30*ratio+"px Arial";
var txt = "Hello World"
ctx.fillText(txt, 10*ratio, 50*ratio);
var textWidth = ctx.measureText(txt).width;
ctx.fillText("width:" + textWidth.toFixed(1), 10*ratio, 100*ratio);

ctx.fillStyle="#ff0000"
ctx.fillRect(10*ratio, 60*ratio, textWidth, 5*ratio);
