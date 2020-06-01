/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
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





