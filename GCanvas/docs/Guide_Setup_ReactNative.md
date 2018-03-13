## Getting started

`npm install react-native-gcanvas --save`

### Mostly automatic installation

`react-native link react-native-gcanvas`

### Manual installation


#### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`

2. Go to `node_modules` ➜ `react-native-gcanvas` and add `RCTGCanvas.xcodeproj`

3. Go to `GCanvas/ios`  and add `GCanvas.xcodeproj`

4. In XCode, in the project navigator, select your project. Add `libRCTGCanvas.a` and `GCanvas.fromework` to your project's `Build Phases` ➜ `Link Binary With Libraries`

5. Goto `GCanvas/bridges/react-native/bridge/examples/RNGCanvasSample` directory,  run `npm install ` install dependency library.

6. Run your project (`Cmd+R`)

7. Run result same as below, congratuation. 
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