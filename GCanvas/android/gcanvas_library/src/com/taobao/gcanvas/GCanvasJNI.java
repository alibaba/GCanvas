/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 * <p>
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */


package com.taobao.gcanvas;


import android.content.Context;
import android.graphics.Bitmap;
import android.os.Build;

import com.taobao.gcanvas.util.GLog;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class GCanvasJNI {
    public static boolean GCanvaslibEnable = false;
    public static void load(){
        if(!GCanvaslibEnable){
            try {
                System.loadLibrary("freetype");
                System.loadLibrary("gcanvas");
                GCanvaslibEnable = true;
            } catch (UnsatisfiedLinkError e) {
                GLog.e("CANVAS", "gcanvas is not found.");
            } catch (Exception e) {
                GLog.e("CANVAS", "fail to load gcanvas.");
            }
        }
    }
    // Native methods
    static {
        load();
    }

    static Map<String, Integer> contextTypeMap = new HashMap<>();
    static Map<String, Double> devicePixelRatioMap = new HashMap<>();
    static Map<String, Boolean> qualityMap = new HashMap<>();


    public static native void setBackgroundColor(String contextID, int red, int green, int blue);

    public static native void setOrtho(String contextID, int width, int height);

    public static native void render(String contextID, String renderCommands);


    public static native void release(); // Deletes native canvas

    public static void setWrapperDevicePixelRatio(String contextID, double ratio){
        setDevicePixelRatio(contextID, ratio);
        devicePixelRatioMap.put(contextID, ratio);
    }

    public static void setWrapperContextType(String contextID, int type){
        setContextType(contextID, type);
        contextTypeMap.put(contextID, type);
    }

    public static void setWrapperHiQuality(String contextID, boolean isHiQuality){
        setHiQuality(contextID, isHiQuality);
        qualityMap.put(contextID, isHiQuality);
    }

    public static void refreshArguments(String contextID){
        Integer type=contextTypeMap.get(contextID);
        if(type!=null){
            setContextType(contextID, type);
        }
        Double ratio=devicePixelRatioMap.get(contextID);
        if(ratio!=null){
            setDevicePixelRatio(contextID, ratio);
        }
        Boolean isHiQuality=qualityMap.get(contextID);
        if(isHiQuality){
            setHiQuality(contextID, isHiQuality);
        }
    }
    public static native void setDevicePixelRatio(String contextID, double ratio);

    public static native void setClearColor(String contextID, String color);

    public static native void setTyOffsetFlag(String contextID, boolean flag);

    public static native void setHiQuality(String contextID, boolean isHiQuality);

    public static native boolean isNeonSupport();

    public static native boolean isFboSupport(String contextID);


    public static native String getImageData(String contextID, int x, int y, int width, int height);

    public static native void setContextType(String contextID, int type);

    public static native void setConfig(String key, int value);


    public static native void setFallbackFont(String fallbackFontFile, String systemFontLocation);

    public static native void addFontFamily(String[] fontNames, String[] fontFiles);

    public static native void addFallbackFontFamily(String[] fallbackFontFiles);

    public static void registerWXCallNativeFunc(Context context){
        /**
         * We have a different javascript-call-native solution on Weex, which we call it "directly" call-native.
         * Weex's customized JavascriptCore has been injected a special handle to deal GCanvas' call. All calls can directly send to JavascriptCore, avoiding JNI calls, which significantly improves performance.
         * For the purpose of decoupling, We open weexjsc.so to inject our implementation at runtime.
         * Thanks to Weex team.
         */
        String libraryPath = null;
        if (Build.VERSION.SDK_INT >= 24) {
            // Android 7 has changed security policy, different .so has different namespace.
            libraryPath = context.getApplicationInfo().nativeLibraryDir + "/libweexcore.so";
            GLog.i("start to load gcanvas library,path=" + libraryPath);
        }
        GCanvasJNI.registerCallback(libraryPath, Build.VERSION.SDK_INT);
    }

    public static native void setLogLevel(String logLevel);

    public static native void setPreCompilePath(String path);

    public static native void bindTexture(String contextId, Bitmap bitmap,int id,
                                          int target, int level,int internalformat,int format,int type);
    public static native void texSubImage2D(String contextId, Bitmap bitmap,int id,
                                          int target, int level,int xoffset,int yoffset,int format,int type);

    public static native boolean sendEvent(String contextId);

    public static native void registerCallback(String soPath, int version);

    public static native int getNativeFps(String contextId);


    public static void setFontFamilies() {
        // TODO: Do we need to make it as singleton?
        GFontConfigParser parser = new GFontConfigParser();
        String fallbackFont = parser.getFallbackFont();
        String systemFontLocation = parser.getSystemFontLocation();
        GCanvasJNI.setFallbackFont(fallbackFont, systemFontLocation);

        HashMap<List<String>, List<String>> fontFamilies = parser.getFontFamilies();


        if (fontFamilies != null) {
            GLog.d("CANVAS", "setFontFamilies() fontFamilies.size() " + fontFamilies.size());
        } else {
            GLog.d("CANVAS", "setFontFamilies() error! fontFamilies is empty");
        }


        if (fontFamilies != null) {
            Iterator<List<String>> it = fontFamilies.keySet().iterator();
            while (it.hasNext()) {
                List<String> fontNames = it.next();
                int fontNameSize = fontNames.size();
                String[] fontNameArray = new String[fontNameSize];
                for (int i = 0; i < fontNameSize; ++i) {
                    fontNameArray[i] = fontNames.get(i);
                }

                List<String> fontFiles = fontFamilies.get(fontNames);
                int fontFileSize = fontFiles.size();
                String[] fontFileArray = new String[fontFileSize];
                for (int i = 0; i < fontFileSize; ++i) {
                    fontFileArray[i] = fontFiles.get(i);
                }

                GCanvasJNI.addFontFamily(fontNameArray, fontFileArray);
            }
        }

        List<String> fallbackfontFiles = parser.getFallbackFontsList();

        if (fallbackfontFiles == null) {
            return;
        }

        int fallbackfontFileSize = fallbackfontFiles.size();
        String[] FallbackFileArray = new String[fallbackfontFileSize];
        for (int i = 0; i < fallbackfontFileSize; ++i) {
            FallbackFileArray[i] = fallbackfontFiles.get(i);
        }

        GCanvasJNI.addFallbackFontFamily(FallbackFileArray);
    }
}
