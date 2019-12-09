
var startAngle = Math.PI ;
var endAngle = Math.PI * 1.5;
ctx.save();
ctx.beginPath();
ctx.translate(414, 414);
ctx.arc(0, 0, 374, startAngle, endAngle);

ctx.save();
ctx.rotate(endAngle);
ctx.moveTo(374, 0);
ctx.lineTo(0, 0);

ctx.restore();
ctx.stroke();

ctx.restore();


