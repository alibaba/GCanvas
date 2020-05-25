var Readable = require('stream').Readable;
var util = require('util');
class PNGStream {
    constructor(canvas, options) {
        this.canvas = canvas;
        this.options = options || {};
        Readable.call(this);
    }
}
function noop() { }
PNGStream.prototype._read = function _read() {
    this._read = noop;
    this.canvas.createPNGStreamSync((err, chunk, len) => {
        if (err) {
            this.emit('error', err);
        } else if (len) {
            this.push(chunk);
        } else {
            this.push(null);
        }
    }, this.options);
}
util.inherits(PNGStream, Readable);
module.exports.PNGStream = PNGStream;
