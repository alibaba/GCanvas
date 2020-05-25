const { Canvas, Image } = require('bindings')('canvas');
const { PNGStream } = require("./stream/pngstream");
module.exports = {
    createCanvas: createCanvas,
    Image: Image,
}
Canvas.prototype.createPNGStream = function (options) {
    return new PNGStream(this, options);
}
function createCanvas(width, height) {
    return new Canvas(width, height);
}




