package com.taobao.gcanvas.bridges.spec.bridge;

/**
 * <p>Communications between Javascript runtime and native are container dependent. Different environment has different standards.</p>
 * <p>This enum defines supported data types.</p>
 * @author ertong
 */

public enum  IJSCallbackType {
    Null,
    Boolean,
    Number,
    String,
    Map,
    Array,
}
