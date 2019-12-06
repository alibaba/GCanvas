package com.taobao.gcanvas.bridges.weex;

import android.content.Context;
import android.view.MotionEvent;

import com.taobao.gcanvas.surface.GTextureView;
import com.taobao.weex.ui.view.gesture.WXGesture;
import com.taobao.weex.ui.view.gesture.WXGestureObservable;

/**
 * @author ertong
 */

public class GWXTextureView extends GTextureView implements WXGestureObservable {

    private WXGesture wxGesture;
    private WXGCanvasWeexComponent wxComponent;

    public GWXTextureView(Context context, WXGCanvasWeexComponent component) {
        super(context, component.getRef());
        this.wxComponent = component;
    }

    @Override
    public void registerGestureListener(WXGesture wxGesture) {
        this.wxGesture = wxGesture;
    }

    @Override
    public WXGesture getGestureListener() {
        return wxGesture;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        boolean result = super.onTouchEvent(event);
        if (wxGesture != null) {
            result |= wxGesture.onTouch(this, event);
        }
        return result;
    }

    public void sendEvent(){
        if(wxComponent != null){
            wxComponent.sendEvent();
        }
    }
}
