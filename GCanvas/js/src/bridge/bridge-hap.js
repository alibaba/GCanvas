
import GLmethod from '../context-webgl/GLmethod';

const GCanvasModule = $app_require$('@app-module/system.canvas')

let isDebugging = false;

let isComboDisabled = false;

const logCommand = (function () {
    const methodQuery = [];
    Object.keys(GLmethod).forEach(key => {
        methodQuery[GLmethod[key]] = key;
    })
    const queryMethod = (id) => {
        return methodQuery[parseInt(id)] || 'NotFoundMethod';
    }
    const logCommand = (id, cmds) => {
        const mId = cmds.split(',')[0];
        const mName = queryMethod(mId);
        console.log(`=== callNative - componentId:${id}; method: ${mName}; cmds: ${cmds}`);
    }
    return logCommand;
})();

function joinArray(arr, sep) {
    let res = '';
    for (let i = 0; i < arr.length; i++) {
        if (i !== 0) {
            res += sep;
        }
        res += arr[i];
    }
    return res;
}

const commandsCache = {}

const GBridge = {
    callEnable: (ref, configArray) => {

        commandsCache[ref] = [];

        return GCanvasModule.enable({
            componentId: ref,
            config: configArray
        });
    },

    callEnableDebug: () => {
        isDebugging = true;
    },

    callEnableDisableCombo: () => {
        isComboDisabled = true;
    },

    callSetContextType: function (componentId, context_type) {
        GCanvasModule.setContextType(context_type, componentId);
    },

    callReset: function(id){
        GCanvasModule.resetComponent && canvasModule.resetComponent(componentId);
    },

    render: function (componentId) {
      return extendCallNative(
        componentId,
        0x40000001,
        'render'
      );
    },

    render2d: function (componentId, commands) {

      if (isDebugging) {
        console.log('>>> >>> render2d ===');
        console.log('>>> commands: ' + commands);
      }

      // GBridge.GCanvasModule.render({commands: commands, componentId: componentId});
      extendCallNative(componentId, 0x20000001, commands);
    },

    callExtendCallNative: function (componentId, cmdArgs) {

        throw 'should not be here anymore ' + cmdArgs;

    },


    flushNative:  function (componentId) {

        const cmdArgs = joinArray(commandsCache[componentId], ';');
        commandsCache[componentId] = [];

        if (isDebugging) {
            console.log('>>> >>> flush native ===');
            console.log('>>> commands: ' + cmdArgs);
        }

        const result = extendCallNative(componentId, 0x40000000, cmdArgs);

        if (isDebugging) {
            console.log('>>> result: ' + result);
        }

        return result;
    },

    callNative: function (componentId, cmdArgs, cache) {

        if (isDebugging) {
            logCommand(componentId, cmdArgs);
        }

        commandsCache[componentId].push(cmdArgs);

        if (!cache || isComboDisabled) {
            return GBridge.flushNative(componentId);
        } else {
            return undefined;
        }
    },

    texImage2D(componentId, ...args) {

        if (args.length === 6) {
            const [target, level, internalformat, format, type, image] = args;
            GCanvasModule.texImage2D(componentId, target, level, internalformat, format, type, image.src);
        } else if (args.length === 9) {
            const [target, level, internalformat, width, height, border, format, type, image] = args;
            GCanvasModule.texImage2D(componentId, target, level, internalformat, width, height, border, format, type, (image ? image.src : 0));
        }

    },

    texSubImage2D(componentId, target, level, xoffset, yoffset, format, type, image) {
        GCanvasModule.texSubImage2D(componentId, target, level, xoffset, yoffset, format, type, image.src);

    },

    bindImageTexture(componentId, src, imageId) {
        GCanvasModule.bindImageTexture([src, imageId], componentId);
    },

    perloadImage([url, id], callback) {
        GCanvasModule.preLoadImage([url, id], function (image) {
            image.url = url;
            image.id = id;
            callback(image);
        });
    }
}

export default GBridge;