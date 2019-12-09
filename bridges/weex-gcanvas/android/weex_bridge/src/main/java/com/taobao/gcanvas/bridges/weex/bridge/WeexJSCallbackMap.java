package com.taobao.gcanvas.bridges.weex.bridge;


import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackType;

import java.util.HashMap;
import java.util.Iterator;


/**
 * Weex IJSCallbackMap implemented by HashMap.
 * @author ertong
 */
public class WeexJSCallbackMap extends HashMap implements IJSCallbackMap {
    @Override
    public boolean hasKey(String name) {
        return containsKey(name);
    }

    @Override
    public boolean isNull(String name) {
        return get(name) == null;
    }

    @Override
    public boolean getBoolean(String name) {
        return (Boolean) get(name);
    }

    @Override
    public double getDouble(String name) {
        return (Double) get(name);
    }

    @Override
    public int getInt(String name) {
        return (Integer) get(name);
    }

    @Override
    public String getString(String name) {
        return String.valueOf(get(name));
    }

    @Override
    public IJSCallbackArray getArray(String name) {
        return (IJSCallbackArray) get(name);
    }

    @Override
    public IJSCallbackMap getMap(String name) {
        return (IJSCallbackMap) get(name);
    }

    @Override
    public IJSCallbackType getType(String name) {
        Object value = get(name);
        if (value == null) {
            return IJSCallbackType.Null;
        } else if (value instanceof Number) {
            return IJSCallbackType.Number;
        } else if (value instanceof String) {
            return IJSCallbackType.String;
        } else if (value instanceof Boolean) {
            return IJSCallbackType.Boolean;
        } else if (value instanceof IJSCallbackMap) {
            return IJSCallbackType.Map;
        } else if (value instanceof IJSCallbackArray) {
            return IJSCallbackType.Array;
        } else {
            throw new IllegalArgumentException("Invalid value " + value.toString() + " for key " + name +
                    "contained in JavaOnlyMap");
        }
    }

    @Override
    public IJSCallbackMapKeySetIterator keySetIterator() {
        return new IJSCallbackMapKeySetIterator() {
            Iterator<String> mIterator = keySet().iterator();

            @Override
            public boolean hasNextKey() {
                return mIterator.hasNext();
            }

            @Override
            public String nextKey() {
                return mIterator.next();
            }
        };
    }

    @Override
    public void putNull(String key) {
        put(key, null);
    }

    @Override
    public void putBoolean(String key, boolean value) {
        put(key, value);
    }

    @Override
    public void putDouble(String key, double value) {
        put(key, value);
    }

    @Override
    public void putInt(String key, int value) {
        put(key, value);
    }

    @Override
    public void putString(String key, String value) {
        put(key, value);
    }

    @Override
    public void putArray(String key, IJSCallbackArray value) {
        put(key, value);
    }

    @Override
    public void putMap(String key, IJSCallbackMap value) {
        put(key, value);
    }

    @Override
    public void merge(IJSCallbackMap source) {
        if (source instanceof WeexJSCallbackMap) {
            putAll(((WeexJSCallbackMap) source));
        }
    }
}
