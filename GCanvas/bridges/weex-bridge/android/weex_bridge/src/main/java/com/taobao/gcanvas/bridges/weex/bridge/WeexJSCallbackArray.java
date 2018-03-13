package com.taobao.gcanvas.bridges.weex.bridge;


import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackType;

import org.json.JSONArray;
import org.json.JSONException;

/**
 * Weex IJSCallbackArray, implemented by ArrayList.
 *
 * @author ertong
 */
public class WeexJSCallbackArray extends JSONArray implements IJSCallbackArray {
    @Override
    public int size() {
        return length();
    }

    @Override
    public boolean isNull(int index) {
        return opt(index) == null;
    }

    @Override
    public boolean getBoolean(int index) {
        return optBoolean(index, false);
    }

    @Override
    public double getDouble(int index) {
        return optDouble(index);
    }

    @Override
    public int getInt(int index) {
        return optInt(index);
    }

    @Override
    public String getString(int index) {
        return optString(index);
    }

    @Override
    public IJSCallbackArray getArray(int index) {
        return (IJSCallbackArray) opt(index);
    }

    @Override
    public IJSCallbackMap getMap(int index) {
        return (IJSCallbackMap) opt(index);
    }

    @Override
    public IJSCallbackType getType(int index) {
        Object object = opt(index);

        if (object == null) {
            return IJSCallbackType.Null;
        } else if (object instanceof Boolean) {
            return IJSCallbackType.Boolean;
        } else if (object instanceof Double ||
                object instanceof Float ||
                object instanceof Integer) {
            return IJSCallbackType.Number;
        } else if (object instanceof String) {
            return IJSCallbackType.String;
        } else if (object instanceof IJSCallbackArray) {
            return IJSCallbackType.Array;
        } else if (object instanceof IJSCallbackMap) {
            return IJSCallbackType.Map;
        }
        return null;
    }

    @Override
    public void pushNull() {
        put(null);
    }

    @Override
    public void pushBoolean(boolean value) {
        put(value);
    }

    @Override
    public void pushDouble(double value) {
        try {
            put(value);
        } catch (JSONException e) {
        }
    }

    @Override
    public void pushInt(int value) {
        put(value);
    }

    @Override
    public void pushString(String value) {
        put(value);
    }

    @Override
    public void pushArray(IJSCallbackArray array) {
        put(array);
    }

    @Override
    public void pushMap(IJSCallbackMap map) {
        put(map);
    }
}
