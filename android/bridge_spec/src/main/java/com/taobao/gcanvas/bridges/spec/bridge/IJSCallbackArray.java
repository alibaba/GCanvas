package com.taobao.gcanvas.bridges.spec.bridge;

/**
 * Communications between Javascript runtime and native are container dependent. Different environment has different standards.
 * This interface provides array data structure for passing parameters from native to javascript.
 * @author ertong
 */

public interface IJSCallbackArray {
    /**
     * Return size of this array.
     * @return 0 if array is empty.
     */
    int size();

    /**
     * Test element at specified index if it is null.
     * @param index
     * @return true if element at index is null.
     */
    boolean isNull(int index);
    /**
     * Get specified index as boolean. Throw ClassCastException if element at specified index is not type of boolean
     * @param index
     * @return
     */
    boolean getBoolean(int index);
    /**
     * Get specified index as double. Throw ClassCastException if element at specified index is not type of double
     * @param index
     * @return
     */
    double getDouble(int index);
    /**
     * Get specified index as integer. Throw ClassCastException if element at specified index is not type of integer
     * @param index
     * @return
     */
    int getInt(int index);
    /**
     * Get specified index as String.
     * @param index
     * @return
     */
    String getString(int index);
    /**
     * Get specified index as IJSCallbackArray instance.Throw ClassCastException if element at specified index is not type of IJSCallbackArray
     * @param index
     * @return
     */
    IJSCallbackArray getArray(int index);
    IJSCallbackMap getMap(int index);
    IJSCallbackType getType(int index);

    void pushNull();
    void pushBoolean(boolean value);
    void pushDouble(double value);
    void pushInt(int value);
    void pushString(String value);
    void pushArray(IJSCallbackArray array);
    void pushMap(IJSCallbackMap map);
}
