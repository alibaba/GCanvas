# Chnagelog

All notable changes of this project will be documented in.


## [1.1.2] - 2020-07-07
### Changed
* Core Engine
  * Support Linux platform
  * Add NodeJS binding of `GCanvas`
  * Fix some 2d issues

* GCanvas Component
  `Weex` has been archived and not updated。 We will delete `/bridges` folder, and no further update about GCanvas bridge components such as `WeexGCanvas` and `ReactNativeGCanvas`. 
  
## [1.1.1] - 2019-12-30
### Changed
* GCanvas Project
  * Add `travi-ci` 
  * Add benchmark and cppcheck

## [1.1.0] - 2019-10-31

### Changed

* Core Engine
	* Refacotor `GCanvas` and `GCanvasContext` class, make it more lower coupling. `GCanvas` response for initialize, create `GCanvasContext` and drawFrame, all 2d `property` and `API` move to `GCanvasContext`
	* Add `GCanvasWeex` only for `Weex` UI framework
	* Add `GCanvasHooks` and `GCanvasLog`, support for custom exception log.
	* `GCanvasContext` Support set canvas dimension
	* New `2d` property `lineDash` and `lineDashOffset`
	* New `2d` property support `shadowColor`,`shadowBlur`, `shadowOffsetX`,`shadowOffstY`
	* Font support `measureText` and `Italic` style
	* `Fill()`、`Clip()` , support winding-rule and promote performance
	* Out of Android platform- implementation :`GCanvas2DContextAndroid`,`GCanvasAndroid`,`GFontManagerAndroid`,
	* Refactor Android-Weex `GcanvasWeexAndroid` 
	* Fix some compatibility issue


	
* Weex GCanvas Component
	* Update New `WeexSDK` (>=0.26.0)
	* [iOS] `iOS` deployment_target update to iOS 9.0
	* [Android]`Android`:moudle bridge_spec source code depend on the module gcanvas_library
