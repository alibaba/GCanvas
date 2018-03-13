import FillStylePattern from './FillStylePattern';
import FillStyleLinearGradient from './FillStyleLinearGradient';
import FillStyleRadialGradient from './FillStyleRadialGradient';

export default class CanvasRenderingContext2D {

    _drawCommands = '';

    _globalAlpha = 1.0;

    _fillStyle = 'rgb(0,0,0)';
    _strokeStyle = 'rgb(0,0,0)';

    _lineWidth = 1;
    _lineCap = 'butt';
    _lineJoin = 'miter';

    _miterLimit = 10;

    _globalCompositeOperation = 'source-over';

    _textAlign = 'start';
    _textBaseline = 'alphabetic';

    _font = '10px sans-serif';

    _savedGlobalAlpha = [];

    timer = null;
    componentId = null;

    // _imageMap = new GHashMap();
    // _textureMap = new GHashMap();

    constructor() {
        this.className = 'CanvasRenderingContext2D';
    }

    set fillStyle(value) {
        this._fillStyle = value;

        if (typeof (value) == 'string') {
            this._drawCommands = this._drawCommands.concat("F" + value + ";");
        } else if (value instanceof FillStylePattern) {
            const image = value._img;
            CanvasRenderingContext2D.GBridge.bindImageTexture(this.componentId, image.src, image._id);
            this._drawCommands = this._drawCommands.concat("G" + image._id + "," + value._style + ";");
        }
        else if (value instanceof FillStyleLinearGradient) {
            var command = "D" + value._start_pos._x.toFixed(2) + "," + value._start_pos._y.toFixed(2) + ","
                + value._end_pos._x.toFixed(2) + "," + value._end_pos._y.toFixed(2) + ","
                + value._stop_count;
            for (var i = 0; i < value._stop_count; ++i) {
                command += ("," + value._stops[i]._pos + "," + value._stops[i]._color);
            }
            this._drawCommands = this._drawCommands.concat(command + ";");
        }
        else if (value instanceof FillStyleRadialGradient) {
            var command = "H" + value._start_pos._x.toFixed(2) + "," + value._start_pos._y.toFixed(2) + "," + value._start_pos._r.toFixed(2) + ","
                + value._end_pos._x.toFixed(2) + "," + value._end_pos._y.toFixed(2) + "," + value._end_pos._r.toFixed(2) + ","
                + value._stop_count;
            for (var i = 0; i < value._stop_count; ++i) {
                command += ("," + value._stops[i]._pos + "," + value._stops[i]._color);
            }
            this._drawCommands = this._drawCommands.concat(command + ";");
        }
    }

    get fillStyle() {
        return this._fillStyle;
    }

    get globalAlpha() {
        return this._globalAlpha;
    }

    set globalAlpha(value) {
        this._globalAlpha = value;
        this._drawCommands = this._drawCommands.concat("a" + value.toFixed(2) + ";");
    }


    get strokeStyle() {
        return this._strokeStyle;
    }

    set strokeStyle(value) {

        this._strokeStyle = value;

        if (typeof (value) == 'string') {
            this._drawCommands = this._drawCommands.concat("S" + value + ";");
        }
        else if (value instanceof FillStylePattern) {
            const image = value._img;
            CanvasRenderingContext2D.GBridge.bindImageTexture(this.componentId, image.src, image._id);
            this._drawCommands = this._drawCommands.concat("G" + image._id + "," + value._style + ";");
        }
        else if (value instanceof FillStyleLinearGradient) {
            var command = "D" + value._start_pos._x.toFixed(2) + "," + value._start_pos._y.toFixed(2) + ","
                + value._end_pos._x.toFixed(2) + "," + value._end_pos._y.toFixed(2) + ","
                + value._stop_count;

            for (var i = 0; i < value._stop_count; ++i) {
                command += ("," + value._stops[i]._pos + "," + value._stops[i]._color);
            }
            this._drawCommands = this._drawCommands.concat(command + ";");
        }
        else if (value instanceof FillStyleRadialGradient) {
            var command = "H" + value._start_pos._x.toFixed(2) + "," + value._start_pos._y.toFixed(2) + "," + value._start_pos._r.toFixed(2) + ","
                + value._end_pos._x.toFixed(2) + "," + value._end_pos._y + ",".toFixed(2) + value._end_pos._r.toFixed(2) + ","
                + value._stop_count;

            for (var i = 0; i < value._stop_count; ++i) {
                command += ("," + value._stops[i]._pos + "," + value._stops[i]._color);
            }
            this._drawCommands = this._drawCommands.concat(command + ";");
        }
    }

    get lineWidth() {
        return this._lineWidth;
    }

    set lineWidth(value) {
        this._lineWidth = value;
        this._drawCommands = this._drawCommands.concat("W" + value + ";");
    }

    get lineCap() {
        return this._lineCap;
    }

