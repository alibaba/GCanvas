package com.taobao.gcanvas.bridges.spec.module;

import java.util.concurrent.atomic.AtomicInteger;

/**
 * Class for recording image loading status and basic information.
 * @author ertong
 */

public class GImageLoadInfo {
    /**
     * Image has not loaded yet.
     */
    public static final int IDLE = -1;
    /**
     * Image is on loading.
     */
    public static final int LOADING = 0x100;
    /**
     * Image has been successfully loaded.
     */
    public static final int LOADED = 0x200;
    public int width;
    public int height;
    public int id;
    /**
     * current load status
     */
    public AtomicInteger status = new AtomicInteger(IDLE);
}
