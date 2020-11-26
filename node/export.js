/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
// const { createCanvas , createImage, registerParseFont} = require('bindings')('canvas');
const { createCanvas , createImage, registerParseFont} = require('./build/Release/canvas.node');
const { PNGStream } = require("./libs/pngstream");
const { JPGStream } = require('./libs/jpgstream');
const { parseFont } = require('./libs/parse-font');


registerParseFont(parseFont);

module.exports = {
    createCanvas: createCanvasInner,
    Image: createImage,
}
function createCanvasInner(width, height, usecairo) {
    let canvas = createCanvas(width, height, usecairo);
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


