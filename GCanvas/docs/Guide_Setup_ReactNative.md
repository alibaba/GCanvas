## Getting started

`npm install gcanvas.js --save`

`npm install react-native-gcanvas --save`

### Mostly automatic installation

`react-native link react-native-gcanvas`

In XCode, in the project navigator, select your project. 

1. Add `GCanvas.fromework` ( in the path `./node_modules/react-native-gcanvas/ios/GCanvas.framework`) to your project's `Build Phases` ➜ `Link Binary With Libraries`
2. Add `$(SRCROOT)/../node_modules/react-native-gcanvas/ios` to your project's `Build Settings` ➜ `Framework Search Path`

### Manual installation


#### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`

2. Go to `node_modules` ➜ `react-native-gcanvas` and add `RCTGCanvas.xcodeproj`

3. Go to `GCanvas/ios`  and add `GCanvas.xcodeproj`

4. In XCode, in the project navigator, select your project. Add `libRCTGCanvas.a` and `GCanvas.fromework` ( in the path `./node_modules/react-native-gcanvas/ios/GCanvas.framework`) to your project's `Build Phases` ➜ `Link Binary With Libraries`

   ​


Also you can run the given sample `RNGCanvasSample`, follow steps below:

1. Goto `GCanvas/bridges/react-native/bridge/examples/RNGCanvasSample` directory,  run `npm install ` install dependency library.
2. Run your project (`Cmd+R`)
3. Run result same as below, congratuation.
   <img src='https://img.alicdn.com/tfs/TB1E4OoX1OSBuNjy0FdXXbDnVXa-1242-2208.png' width="60%">



#### Android

1. Add dependencies to your gradle file
```groovy
    compile "com.taobao.gcanvas:corelib:1.0.4"
    compile "com.taobao.gcanvas.adapters:img:1.0.0"
    compile "com.taobao.gcanvas.adapters:img_fresco:1.0.1"
    compile "com.taobao.gcanvas.bridges:spec:1.0.1"
    compile "com.taobao.gcanvas.bridges:rn:1.0.4"
```
2. Add 'react-native-gcanvas' dependencies to your package.json
```json
{
    "dependencies": {
        "gcanvas.js": "^0.0.7",
        "react-native-gcanvas": "^0.0.5"
    }
}
```
gcanvas.js uses babel to compatible with ES6. If your environment has no babel installed, add below lines to your package.json
```json
{
    "dependencies": {
        "babel-cli": "^6.26.0",
        "babel-plugin-transform-decorators-legacy": "^1.3.4",
        "babel-preset-es2015": "^6.24.1",
        "babel-preset-stage-0": "^6.24.1"
    }
}
```
run ```npm install ``` to install all packages.
3. Register GCanvas ReactNative Package in your Application class:
```java
 @Override
protected List<ReactPackage> getPackages() {
    return Arrays.<ReactPackage>asList(
        new MainReactPackage(),
        new GReactPackage()
    );
}
```


## Usage
```javascript
 import {
   GCanvasView,
 } from 'react-native-gcanvas';
 
 import {
   requireNativeComponent,
   findNodeHandle,
   NativeModules
 } from 'react-native';
 
 import { enable, ReactNativeBridge, Image as GImage } from "gcanvas.js/src/index.js";
 
 export default class App extends Component<Props> {
    .....
    {
       var ref = this.refs.canvas_holder;
    
       var canvas_tag = findNodeHandle(ref);
       // var canvas_tag = "2";
       var el = { ref:""+canvas_tag, style:{width:414, height:376}};
    
       ref = enable(el, {bridge: ReactNativeBridge});
    
       var ctx = ref.getContext('2d');
       //rect
       ctx.fillStyle = 'green';
       ctx.fillRect(0, 0, 100, 100);
    
       //rect
       ctx.fillStyle = 'black';
       ctx.fillRect(100, 100, 100, 100);
       ctx.fillRect(25, 205, 414-50, 5);  
    }
    
    render() {
        return (
              <GCanvasView ref='canvas_holder'>
              </GCanvasView>
        );
      }
 }
```
