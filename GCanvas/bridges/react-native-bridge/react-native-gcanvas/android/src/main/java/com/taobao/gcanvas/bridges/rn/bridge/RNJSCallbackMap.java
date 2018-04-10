package com.taobao.gcanvas.bridges.rn.bridge;

import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableMapKeySetIterator;
import com.facebook.react.bridge.ReadableType;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.WritableNativeMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackType;

/**
 * @author ertong
 */

public class RNJSCallbackMap implements IJSCallbackMap {

    private WritableNativeMap mMap = new WritableNativeMap();

    public RNJSCallbackMap(ReadableMap map) {
        if (null != map) {
            mMap.merge(map);
        }
    }

    public RNJSCallbackMap() {
    }

    public WritableMap getMap() {
        return mMap;
    }

    @Override
    public boolean hasKey(String name) {
        return mMap.hasKey(name);
    }

    @Override
    public boolean isNull(String name) {
        return mMap.isNull(name);
    }

    @Override
    public boolean getBoolean(String name) {
        return mMap.getBoolean(name);
    }

    @Override
    public double getDouble(String name) {
        return mMap.getDouble(name);
    }

    @Override
    public int getInt(String name) {
        return mMap.getInt(name);
    }

    @Override
    public String getString(String name) {
        return mMap.getString(name);
    }

    @Override
    public IJSCallbackArray getArray(String name) {
        ReadableArray array = mMap.getArray(name);
        if (null != array) {
            return new RNJSCallbackArray(array);
        }
        return null;
    }

    @Override
    public IJSCallbackMap getMap(String name) {
        ReadableMap map = mMap.getMap(name);
        if (null != map) {
            return new RNJSCallbackMap(map);
        }
        return null;
    }

    @Override
    public IJSCallbackType getType(String name) {
        ReadableType type = mMap.getType(name);
        switch (type) {
            case Map:
                return IJSCallbackType.Map;

            case Null:
                return IJSCallbackType.Null;

            case Boolean:
                return IJSCallbackType.Boolean;

            case Number:
                return IJSCallbackType.Number;

            case String:
                return IJSCallbackType.String;

            case Array:
                return IJSCallbackType.Array;

            default:
                throw new RuntimeException("unknown type");
        }
    }

    @Override
    public IJSCallbackMapKeySetIterator keySetIterator() {
        return new RNJSCallbackMapIterator(this);
    }

    @Override
    public void putNull(String key) {
        mMap.putNull(key);
    }

    @Override
    public void putBoolean(String key, boolean value) {
        mMap.putBoolean(key, value);
    }

    @Override
    public void putDouble(String key, double value) {
        mMap.putDouble(key, value);
    }

    @Override
    public void putInt(String key, int value) {
        mMap.putInt(key, value);
    }

    @Override
    public void putString(String key, String value) {
        mMap.putString(key, value);
    }

    @Override
    public void putArray(String key, IJSCallbackArray value) {
        if (value instanceof RNJSCallbackArray) {
            mMap.putArray(key, ((RNJSCallbackArray) value).getArray());
        }
    }

    @Override
    public void putMap(String key, IJSCallbackMap value) {
        if (value instanceof RNJSCallbackMap) {
            mMap.putMap(key, ((RNJSCallbackMap) value).getMap());
        }
    }

    @Override
    public void merge(IJSCallbackMap source) {
        if (source instanceof RNJSCallbackMap) {
            mMap.merge(((RNJSCallbackMap) source).getMap());
        }
    }

    private static class RNJSCallbackMapIterator implements IJSCallbackMapKeySetIterator {

        private ReadableMapKeySetIterator mIterator;

        public RNJSCallbackMapIterator(RNJSCallbackMap mMap) {
            this.mIterator = mMap.mMap.keySetIterator();
        }

        @Override
        public boolean hasNextKey() {
            return this.mIterator.hasNextKey();
        }

        @Override
        public String nextKey() {
            return this.mIterator.nextKey();
        }
    }
}
