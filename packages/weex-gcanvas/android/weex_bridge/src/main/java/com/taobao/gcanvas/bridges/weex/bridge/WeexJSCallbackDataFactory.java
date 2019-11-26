package com.taobao.gcanvas.bridges.weex.bridge;

import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;

/**
 * @author ertong
 */
public class WeexJSCallbackDataFactory implements IJSCallbackDataFactory {
    @Override
    public IJSCallbackArray createJSCallbackArray() {
        return new WeexJSCallbackArray();
    }

    @Override
    public IJSCallbackMap createJSCallbackMap() {
        return new WeexJSCallbackMap();
    }
}
