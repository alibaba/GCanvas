## 1. Why

The project of GCanvas was started in 2013. At that time, canvas animation running on a mobile app was really a nightmare(at least in most cases). A few severe problems prevented developers utilizing the canvas in a wide way:
- slow performance
- bad compatibility(especially on Android)
- lack of completeness of canvas APIs

To solve the problems mentioned above, we decided to write a cross-platform solutions and provide APIs in JavaScript fully aligned with HTML5 canvas APIs. To ensure the performance, C++ with OpenGL ES appears to be the only way.

Time has passed, powerful mobile hybrid frameworks such as ReactNative/Weex have become popular, but Canvas/WebGL has been scarcely implemented on these frameworks. So we have GCanvas integrated into the most popular framework to provide high performance canvas APIs.

## 2. How

In short, the principle of GCanvas is quite simple:
- intercepts draw calls of canvas
- reforms these draw calls and transfer them to render core implemented in C++
- runs OpenGL ES APIs per draw calls

The below diagram shows the high-level design of GCanvas:

![GCanvas_HLD](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/gcanvas/gcanvas/4c1e9581af4a374331cd3afb4c7db84d/GCanvas_HLD.png)

### 2.1 Canvas Delegate Layer

Canvas Delegate Layer is responsible for mimicing canvas APIs. It intercepts the calls of origin canvas APIs and converts the call to a command sequence.
The code is under ./gcanvas/js/, which mainly includes core files in ./src/core/ and bridge files in ./bridge/.

A simple workflow is as below:

![design](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/gcanvas/gcanvas/039090e325495e896b5f69dd3a3828fa/JS_Layer_Design.png)

### 2.2 Bridge Layer

Bridge Layer is responsible for setting up a bridge to interact between JavaScript and OS native languages.

There are typically 2 ways to achieve this goal. One way is to implement it based on JSBridge, for example, Cordova is supposed to be a good choice. Another way is to implement it based on JSBinding, for example, V8 on Android and JSC on iOS.

We have integrated an Android bridge(based on Cordova) & an iOS bridge(based on WebViewJavascriptBridge) into this project. It is only used to demonstrate how a bridge plug-in can be implemented. Any implementation in this layer is supposed to  be replaced or extended easily. The code of Android plug-in is under ./gcanvas/android/android_cordova_bridge_plugin/ and the code of iOS plug-in is under ./gcanvas/ios/GCanvasJSBridge/.

### 2.3 Platform Adapter Layer

Operation System Adapter Layer is responsible for relaying draw calls, responding OS events, like touch etc. and other OS-related operations that cannot be processed in Render Core Layer. The code of Android is under ./gcanvas/android/android_library/ and the code of iOS is under ./gcanvas/ios/Classes/.

#### 2.3.1 Android simple workflow
![gcanvas-android-archive_1](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/gcanvas/gcanvas/42fa4dbe148a09baaef24b8ca132d3d2/gcanvas-android-adapter_layer_brief.png)

#### 2.3.2 iOS simple workflow
![GCanvas_brief_iOS](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/gcanvas/gcanvas/c20f34aafadc65bfa6c7af91f817c02c/GCanvas_brief_iOS.png)

### 2.4 Render Core Layer

Render Core Layer is responsible for implementing canvas APIs one by one. It contains a pipeline that consumes a draw call and produces a frame that can be rendered by upper layers or reused as a new input by a new draw call(FrameBuffer Object). The developers can simply look on it as a tiny render engine that provides a lot of APIs aligned with HTML5 canvas, which is implemented by OpenGL ES 2.0. The code is under ./gcanvas/core/.

A simple workflow is as below:
![GCanvas_Render_Core_Layer_Detail_Design](http://git.cn-hangzhou.oss.aliyun-inc.com/uploads/gcanvas/gcanvas/97d10777075cbe0f9134beeea94a54e3/GCanvas_Render_Core_Layer_Detail_Design.png)


### 2.5 Demo

#### 2.5.1 Android demo

./gcanvas/android/android_cordova_bridge_demo

#### 2.5.2 iOS demo

./gcanvas/ios/GCanvasJSBridgeDemo

### 2.6 Test

For quality assurance, test project is provided in the directory ./gcanvas/test/ which includes three parts: Android test app, iOS test App and UI test project.
The diagram below shows their workflow in simple:
![gcanvas_testproject_architecture](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/gcanvas/gcanvas/7efe726528156d99a11c84496657577a/gcanvas_testproject_architecture.png)

#### 2.6.1 Test App & Test Project

Two test apps are provided to check the test environment and add test cases which are both the target apps of the test project.

Test app includes all the html cases operated by the test project by appium. If you want to run the cases exist, you can run the shell script directly;

but if you want to add js case, you need to change the TestApp, for detail you can read the readme.


#### 2.6.2 Test Environment

Appium: Appium1.5.3

java: Maven 3.3.9，JDK 1.8.0_111

iOS (required) : Xcode 7

Android: Android device


#### 2.6.3 Cases

The test cases can be divided into two parts, one for gwebgl.js and another for gcanvas.js, both include the scenerio rendered by WebView and GCanvasView as you can see in TestApp.

gcanvas.js test suite: refer to the test suite of the html canvas on w3school;

gwebgl.js test suite: water ball, shark and others are drawed by the methods gwebgl.js provided.


#### 2.6.4 Test Results

After all the environment is ready, the result can be outputed by running the run-iOS.sh/run-Android.sh/run-all.sh. A series of screenshots will be gotten after running the shell script to be matched with the base screenshots by the PicMatch Engine.  
Pure_pixel_match and pHash are two methods can be used in the project, the former is suitable for testing in the same environment (especially the test device) and the result will not be so good if different size devices are used,
the latter is good for testing in different environment (such different test devices), but the accuracy will be decreased for less pixel test cases (such as thin lines, small box).


#### 2.6.5 Others

In order to ensure and improve the quality of the GCanvas test code, we have refer some test code specification (link)

For more information about GCanvas test , such as environment installation, test project, test cases, you can read in the readme document.

## 3. Future

For now, GCanvas has a few limitation:
- does not support video
- part of WebGL APIs has been implemented, but not well covered.
- does not support new-genernation GPU architectures

In the future, GCanvas will
- fully support WebGL APIs
- upgrade to OpenGL ES 3.0
- integrate Vulkan & Metal
- have independent JavaScript runtime

VR/AR market is very hot. GCanvas is going to provide more capabilities in-application for developers to practice in these areas.

And, we also wants to try combining server-side rendering into GCanvas engine for high-quality realtime rendering on mobile phones.

## 4. Last

Last but not least, this project is inspired by a lot of open source projects instead of building from nothing. We should give special thanks to these projects: Chromium, Skia, Ejecta, Fast-Canvas, Cocos2d.
We hope this project can provide more value to the community as we do gain a lot from it.

<h5 align = "right">- GCanvas Open Source Team</h5>
