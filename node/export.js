const { createCanvas, Image } = require('bindings')('canvas');
const { PNGStream } = require("./stream/pngstream");
const { JPGStream } = require('./stream/jpgstream')
module.exports = {
    createCanvas: createCanvasInner,
    Image: Image,
}
function createCanvasInner(width, height) {
    let canvas = createCanvas(width, height);
    Object.defineProperty(canvas, 'createPNGStream', {
        value: function (options) {
            return new PNGStream(canvas, options);
        }
    })
    Object.defineProperty(canvas, 'createJPEGStream', {
        value: function (options) {
            return new JPGStream(canvas, options);
        }
    })
    return canvas;
}





