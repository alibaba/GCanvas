const isReactNativeIOS = () => {
    GBridge.Platform.OS === 'ios';
};

const isReactNativeAndroid = () => {
    GBridge.Platform.OS === 'android';
};

let isDebugging = false;

let isComboDisabled = false;

const commandsCache = {}

const GBridge = {

    GCanvasModule : null,

    Platform: null,

    callEnable: (ref, configArray) => {

        commandsCache[ref] = [];

        return GBridge.GCanvasModule.enable({
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
        GBridge.GCanvasModule.setContextType(context_type, componentId);
    },

    render: function (componentId) {
        return GBridge.GCanvasModule.extendCallNative({
            contextId: componentId,
            type: 0x60000001
        });
    },

    render2d: function (componentId, commands) {

        if (isDebugging) {
            console.log('>>> >>> render2d ===');
            console.log('>>> commands: ' + commands);
        }

        GBridge.GCanvasModule.render(commands, componentId);

    },


    flushNative: function (componentId) { },

    callNative: function (componentId, cmdArgs, cache) {

    },

    texImage2D(componentId, ...args) {
        if (isReactNativeIOS()) {
            if (args.length === 6) {
                const [target, level, internalformat, format, type, image] = args;
                GBridge.GBridge.callNative(
                    componentId,
                    GLmethod.texImage2D + ',' + 6 + ',' + target + ',' + level + ',' + internalformat + ',' + format + ',' + type + ',' + image.src
                )
            } else if (args.length === 9) {
                const [target, level, internalformat, width, height, border, format, type, image] = args;
                GBridge.GBridge.callNative(
                    componentId,
                    GLmethod.texImage2D + ',' + 9 + ',' + target + ',' + level + ',' + internalformat + ',' + width + ',' + height + ',' + border + ',' +
                    + format + ',' + type + ',' + (image ? image.src : 0)
                )
            }
        } else if (isReactNativeAndroid()) {
            if (args.length === 6) {
                const [target, level, internalformat, format, type, image] = args;
                GBridge.GCanvasModule.texImage2D(componentId, target, level, internalformat, format, type, image.src);
            } else if (args.length === 9) {
                const [target, level, internalformat, width, height, border, format, type, image] = args;
                GBridge.GCanvasModule.texImage2D(componentId, target, level, internalformat, width, height, border, format, type, (image ? image.src : 0));
            }
        }
    },

    texSubImage2D(componentId, target, level, xoffset, yoffset, format, type, image) {
        if (isReactNativeIOS() ) {
            if (arguments.length === 8) {
                GBridge.callNative(
                    componentId,
                    GLmethod.texSubImage2D + ',' + 6 + ',' + target + ',' + level + ',' + xoffset + ',' + yoffset, + ',' + format + ',' + type + ',' + image.src
                )
            }
        } else if (isReactNativeAndroid()) {
            GCanvasModule.texSubImage2D(componentId, target, level, xoffset, yoffset, format, type, image.src);
        }
    },

    bindImageTexture(componentId, src, imageId) {
        GBridge.GCanvasModule.bindImageTexture([src, imageId], componentId, function(e){

        });
    },

    perloadImage([url, id], callback) {
        GBridge.GCanvasModule.preLoadImage([url, id], function (image) {
            image.url = url;
            image.id = id;
            callback(image);
        });
    }
}

export default GBridge;