    set lineCap(value) {
        this._lineCap = value;
        this._drawCommands = this._drawCommands.concat("C" + value + ";");
    }

    get lineJoin() {
        return this._lineJoin;
    }

    set lineJoin(value) {
        this._lineJoin = value;
        this._drawCommands = this._drawCommands.concat("J" + value + ";");
    }

    get miterLimit() {
        return this._miterLimit;
    }

    set miterLimit(value) {
        this._miterLimit = value;
        this._drawCommands = this._drawCommands.concat("M" + value + ";");
    }

    get globalCompositeOperation() {
        return this._globalCompositeOperation;
    }

    set globalCompositeOperation(value) {

        this._globalCompositeOperation = value;
        let mode = 0;
        switch (value) {
            case "source-over":
                mode = 0;
                break;
            case "source-atop":
                mode = 5;
                break;
            case "source-in":
                mode = 0;
                break;
            case "source-out":
                mode = 2;
                break;
            case "destination-over":
                mode = 4;
                break;
            case "destination-atop":
                mode = 4;
                break;
            case "destination-in":
                mode = 4;
                break;
            case "destination-out":
                mode = 3;
                break;
            case "lighter":
                mode = 1;
                break;
            case "copy":
                mode = 2;
                break;
            case "xor":
                mode = 6;
                break;
            default:
                mode = 0;
        }

        this._drawCommands = this._drawCommands.concat("B" + mode + ";");
    }

    get textAlign() {
        return this._textAlign;
    }

    set textAlign(value) {

        this._textAlign = value;
        let Align = 0;
        switch (value) {
            case "start":
                Align = 0;
                break;
            case "end":
                Align = 1;
                break;
            case "left":
                Align = 2;
                break;
            case "center":
                Align = 3;
                break;
            case "right":
                Align = 4;
                break;
            default:
                Align = 0;
        }

        this._drawCommands = this._drawCommands.concat("A" + Align + ";");
    }

    get textBaseline() {
        return this._textBaseline;
    }

    set textBaseline(value) {
        this._textBaseline = value;
        let baseline = 0;
        switch (value) {
            case "alphabetic":
                baseline = 0;
                break;
            case "middle":
                baseline = 1;
                break;
            case "top":
                baseline = 2;
                break;
            case "hanging":
                baseline = 3;
                break;
            case "bottom":
                baseline = 4;
                break;
            case "ideographic":
                baseline = 5;
                break;
            default:
                baseline = 0;
                break;
        }

        this._drawCommands = this._drawCommands.concat("E" + baseline + ";");
    }

    get font() {
        return this._font;
    }

    set font(value) {
        this._font = value;
        this._drawCommands = this._drawCommands.concat("j" + value + ";");
    }

    setTransform(a, b, c, d, tx, ty) {
        this._drawCommands = this._drawCommands.concat("t"
            + (a === 1 ? "1" : a.toFixed(2)) + ","
            + (b === 0 ? "0" : b.toFixed(2)) + ","
            + (c === 0 ? "0" : c.toFixed(2)) + ","
            + (d === 1 ? "1" : d.toFixed(2)) + "," + tx.toFixed(2) + "," + ty.toFixed(2) + ";");
    }

    transform(a, b, c, d, tx, ty) {
        this._drawCommands = this._drawCommands.concat("f"
            + (a === 1 ? "1" : a.toFixed(2)) + ","
            + (b === 0 ? "0" : b.toFixed(2)) + ","
            + (c === 0 ? "0" : c.toFixed(2)) + ","
            + (d === 1 ? "1" : d.toFixed(2)) + "," + tx + "," + ty + ";");
    }

    resetTransform() {
        this._drawCommands = this._drawCommands.concat("m;");
    }

    scale(a, d) {
        this._drawCommands = this._drawCommands.concat("k" + a.toFixed(2) + ","
            + d.toFixed(2) + ";");
    }

    rotate(angle) {
        this._drawCommands = this._drawCommands
            .concat("r" + angle.toFixed(6) + ";");
    }

    translate(tx, ty) {
        this._drawCommands = this._drawCommands.concat("l" + tx.toFixed(2) + "," + ty.toFixed(2) + ";");
    }

    save() {
        this._savedGlobalAlpha.push(this._globalAlpha);
        this._drawCommands = this._drawCommands.concat("v;");
    }

    restore() {
        this._drawCommands = this._drawCommands.concat("e;");
        this._globalAlpha = this._savedGlobalAlpha.pop();
    }

    createPattern(img, pattern) {
        return new FillStylePattern(img, pattern);
    }

    createLinearGradient(x0, y0, x1, y1) {
        return new FillStyleLinearGradient(x0, y0, x1, y1);
    }

    createRadialGradient = function (x0, y0, r0, x1, y1, r1) {
        return new FillStyleRadialGradient(x0, y0, r0, x1, y1, r1);
    };

    strokeRect(x, y, w, h) {
        this._drawCommands = this._drawCommands.concat("s" + x + "," + y + "," + w + "," + h + ";");
    }



