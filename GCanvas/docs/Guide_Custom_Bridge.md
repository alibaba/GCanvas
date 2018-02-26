### Android

Note: **At this moment, our publication is being reviewed by Maven. Once published, source code dependency will be changed to AAR dependency.**

* Add bridge specification dependency
* _Download gcanvas source_, add below line to your build.gradle:
  `compile project (':android:bridge\_spec')`
* Implement `IGBridgeModule` interface. The interface should build bridge between GCanvas C++ rendering engine and your Javascript runtime environment. Check comments of the interface for more details. You can extend from an abstract class named 'AbsGBridgeModule'. We strongly recommend implements your bridge by composite implementation.
* Implement interfaces with prefix `IJSCallbackXXX`. These interfaces are used for data communicating between Javascript and native.
* Inherit from `GTextureView`. This stage is optional, depending on how your runtime communicates between native and Javascript, but an OpenGL-renderable view is necessary.

### iOS

* Add Cocoapods dependency Add this pod to your Podfile and run `pod install` to install:
  `pod 'GCanvas'`
* Export module and methods of GCanvas
  * Implement protocol `GCanvasModuleProtocol` in your bridge which you used, such as [Cordova](https://cordova.apache.org/)
  * Implement protocol `GCanvasViewPortocol` in your bridge as a component which contains a `GLKView`as display view of OpenGL ES.
  * Export module to Javascript in your custom bridge For export module, `Weex` and `ReactNative` supply the method `WX\_PlUGIN\_EXPORT\_MODULE` and `RCT\_EXPORT\_MODULE()`
  * Export method to Javascript in your custom bridge For export method, `Weex` and `ReactNative` supply the method.
    * Weex `WX\_EXPORT\_METHOD\_SYNC`, synchronous method `WX\_EXPORT\_METHOD`, asynchornous method
    * ReactNative `RCT\_EXPORT\_BLOCKING\_SYNCHRONOUS\_METHOD` , synchronous method`RCT\_EXPORT\_METHOD`, asynchornous method

There may be different ways in bridges. But your bridge must support export asynchornous and synchronous method both for javascript call native method. And you must implement [Export method list](https://gplatformteam.github.io/gplatform-website/docs/gcanvas/guide/Customize%20your%20own%20bridge.html#Export Method List) to ensure `GCanvas`works well with your bridge.




### Native APIs List
| Method | Async/Sync | Android/iOS | Comments |
| :--- | :--- | :--- | :--- |
| enable(args) | Sync | Both | Build relationships between GCanvas.js and native view |
| render(cmd, componentId) | Async | Both | Execute render command |
| preLoadImage(data, callback) | Async | Both | Preload image with callback |
| bindImageTexture(data, callback) | Async | Both | Bind image to an OpenGL Texture with callback |
| setContextType | Async | Both | Set GCanvas Context Type, 0-2d, 1-WebGL |
| setLogLevel | Async | Both | Set Native Log Level |
| resetComponent | Async | iOS only | call the method while view disappear |
| extendCallNative | Sync | iOS only | method |
| texImage2D | Sync | Android only | convert image to texture, used for WebGL |
| texSubImage2D | Sync | Android only | convert image to sub-texture, used for WebGL |
