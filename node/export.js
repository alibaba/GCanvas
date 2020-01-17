const { createCanvas, Image } = require('bindings')('canvas');

module.exports = {
    createCanvas: createCanvas,
    Image: Image,
    test:"hello world"
}