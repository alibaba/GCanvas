package com.taobao.gcanvas.bridges.spec.bridge;

/**
 * <p>Communications between Javascript runtime and native are container dependent. Different environment has different standards.</p>
 * <p>This interface provides an map that allows typed access to its members. Used to pass parameters from native
 * to javascript.</p>
 * @author ertong
 */

public interface IJSCallbackMap {
    boolean hasKey(String name);
    boolean isNull(String name);
    boolean getBoolean(String name);
    double getDouble(String name);
    int getInt(String name);
    String getString(String name);
    IJSCallbackArray getArray(String name);
    IJSCallbackMap getMap(String name);
    IJSCallbackType getType(String name);
    IJSCallbackMapKeySetIterator keySetIterator();

    void putNull(String key);
    void putBoolean(String key, boolean value);
    void putDouble(String key, double value);
    void putInt(String key, int value);
    void putString(String key, String value);
    void putArray(String key, IJSCallbackArray value);
    void putMap(String key, IJSCallbackMap value);

    /**
     * Merge specified map's contents into this map.
     * @param source
     */
    void merge(IJSCallbackMap source);

    /**
     * Iterator of this map.
     */
    interface IJSCallbackMapKeySetIterator {
        boolean hasNextKey();
        String nextKey();
    }
}
