package com.taobao.gcanvas.bridges.rn.bridge;

import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableType;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.WritableNativeArray;
import com.facebook.react.bridge.WritableNativeMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackArray;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackMap;
import com.taobao.gcanvas.bridges.spec.bridge.IJSCallbackType;

/**
 * @author ertong
 */

public class RNJSCallbackArray implements IJSCallbackArray {

    private WritableNativeArray mArray = new WritableNativeArray();

    public RNJSCallbackArray(ReadableArray array) {
        if (null != array) {
            for (int i = 0; i < array.size(); i++) {
                ReadableType type = array.getType(i);
                switch (type) {
                    case Map:
                        ReadableMap map = array.getMap(i);
                        if (map instanceof WritableNativeMap) {
                            mArray.pushMap((WritableNativeMap) map);
                        }
                        break;

                    case Null:
                        mArray.pushNull();
                        break;

                    case Boolean:
                        mArray.pushBoolean(array.getBoolean(i));
                        break;

                    case Number:
                        mArray.pushDouble(array.getDouble(i));
                        break;

                    case String:
                        mArray.pushString(array.getString(i));
                        break;

                    case Array:
                        ReadableArray arr = array.getArray(i);
                        if (arr instanceof WritableNativeArray) {
                            mArray.pushArray((WritableArray) arr);
                        }
                }
            }
        }
    }

    public RNJSCallbackArray() {
    }

    public WritableNativeArray getArray() {
        return mArray;
    }

    @Override
    public int size() {
        return mArray.size();
    }

    @Override
    public boolean isNull(int index) {
        return mArray.isNull(index);
    }

    @Override
    public boolean getBoolean(int index) {
        return mArray.getBoolean(index);
    }

    @Override
    public double getDouble(int index) {
        return mArray.getDouble(index);
    }

    @Override
    public int getInt(int index) {
        return mArray.getInt(index);
    }

    @Override
    public String getString(int index) {
        return mArray.getString(index);
    }

    @Override
    public IJSCallbackArray getArray(int index) {
        ReadableArray array = mArray.getArray(index);
        if (null != array) {
            return new RNJSCallbackArray(array);
        }
        return null;
    }

    @Override
    public IJSCallbackMap getMap(int index) {
        ReadableMap map = mArray.getMap(index);
        if (null != map) {
            return new RNJSCallbackMap(map);
        }
        return null;
    }

    @Override
    public IJSCallbackType getType(int index) {
        ReadableType type = mArray.getType(index);
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
    public void pushNull() {
        mArray.pushNull();
    }

    @Override
    public void pushBoolean(boolean value) {
        mArray.pushBoolean(value);
    }

    @Override
    public void pushDouble(double value) {
        mArray.pushDouble(value);
    }

    @Override
    public void pushInt(int value) {
        mArray.pushInt(value);
    }

    @Override
    public void pushString(String value) {
        mArray.pushString(value);
    }

    @Override
    public void pushArray(IJSCallbackArray array) {
        if (array instanceof RNJSCallbackArray) {
            mArray.pushArray(((RNJSCallbackArray) array).getArray());
        }
    }

    @Override
    public void pushMap(IJSCallbackMap map) {
        if (map instanceof RNJSCallbackMap) {
            mArray.pushMap(((RNJSCallbackMap) map).getMap());
        }
    }
}
