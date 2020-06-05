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
const fpsArr=[];
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

var mouse = {},
    draw, body, H, W, particlemotion, Particle, particles = [],
    randSpeed, particlesNum;

const { createCanvas, Image } = require('../export')
var c = createCanvas(400, 400);
var ctx = c.getContext('2d')

var W = c.width;
var H = c.height;

particlesNum = 100; //粒子数

particlemotion = function () {
    ctx.clearRect(0, 0, W, H);
    for (i = 0; i < particlesNum; i++) {
        draw(particles[i].x, particles[i].y, particles[i].r, particles[i].rgb);
        var x = particles[i].x - mouse.x,
            dx = Math.abs(x),
            y = particles[i].y - mouse.y,
            dy = Math.abs(y),
            dt = Math.sqrt(dx * dx + dy * dy);
        if (dt < particles[i].clr) {
            if (particles[i].beginDeg === 0) {
                particles[i].beginDeg = Math.acos(x / particles[i].clr);
            }
            particles[i].moveWithMouse(dt, x);
        } else {
            particles[i].move();
        }
    }
};

let render=null;
function imageLoad(){
    render =setInterval(() => {
        if (!time2Quit) {
            particlemotion();
            drawFps();
        }else{
            outputRender2Png(c);
            clearInterval(render);
        }
    }, 16);
}

var image = new Image();
image.onload = imageLoad;
image.src = 'https://gw.alicdn.com/tfs/TB1iQoakAvoK1RjSZFwXXciCFXa-37-40.png';

//FPS
FPSMeter.init();
function drawFps() {
    var result = FPSMeter.update();
    var fps = FPSMeter.getFramerate();
    if (fps > 0) {
        fpsArr.push(fps);
        var fpsStr = "FPS: " + fps + ",Count:" + particlesNum;
        // console.log(`the fps is ${fps}`)
        ctx.font = 20 + 'px';
        ctx.fillStyle = '#000';
        ctx.fillText(fpsStr, 20, 20);
    }
}


if (typeof (isWeb) == "undefined") {
} else {
    //web mouse event
    var onMouseMove = function (e) {
        e = e || window.event;
        mouse.x = e.pageX || 1;
        mouse.y = e.pageY || 1;
    };

    var onClick = function (e) {
        particlesNum += 100;
        createParticles(particlesNum);
    }

    c.addEventListener('mousemove', onMouseMove, false);
    c.addEventListener('click', onClick, false);
}


//  随机方法
randSpeed = function (speed) {
    return Math.round(Math.random()) ? Math.round(Math.random() * 9 + 1) : -Math.round(Math.random() * 9 + 1);
};

//  粒子构造器
Particle = function () {
    this.x = Math.round(Math.random() * W);//粒子初始x坐标
    this.y = Math.round(Math.random() * H);//粒子初始y坐标
    this.r = Math.round(Math.random() * 10 + 5);//粒子半径
    this.clr = this.r * 5.5;//绕鼠标转动半径
    this.rgb = {};//随机产生颜色
    this.rgb.r = Math.round(Math.random() * 255);
    this.rgb.g = Math.round(Math.random() * 255);
    this.rgb.b = Math.round(Math.random() * 255);
    this.speed_x = randSpeed();//x方向速度
    this.speed_y = randSpeed();//y方向速度
    this.speed_z = Math.random() + 0.5;//转动速度
    this.beginDeg = 0;
    //    默认移动方式
    this.move = function () {
        if (this.x < 0) {
            this.x = 0;
            this.speed_x = -this.speed_x;
        }
        if (this.x > W) {
            this.x = W;
            this.speed_x = -this.speed_x;
        }
        if (this.y < 0) {
            this.y = 0;
            this.speed_y = -this.speed_y;
        }
        if (this.y > H) {
            this.y = H;
            this.speed_y = -this.speed_y;
        }
        this.x += this.speed_x;
        this.y += this.speed_y;
        this.beginDeg = 0;
    };

    //    绕鼠标转动
    this.moveWithMouse = function (dt, x) {
        var rad = Math.PI / 180;
        this.x = mouse.x + dt * Math.cos(this.beginDeg * rad);

        this.y = mouse.y + dt * Math.sin(this.beginDeg * rad);
        if (this.beginDeg > 360 || this.beginDeg < -360) {
            this.beginDeg = 0.5;
        }
        if (x < 0 && this.speed_z > 0) {
            this.speed_z = -this.speed_z;
        }
        this.beginDeg += this.speed_z;
    };
};

createParticles = function (max) {
    if (particles.length < max) {
        //add particle
        for (var i = particles.length; i < max; i++) {
            particles.push(new Particle());
        }
    } else {
        //remove particle
        particles.splice(max, particles.length - max);
    }
}

createParticles(particlesNum);


draw = function (mouse_x, mouse_y, r, rgb) {
    //console.log("draw....x:" + mouse_x + ",y:" + mouse_y+",r:" +r +",rgb:("+rgb.r+","+rgb.g+","+rgb.b+")");
    //    var rgbcolor = "rgb(" + rgb.r + "," + rgb.g + "," + rgb.b + "," + ")";
    //    ctx.strokeStyle = "rgb(" + rgb.r + "," + rgb.g + "," + rgb.b + ")";
    //    ctx.beginPath();
    //    ctx.arc(mouse_x, mouse_y, r, 0, 2 * Math.PI);
    //    ctx.closePath();
    //    ctx.stroke();

    ctx.drawImage(image, mouse_x, mouse_y);
};


let time2Quit = false;

setTimeout(() => {
    time2Quit = true;
}, 3000);

function outputRender2Png(canvas){
    const path = require('path');
    const fs=require('fs');
    const out = fs.createWriteStream(path.join(__dirname, "..") + '/drawImageBechMark.png');
    var stream = canvas.createPNGStream();
    stream.on('data', function (chunk) {
        out.write(chunk);
    });
    let sum=0;
    fpsArr.forEach(function (oneFps, index, array) {
          sum+=oneFps;
    });
    console.log(`the avg fps is ${sum/fpsArr.length}`)
}



