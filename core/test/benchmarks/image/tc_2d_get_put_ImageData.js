ctx.fillStyle="#000000"
ctx.rect(10 *ratio, 10 *ratio, 100 *ratio, 100 *ratio);
ctx.fill();

let imageData = ctx.getImageData(60 *ratio, 60 *ratio, 200 *ratio, 100 *ratio);
ctx.putImageData(imageData, 150 *ratio, 10 *ratio);

ctx.fillStyle = "red";
ctx.fillRect(50*ratio, 50*ratio, 50*ratio, 50*ratio);


let imgData = ctx.getImageData(50*ratio, 50*ratio, 50*ratio, 50*ratio);
ctx.putImageData(imgData, 50*ratio, 150*ratio);