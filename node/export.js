const { Canvas, Image } = require('bindings')('canvas');
module.exports = {
    createCanvas: createCanvas,
    Image: Image,
}
function createCanvas(width, height) {
   return new Canvas(width, height);
}


