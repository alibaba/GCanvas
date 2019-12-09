package com.taobao.gcanvas.util;

/**
 * 供外部注入的java层 日志埋点相关接口
 * 支持trace()埋点 和 日志
 * Created by zhanqu on 2019/3/20.
 */
public abstract class GCanvasTracer {


    public abstract void trace(String code, String msg, String extraInfo);


    public abstract void log(String level, String tag, String msg);


}
