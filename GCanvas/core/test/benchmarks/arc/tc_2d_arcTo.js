
ctx.fillStyle = '#000000';
ctx.fillRect(0, 0, c.width, c.height);

ctx.beginPath();
ctx.strokeStyle = "yellow";
ctx.moveTo(20*ratio, 20*ratio);           // 创建开始点
ctx.lineTo(100*ratio, 20*ratio);          // 创建水平线
ctx.arcTo(150*ratio, 20*ratio, 150*ratio, 70*ratio, 50*ratio); // 创建弧
ctx.lineTo(150*ratio, 120*ratio);         // 创建垂直线
ctx.stroke();
