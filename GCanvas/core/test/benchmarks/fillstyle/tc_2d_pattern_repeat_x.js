image.onload = function(){
    var pattern1 = ctx.createPattern(image, 'repeat-x');
    ctx.fillStyle = pattern1;
    ctx.fillRect(0, 0, 200*ratio, 150*ratio,image.width*ratio,image.height*ratio);
}
image.src = 'https://gw.alicdn.com/tfs/TB1iQoakAvoK1RjSZFwXXciCFXa-37-40.png';
