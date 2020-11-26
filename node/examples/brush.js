const fs = require('fs')
const path = require('path');
const out = fs.createWriteStream(path.join(__dirname, "..")+ '/brush.png');

const { createCanvas, Image } = require('../export')

function WriteFont(options) {
    var self = this;
    this.canvas = createCanvas(400, 600);
    var obj = {
        canvas: this.canvas,
        context: this.canvas.getContext("2d"),
        isWrite: false,
        lastWriteTime: -1,
        lastWriteSpeed: 0,
        lastWriteWidth: 0,
        canvasWidth: this.canvas.width,
        canvasHeight: this.canvas.height,
        isShowBorder: true,
        bgColor: '#fff',
        borderWidth: 6,
        borderColor: 'red',
        lastPoint: {},
        writeWidth: 6,
        maxWriteWidth: 30,
        minWriteWidth: 1,
        writeColor: '#000'
    }
    for (var name in options) {
        obj[name] = options[name];
    }
    this.setLineWidth = function () {
        var nowTime = new Date().getTime();
        var diffTime = nowTime - obj.lastWriteTime;
        obj.lastWriteTime = nowTime;
        var returnNum = obj.minWriteWidth + (obj.maxWriteWidth - obj.minWriteWidth) * diffTime / 30;
        if (returnNum < obj.minWriteWidth) {
            returnNum = obj.minWriteWidth;
        } else if (returnNum > obj.maxWriteWidth) {
            returnNum = obj.maxWriteWidth;
        }
        returnNum = returnNum.toFixed(2);
        obj.context.lineWidth = obj.lastWriteWidth = obj.lastWriteWidth / 4 * 3 + returnNum / 4;
    }

    this.writing = function (point) {

        obj.context.beginPath();
        obj.context.moveTo(obj.lastPoint.x, obj.lastPoint.y);
        obj.context.lineTo(point.x, point.y);
        self.setLineWidth();
        obj.context.stroke();
        obj.lastPoint = point;
        obj.context.closePath();
    }

    this.writeContextStyle = function () {
        obj.context.beginPath();
        obj.context.strokeStyle = obj.writeColor;
        obj.context.lineCap = 'round';
        obj.context.lineJoin = "round";
    }

    this.writeBegin = function (point) {
        obj.isWrite = true;
        obj.lastWriteTime = new Date().getTime();
        obj.lastPoint = point;
        self.writeContextStyle();
    }
    this.writeEnd = function () {
        obj.isWrite = false;
    }
    this.canvasClear = function () {
        obj.context.save();
        obj.context.strokeStyle = '#fff';
        obj.context.clearRect(0, 0, obj.canvasWidth, obj.canvasHeight);
        if (obj.isShowBorder) {
            console.log('show border');
            obj.context.beginPath();
            var size = obj.borderWidth / 2;
            //画外面的框
            obj.context.moveTo(size, size);
            obj.context.lineTo(obj.canvasWidth - size, size);
            obj.context.lineTo(obj.canvasWidth - size, obj.canvasHeight - size);
            obj.context.lineTo(size, obj.canvasHeight - size);
            obj.context.closePath();
            obj.context.lineWidth = obj.borderWidth;
            obj.context.strokeStyle = obj.borderColor;
            obj.context.stroke();
            //画里面的框
            obj.context.moveTo(0, 0);
            obj.context.lineTo(obj.canvasWidth, obj.canvasHeight);
            obj.context.lineTo(obj.canvasWidth, obj.canvasHeight / 2);
            obj.context.lineTo(obj.canvasWidth, obj.canvasHeight / 2);
            obj.context.lineTo(0, obj.canvasHeight / 2);
            obj.context.lineTo(0, obj.canvasHeight);
            obj.context.lineTo(obj.canvasWidth, 0);
            obj.context.lineTo(obj.canvasWidth / 2, 0);
            obj.context.lineTo(obj.canvasWidth / 2, obj.canvasHeight);
            obj.context.stroke();
        }
        obj.context.restore();
    }

    this.canvasInit = function () {
        this.canvas.width = obj.canvasWidth;
        this.canvas.height = obj.canvasHeight;
    }

    // this.canvasInit();
    // this.canvasClear();
    this.option = obj;
    obj.control = {
        clearCanvas: self.canvasClear
    };

    this.save = function () {
        var stream = obj.canvas.createPNGStream();
        stream.pipe(out);
    }
}

var writeCanvas = new WriteFont({
    borderWidth: 10,
    borderColor: '#ff6666'
});

writeCanvas.canvasInit();
writeCanvas.canvasClear();
writeCanvas.save();






