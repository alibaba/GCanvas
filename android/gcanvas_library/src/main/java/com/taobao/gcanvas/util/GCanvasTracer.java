package com.taobao.gcanvas.util;


public abstract class GCanvasTracer {


    public abstract void trace(String code, String msg, String extraInfo);


    public abstract void log(String level, String tag, String msg);


}
