package com.taobao.gcanvas.bridges.rn.bridge;

import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackDataFactory;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;

/**
 * @author ertong
 */

public class RNJSCallbackDataFactory implements IJSCallbackDataFactory {
    @Override
    public IJSCallbackArray createJSCallbackArray() {
        return new RNJSCallbackArray();
    }

    @Override
    public IJSCallbackMap createJSCallbackMap() {
        return new RNJSCallbackMap();
    }
}
