package com.taobao.gcanvas.bridges.spec.bridge;

/**
 * Factory for creating Callback data structures.
 * @author ertong
 */

public interface IJSCallbackDataFactory {
    IJSCallbackArray createJSCallbackArray();
    IJSCallbackMap createJSCallbackMap();
}
