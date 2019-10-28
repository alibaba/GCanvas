## Android
### Requirement
Make sure the following configuration is completed:
* [JDK](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)  >= 1.7 , and configure the environment variable
* Android SDK installed and configure the environment variable
* Android SDK version 23 (compileSdkVersion in build.gradle)
* SDK build tools version 21.1.2 (buildToolsVersion in build.gradle)
* Android Support Repository >= 17 (for Android Support Library)
* [Weex](https://weex.apache.org/) SDK >= 0.16.X

### Steps
The steps are as follows:
1. Create an Android project. There is nothing to be specified, according to your habits to.
2. **At this moment, our publication is being reviewed by Maven. Once published, source dependency will be changed to AAR dependency. **
   Download source code, update build.gradle by adding the following dependencies:
```groovy
compile project ':bridges:weex-bridge:android:weex_bridge'
```
3. Register GCanvas Module and Component to your Weex Application.
```java
WXSDKEngine.registerModule("gcanvas", GCanvasWeexModule.class);
WXSDKEngine.registerComponent("gcanvas", WXGCanvasWeexComponent.class);
```
4. Modify AndroidManifest.xml, add below line to acquire OpenGL ES features.
```xml
<uses-feature android:glEsVersion="0x00020000" />
```

## iOS
#### Requirement
* Make sure that you have installed the [iOS development environment](https://help.apple.com/xcode/mac/current/#/dev60b6fbbc7) and [CocoaPods](https://guides.cocoapods.org/using/getting-started.html).
* Make sure that you have integrate [WeexSDK](https://github.com/apache/incubator-weex#ios)
* Make sure that you have insalled the [weex-toolkit](https://www.npmjs.com/package/weex-toolkit) in [Nodejs](https://nodejs.org/en/) 4.0+ for running case

#### Steps
1. Install components with CocoaPods
```objective-c
pod 'WeexGcanvas'
pod 'WeexPluginLoader'
```

`WeexPluginLoader` would dynamic register Weex `Module` and `Component` of GCanvas, so we do not need to register manually.
