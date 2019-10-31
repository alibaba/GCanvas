# BridgeModule
为GCanvas提供 Bridge 桥接层的基础功能。由于存在不同的桥接层，提供了一组通用的桥阶层的功能，为方便实现不同GCanvas的桥接层实现(如，Weex/ReactNaitve等)。

## 类与接口
### GCanvasModuleProtocol

定义模块的接口，不同的桥接层模块Module去实现它

### GCanvasViewProtocol

定义渲染视图的接口，不同桥接层组件Component去实现它

### GCanvasModule

GCanvas模块的实现和生命周期的管理，管理多个GCanvas Component。

功能包括初始化、GCanvas类型设置、日志设置、2D单帧渲染调用、图片预加载、纹理绑定、WebGL渲染调用等，具体参看相关代码。

### GCanvasPlugin 

对GCanvas C++内核的封装，并提供2D和WebGL的渲染指令执行接口。

### GCanvasObject
componentId, GCanvasPlugin, Component建立对关联关系 

## Bridge实现

### WeexBridge实现

* Component

```
@interface WXGCanvasComponent : WXComponent<GCanvsViewPrtocol>

```

* Module

```
@interface WXGCanvasModule()<GCanvasModuleProtocol, GCVImageLoaderProtocol>

@property (nonatomic, strong) GCanvasModule  *gcanvasModule;

@end

```