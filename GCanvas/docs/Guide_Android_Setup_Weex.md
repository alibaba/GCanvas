# GCanvas Android Guide

### Integrate to Android
Tip: The following documents assume that you already hava a certain Android development experience.

### Prerequisites
Make sure the following configuration is completed:

* [JDK](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)  >= 1.7 , and configure the environment variable
* Android SDK installed and configure the environment variable
* Android SDK version 23 (compileSdkVersion in build.gradle)
* SDK build tools version 21.1.2 (buildToolsVersion in build.gradle)
* Android Support Repository >= 17 (for Android Support Library)
* [Weex](https://weex.apache.org/) SDK >= 0.16.X


### Quick to use
The steps are as follows:

1. Create an Android project. There is nothing to be specified, according to your habits to.

2. **At this moment, our publication is being reviewed by Maven. Once published, source code dependency will be changed to AAR dependency. **
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

