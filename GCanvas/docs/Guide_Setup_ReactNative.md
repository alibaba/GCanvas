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

1. Open `android/app/src/main/java/[...]/MainActivity.java`
2. Add `import com.reactlibrary.RNReactNativeGCanvasPackage;` to the imports at the top of the file
3. Add `new RNReactNativeGCanvasPackage()` to the list returned by the `getPackages()` method
4. Append the following lines to `android/settings.gradle`:
   `include ':react-native-gcanvas'`
   `project(':react-native-gcanvas').projectDir = new File(rootProject.projectDir,     '../node\_modules/react-native-gcanvas/android')`
5. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
   `compile project(':react-native-gcanvas')`


## Usage
```javascript
import GCanvasView from 'react-native-gcanvas';

...

render() {
    return (
   		<GCanvasView ref='gcanvasRef' style={width: 414, height :736, backgroundColor: '#FF000030'}>
        </GCanvasView>
    );
  }
```