    clearRect(x, y, w, h) {
        this._drawCommands = this._drawCommands.concat("c" + x + "," + y + "," + w
            + "," + h + ";");
    }

    clip() {
        this._drawCommands = this._drawCommands.concat("p;");
    }

    resetClip() {
        this._drawCommands = this._drawCommands.concat("q;");
    }

    closePath() {
        this._drawCommands = this._drawCommands.concat("o;");
    }

    moveTo(x, y) {
        this._drawCommands = this._drawCommands.concat("g" + x.toFixed(2) + "," + y.toFixed(2) + ";");
    }

    lineTo(x, y) {
        this._drawCommands = this._drawCommands.concat("i" + x.toFixed(2) + "," + y.toFixed(2) + ";");
    }

    quadraticCurveTo = function (cpx, cpy, x, y) {
        this._drawCommands = this._drawCommands.concat("u" + cpx + "," + cpy + "," + x + "," + y + ";");
    }

    bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y, ) {
        this._drawCommands = this._drawCommands.concat(
            "z" + cp1x.toFixed(2) + "," + cp1y.toFixed(2) + "," + cp2x.toFixed(2) + "," + cp2y.toFixed(2) + "," +
            x.toFixed(2) + "," + y.toFixed(2) + ";");
    }

    arcTo(x1, y1, x2, y2, radius) {
        this._drawCommands = this._drawCommands.concat("h" + x1 + "," + y1 + "," + x2 + "," + y2 + "," + radius + ";");
    }

    beginPath() {
        this._drawCommands = this._drawCommands.concat("b;");
    }


    fillRect(x, y, w, h) {
        this._drawCommands = this._drawCommands.concat("n" + x + "," + y + "," + w
            + "," + h + ";");
    }

    rect(x, y, w, h) {
        this._drawCommands = this._drawCommands.concat("w" + x + "," + y + "," + w + "," + h + ";");
    }

    fill() {
        this._drawCommands = this._drawCommands.concat("L;");
    }

    stroke(path) {
        this._drawCommands = this._drawCommands.concat("x;");
    }

    arc(x, y, radius, startAngle, endAngle, anticlockwise) {

        let ianticlockwise = 0;
        if (anticlockwise) {
            ianticlockwise = 1;
        }

        this._drawCommands = this._drawCommands.concat(
            "y" + x.toFixed(2) + "," + y.toFixed(2) + ","
            + radius.toFixed(2) + "," + startAngle + "," + endAngle + "," + ianticlockwise
            + ";"
        );
    }

    fillText(text, x, y) {
        let tmptext = text.replace(/!/g, "!!");
        tmptext = tmptext.replace(/,/g, "!,");
        tmptext = tmptext.replace(/;/g, "!;");
        this._drawCommands = this._drawCommands.concat("T" + tmptext + "," + x + "," + y + ",0.0;");
    }

    strokeText = function (text, x, y) {
        let tmptext = text.replace(/!/g, "!!");
        tmptext = tmptext.replace(/,/g, "!,");
        tmptext = tmptext.replace(/;/g, "!;");
        this._drawCommands = this._drawCommands.concat("U" + tmptext + "," + x + "," + y + ",0.0;");
    }

    measureText = function (text) {
        throw new Error('GCanvas not supported yet');
    }

    isPointInPath = function (x, y) {
        throw new Error('GCanvas not supported yet');
    }

    drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh) {
        const numArgs = arguments.length;
        function drawImageCommands() {

            if (numArgs === 3) {
                const x = parseFloat(sx) || 0.0;
                const y = parseFloat(sy) || 0.0;

                return ("d" + image._id + ",0,0,"
                    + image.width + "," + image.height + ","
                    + x + "," + y + "," + image.width + "," + image.height + ";");
            } else if (numArgs === 5) {
                const x = parseFloat(sx) || 0.0;
                const y = parseFloat(sy) || 0.0;
                const width = parseInt(sw) || image.width;
                const height = parseInt(sh) || image.height;

                return ("d" + image._id + ",0,0,"
                    + image.width + "," + image.height + ","
                    + x + "," + y + "," + width + "," + height + ";");
            } else if (numArgs === 9) {
                sx = parseFloat(sx) || 0.0;
                sy = parseFloat(sy) || 0.0;
                sw = parseInt(sw) || image.width;
                sh = parseInt(sh) || image.height;
                dx = parseFloat(dx) || 0.0;
                dy = parseFloat(dy) || 0.0;
                dw = parseInt(dw) || image.width;
                dh = parseInt(dh) || image.height;

                return ("d" + image._id + ","
                    + sx + "," + sy + "," + sw + "," + sh + ","
                    + dx + "," + dy + "," + dw + "," + dh + ";");
            }
        }
        CanvasRenderingContext2D.GBridge.bindImageTexture(this.componentId, image.src, image._id);
        this._drawCommands += drawImageCommands();
    }
}