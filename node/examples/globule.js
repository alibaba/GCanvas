const { createCanvas, Image } = require('../export');
const canvas = createCanvas(1000, 600)
const ctx = canvas.getContext('2d')




function Circle(x, y, r, color) {
    this.x = x;
    this.y = y;
    this.r = r;
    // 颜色的取值范围
    this.color = "rgb(" + (parseInt(Math.random() * 240) + 9) + "," + (parseInt(Math.random() * 220) + 18) + ",203)";

    //随机方向
    this.dx = Math.random() * 12 - 7;
    this.dy = Math.random() * 12 - 7;
    //往数组中push自己
    circleArr.push(this);
}

Circle.prototype.render = function () {
    //新建一条路径
    ctx.beginPath();
    //创建一个圆
    ctx.arc(this.x, this.y, this.r, 0, Math.PI * 2, true);
    //设置样式颜色
    ctx.fillStyle = this.color;
    //通过填充路径的内容区域生成实心的图形
    ctx.fill();
}

Circle.prototype.update = function () {
    this.x += this.dx;
    this.y += this.dy;
    this.r--;
    if (this.r < 0) {
        for (var i = 0; i < circleArr.length; i++) {
            if (circleArr[i] === this) {
                circleArr.splice(i, 1);
            };
        }
        return false;
    }
    return true;
}
var circleArr = [];
var handler =  setInterval(function () {
    new Circle(parseInt(Math.random() * 1000), parseInt(Math.random() * 600), 30, "orange");
    new Circle(parseInt(Math.random() * 1000), parseInt(Math.random() * 600), 20, "red");
    ctx.clearRect(0, 0, 1000, 600)
    for (var i = 0; i < circleArr.length; i++) {
        circleArr[i].update() && circleArr[i].render();
    };
    if (circleArr.length > 40) {
        clearInterval(handler);
        const fs = require('fs')
        const path = require('path')
        const out = fs.createWriteStream(path.join(__dirname, "..")+ '/globule.png');
        var stream = canvas.createPNGStream();
        stream.pipe(out);
        return;
    }
    console.log("the arr length is " + circleArr.length);
}, 1000);



