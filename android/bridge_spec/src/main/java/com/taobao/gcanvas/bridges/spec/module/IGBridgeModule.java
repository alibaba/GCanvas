package com.taobao.gcanvas.bridges.spec.module;


import com.taobao.gcanvas.bridges.spec.AndroidOnly;

import org.json.JSONArray;
import org.json.JSONObject;

/**
 * <b>Bridge interface for GCanvas bridge layer.</b>
 * <p>The bridge is the environment which provides Javascript execution functionality, such as ReactNative, Weex, WebView, and so on.
 * We need such functionality to connect between GCanvas C++ Render Layer and Javascript runtime. That's why "bridge" comes from.
 * </p>
 * <p>However, on other view, GCanvas bridge is a plugin for Javascript runtime container, so we also use "plugin" to describe these bridges.</p>
 * <p>This class is for bridge extension in case of situation that you need your own bridge.</p>
 * @author ertong
 */

public interface IGBridgeModule<JSCallback> {
    /**
     * Canvas context type
     */
    enum ContextType {
        /**
         * "canvas"
         **/
        _2D(0),

        /**
         * "webgl"
         */
        _3D(1);

        private int value;

        ContextType(int value) {
            this.value = value;
        }

        public int value() {
            return value;
        }
    }

    /**
     * Enable this bridge module, tell native view prepare for rendering.
     * This will build relationship between GCanvas.js and native view.
     * @param data
     * @return String "true" if enable succeed, otherwise "false"
     */
    String enable(JSONObject data);

    /**
     * bind the specified image to the specified canvas's OpenGL environment.
     * @param canvasId
     * @param src
     * @param id image id, a unique id that used for tracking cache.
     * @param callback
     */
    void bindImageTexture(String canvasId, String src, int id, JSCallback callback);

    /**
     * Set the context type for specified GCanvas.
     * @param canvasId
     * @param type
     */
    void setContextType(String canvasId, ContextType type);

    /**
     * Set log level for C++ renderer.
     * @param level 0-debug, 1-info, 2-warn, 3-error
     */
    void setLogLevel(int level);

    /**
     * Load texture sub image for webgl
     * @param canvasid
     * @param target
     * @param level
     * @param xoffset
     * @param yoffset
     * @param format
     * @param type
     * @param path
     */
    @AndroidOnly
    void texSubImage2D(final String canvasid, final int target, final int level, final int xoffset, final int yoffset, final int format, final int type, String path);

    /**
     *
     * @param canvasId
     * @param target
     * @param level
     * @param internalformat
     * @param format
     * @param type
     * @param path
     */
    @AndroidOnly
    void texImage2D(final String canvasId, final int target, final int level, final int internalformat, final int format, final int type, String path);

    /**
     * <p>Load specified image into bitmap cache, send result to callback. </p>
     * <p>Result is some map-like data. If failed, it contains "error" key, otherwise succeed.</p>
     * @param imageInfo index 0 is url, index 1 is image id
     * @param success callback to handle result
     */
    void preLoadImage(JSONArray imageInfo, final JSCallback success);

    /**
     *
     * @param canvasId
     * @param ratio
     */
    void setDevicePixelRatio(String canvasId, double ratio);

    /**
     * Send render command to specified canvas.
     * @param cmd
     * @param canvasId
     */
    void render(String cmd, String canvasId);
}
