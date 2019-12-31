ctx.translate(100, 100);

ctx.shadowColor = "green";
ctx.shadowBlur = 20;
ctx.shadowOffsetX = 20;
ctx.shadowOffsetY = 20;

var image = tbplay.createImage();
image.src = "https://gw.alicdn.com/tfs/TB1KwRTlh6I8KJjy0FgXXXXzVXa-225-75.png";
image.onload = function(){
    ctx.drawImage(image, 10, 10);
}



