import GContext2D from '../context-2d/OffScreenRenderingContext';
import GContextWebGL from '../context-webgl/RenderingContext';


export default class GOffScreenCanvas {

    id = null;

    _needRender = true;

    constructor(id) {
        this.id = id;
        this.className = 'GOffScreenCanvas';

    }


    getContext(type) {

        let context = null;

        if (type.match(/webgl/i)) {
            context = new GContextWebGL(this);

            context.componentId = this.id;

            if (!this._disableAutoSwap) {
                const render = () => {
                    if (this._needRender) {
                        GCanvas.GBridge.render(this.id);
                        this._needRender = false;
                    }
                }
                setInterval(render, 16);
            }

            GCanvas.GBridge.callSetContextType(this.id, 1); // 0 for 2d; 1 for webgl
        } else if (type.match(/2d/i)) {
            if (this.context2d) {
                context = this.context2d;
            } else {
                this.context2d = context = new GContext2D(this);
                context.componentId = this.id;
            }

        } else {
            throw new Error('not supported context ' + type);
        }

        return context;

    }

    get drawCommands() {
        return this.context2d._drawCommands;
    }

    get images() {
        return this.context2d.images;

    }
}