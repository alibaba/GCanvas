# Create a custom GCanvas Bridge

If you have known how GCanvas bridge worked in `Weex` or `ReactNative`, and you want to implement a GCanvas bridge by youself in other bridge(Cordva、WebViewJavascriptBridge etc.)

### iOS
- Add Cocoapods dependency
  Add this pod to your Podfile and run `pod install` to install:
```objective-c
	pod 'GCanvas'
```

-  Export module and methods of GCanvas
   * Implement  protocol `GCanvasModuleProtocol`  in your bridge which you used, such as [Cordova](https://cordova.apache.org/)
   * Implement protocol `GCanvasViewPortocol` in your bridge as a component which contains a `GLKView` as display view of OpenGL ES.
   * Export module to Javascript in your custom bridge
      For export module, `Weex` and `ReactNative` supply the method `WX_PlUGIN_EXPORT_MODULE` and `RCT_EXPORT_MODULE()`

   * Export method to Javascript in your custom bridge
      For export method, `Weex` and `ReactNative` supply the method.
     * Weex
       `WX_EXPORT_METHOD_SYNC`, synchronous method
        `WX_EXPORT_METHOD`, asynchornous method

     * ReactNative
       `RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD` , synchronous method
       `RCT_EXPORT_METHOD`,  asynchornous method

There may be different way in bridges. But you bridge must support export asynchornous and synchronous method both for javascript call native method. And you must implement [Export method list](#Export Method List)  to ensure `GCanvas` work well through your bridge .

### Android

* Add bridge specification dependency

  **At this moment, our publication is being reviewed by Maven. Once published, source code dependency will be changed to AAR dependency. **

  *Download gcanvas source*, add below line to your build.gradle:

  ```groovy
  compile project (':android:bridge_spec')
  ```

* Implement `IGBridgeModule` interface. The interface should build bridge between GCanvas C++ rendering engine and your Javascript runtime environment. Check comments of the interface for more details. You can extend from an abstract class named 'AbsGBridgeModule'. We strongly recommend implements your bridge by composite implementation.
* Implement interfaces with prefix `IJSCallbackXXX`. These interfaces are used for data communicating between Javascript and native.
* Inherit from `GTextureView`. This stage is optional, depending on how your runtime communicates between native and Javascript, but an OpenGL-renderable view is necessary.

### Javascript
About writing the javascript part, you can refer to[Custom Javascript Bridge](./docs/Guide_JS_Use.md).

### Export Method List
| Method                           | Async/Sync | Android/iOS  | Comments                                 |
| -------------------------------- | ---------- | ------------ | ---------------------------------------- |
| enable(args)                     | Sync       | Both         | Build relationships between gcanvas.js and native view |
| render(cmd, componentId)         | Async      | Both         | Execute render command                   |
| preLoadImage(data, callback)     | Async      | Both         | Preload image with callback              |
| bindImageTexture(data, callback) | Async      | Both         | Bind image to an OpenGL Texture with callback |
| setContextType                   | Async      | Both         | Set GCanvas Context Type, 0-2d, 1-WebGL  |
| setLogLevel                      | Async      | Both         | Set Native Log Level                     |
| resetComponent                   | Async      | iOS only     | call the method while view disappear     |
| extendCallNative                 | Sync       | iOS only     | method                                   |
| texImage2D                       | Sync       | Android only | convert image to texture, used for WebGL |
| texSubImage2D                    | Sync       | Android only | convert image to sub-texture, used for WebGL |
