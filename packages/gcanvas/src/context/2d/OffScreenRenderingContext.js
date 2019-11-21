import GImage from '../env/image';
import GContext2D from './RenderingContext';


export default class GOffScreenContext2D extends GContext2D {
  constructor() {
    super();
    this.className = 'OffScreenCanvasRenderingContext2D';
    this.images = [];
  }

  drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh) {
    const numArgs = arguments.length;
    function drawImageCommands() {
      if (numArgs === 3) {
        const x = parseFloat(sx) || 0.0;
        const y = parseFloat(sy) || 0.0;

        return 'd' + image._id + ',0,0,'
                    + image.width + ',' + image.height + ','
                    + x + ',' + y + ',' + image.width + ',' + image.height + ';';
      } else if (numArgs === 5) {
        const x = parseFloat(sx) || 0.0;
        const y = parseFloat(sy) || 0.0;
        const width = parseInt(sw) || image.width;
        const height = parseInt(sh) || image.height;

        return 'd' + image._id + ',0,0,'
                    + image.width + ',' + image.height + ','
                    + x + ',' + y + ',' + width + ',' + height + ';';
      } else if (numArgs === 9) {
        sx = parseFloat(sx) || 0.0;
        sy = parseFloat(sy) || 0.0;
        sw = parseInt(sw) || image.width;
        sh = parseInt(sh) || image.height;
        dx = parseFloat(dx) || 0.0;
        dy = parseFloat(dy) || 0.0;
        dw = parseInt(dw) || image.width;
        dh = parseInt(dh) || image.height;

        return 'd' + image._id + ','
                    + sx + ',' + sy + ',' + sw + ',' + sh + ','
                    + dx + ',' + dy + ',' + dw + ',' + dh + ';';
      }
    }
    this.images.push(image);
    this._drawCommands += drawImageCommands();
  }
}