image.onload = function() {
       var pattern = ctx.createPattern(image, "no-repeat");
       ctx.fillStyle = pattern;
       ctx.fillRect(0, 0, 100*ratio, 100*ratio);
 };

image.src = 'https://gw.alicdn.com/tfs/TB1iQoakAvoK1RjSZFwXXciCFXa-37-40.png';

