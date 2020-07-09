////////////////////////////////////////////////////////////////////////////
//
//  工具类
//
////////////////////////////////////////////////////////////////////////////
var TimeUtil = {
    startTime: Now(),
    getTimer: function () {
        return Now() - TimeUtil.startTime;
    }
}

/**
 * FPS测试工具
 */
var FPSMeter = {
    init: function () {
        this.lastFrameTime = TimeUtil.getTimer();
        this.lastSampledTime = TimeUtil.getTimer();
        this.sampleFrames = 0;
        this.framerate = 0;
        this.timeDeltaS = 0.1;
        this.sampleFrameThreshold = 600;
    },

    formatNumber: function (val) {
        return Math.floor(val);
    },

    update: function () {
        this.timeDeltaS = (TimeUtil.getTimer() - this.lastFrameTime) / 1000;
        this.lastFrameTime = TimeUtil.getTimer();
        if (++this.sampleFrames >= this.sampleFrameThreshold) {
            this.framerate = this.getFramerate();
            var frames = this.sampleFrames;
            this.sampleFrames = 0;
            this.lastSampledTime = TimeUtil.getTimer();
            return { "framerate": this.framerate, "frames": frames };
        }
        return { "framerate": 0 };
    },

    getFramerate: function () {
        var diff = TimeUtil.getTimer() - this.lastSampledTime;
        var rawFPS = this.sampleFrames / (diff / 1000);
        var sampleFPS = this.formatNumber(rawFPS);
        return sampleFPS;
    },

    getTimeDelta: function () {
        return this.timeDeltaS;
    }
}

function Now() {
    if (typeof (isWeb) != "undefined") {
        return performance.now();
    } else {
        return Math.floor(Date.now());
    }
}

///////////////////////////
// TestCase
///////////////////////////
const { createCanvas, Image } = require('../export')
const c = createCanvas(400, 400);
const ctx = c.getContext('2d')
var width = c.width;
var height = c.height;
const fpsArr = [];
//FPS
FPSMeter.init();
function drawFps() {
    var result = FPSMeter.update();
    var fps = FPSMeter.getFramerate();
    if (fps > 0) {
        fpsArr.push(fps);
        // var fpsStr = "FPS: " + fps;
        // // console.log(`the fps is ${fps}`)
        // ctx.font = 20 + 'px';
        // ctx.fillStyle = '#000';
        // ctx.fillText(fpsStr, 20, 20);
    }
}

var count = 5000;
var infoArray = {};
for (var i = 0; i < count; i++) {
    var info = {};
    info.r = Math.floor(Math.random() * 255);
    info.g = Math.floor(Math.random() * 255);
    info.b = Math.floor(Math.random() * 255);
    infoArray[i] = info;
}

function drawFrame() {
    // ctx.fillStyle = '#09F' // Make changes to the settings
    for (var i = 0; i < count; i++) {
        var info = infoArray[i];
        ctx.fillStyle = "rgb(" + info.r + "," + info.g + "," + info.b + ")"
        var x = Math.random() * width;
        var y = Math.random() * height;
        ctx.fillRect(x, y, 20, 20);
    }
}

let time2Quit = false;

setTimeout(() => {
    time2Quit = true;
}, 3000);

function requestAnimationFrame(fun) {
    if (!time2Quit) {
        setImmediate(() => fun(Date.now()))
    } else {
        const path = require('path');
        const fs = require('fs');
        const out = fs.createWriteStream(path.join(__dirname, "..") + '/fillRectBechMark.png');
        var stream = c.createPNGStream();
        stream.on('data', function (chunk) {
            out.write(chunk);
        });
        let sum = 0;
        fpsArr.forEach(function (oneFps, index, array) {
            sum += oneFps;
        });
        if (fpsArr.length != 0) {
            console.log(`the avg fps is ${sum / fpsArr.length}`);
        }
    }
}


var loop = function () {
    ctx.clearRect(0, 0, width, height);
    ctx.fillStyle = '#09F'
    drawFrame();
    drawFps();
    requestAnimationFrame(loop);
}
loop();