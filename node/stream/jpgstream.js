/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
var Readable = require('stream').Readable;
var util = require('util');
class JPGStream {
    constructor(canvas, options) {
        this.canvas = canvas;
        this.options = options || {};
        Readable.call(this);
    }
}
function noop() { }
JPGStream.prototype._read = function _read() {
    this._read = noop;
    this.canvas.createJPGStreamSync((err, chunk, len) => {
        if (err) {
            this.emit('error', err);
        } else if (len) {
            this.push(chunk);
        } else {
            this.push(null);
        }
    }, this.options);
}
util.inherits(JPGStream, Readable);
module.exports.JPGStream = JPGStream;
