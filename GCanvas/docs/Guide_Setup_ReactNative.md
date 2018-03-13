### Automatic install:
`react-native link react-native-gcanvas`

### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`
2. Go to `node\_modules` ➜ `react-native-gcanvas` and add `RNReactNativeGCanvas.xcodeproj`
3. In XCode, in the project navigator, select your project. Add `libRNReactNativeGCanvas.a` to your project's `Build Phases` ➜ `Link Binary With Libraries`
4. Run your project (`Cmd+R`)


### Android

1. Open `android/app/src/main/java/[...]/MainActivity.java`
2. Add `import com.reactlibrary.RNReactNativeGCanvasPackage;` to the imports at the top of the file
3. Add `new RNReactNativeGCanvasPackage()` to the list returned by the `getPackages()` method
4. Append the following lines to `android/settings.gradle`:
   `include ':react-native-gcanvas'`
   `project(':react-native-gcanvas').projectDir = new File(rootProject.projectDir,     '../node\_modules/react-native-gcanvas/android')`
5. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
   `compile project(':react-native-gcanvas')`
