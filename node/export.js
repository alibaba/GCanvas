const { Canvas, Image } = require('bindings')('canvas');
const { PNGStream } = require("./stream/pngstream");
const {JPGStream} =require('./stream/jpgstream')
module.exports = {
    createCanvas: createCanvas,
    Image: Image,
}
Canvas.prototype.createPNGStream = function (options) {
    return new PNGStream(this, options);
}
Canvas.prototype.createJPEGStream = function (options) {
    return new JPGStream(this, options);
}
function createCanvas(width, height) {
    return new Canvas(width, height);
}





