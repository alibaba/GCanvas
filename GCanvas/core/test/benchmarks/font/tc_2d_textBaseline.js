const baselines = ['top', 'hanging', 'middle', 'alphabetic', 'ideographic', 'bottom'];
ctx.font = 15*ratio+'px serif';
ctx.strokeStyle = 'red';

baselines.forEach(function (baseline, index) {
  ctx.textBaseline = baseline;
  let y = 75*ratio + index * 75*ratio;
  ctx.beginPath();
  ctx.moveTo(0, y + 0.5);
  ctx.lineTo(550*ratio, y + 0.5);
  ctx.stroke();
  ctx.fillText('Abcdefghijklmnop (' + baseline + ')', 0, y);
});

