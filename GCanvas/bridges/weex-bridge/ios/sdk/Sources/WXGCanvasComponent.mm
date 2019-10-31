/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#import "WXGCanvasComponent.h"
#import <GLKit/GLKit.h>
#import <WeexPluginLoader/WeexPluginLoader.h>
#import <WeexSDK/WeexSDK.h>
#import <WeexSDK/WXComponent+Layout.h>

#import <GCanvas/GCVCommon.h>
#import <GCanvas/GCanvasModule.h>

@interface WXGCanvasComponent()

@property (nonatomic, strong) GLKView   *glkview;
@property (nonatomic, assign) CGSize    viewSize;
@property (nonatomic, assign) CGRect    componetFrame;
@property (nonatomic, assign) BOOL      isOffscreen;

@end

@implementation WXGCanvasComponent

WX_PlUGIN_EXPORT_COMPONENT(gcanvas,WXGCanvasComponent)


/**
 *  @abstract Initializes a new component using the specified  properties.
 *
 *  @param ref          the identity string of component
 *  @param type         component type
 *  @param styles       component's styles
 *  @param attributes   component's attributes
 *  @param events       component's events
 *  @param weexInstance the weexInstance with which the component associated
 *
 *  @return A WXComponent instance.
 */
- (instancetype)initWithRef:(NSString *)ref
                       type:(NSString*)type
                     styles:(nullable NSDictionary *)styles
                 attributes:(nullable NSDictionary *)attributes
                     events:(nullable NSArray *)events
               weexInstance:(WXSDKInstance *)weexInstance{
    GCVLOG_METHOD(@"ref=%@, type=%@, styles=%@, attributes=%@, events=%@, weexInstance=%@", ref, type, styles, attributes, events, weexInstance)
    self = [super initWithRef:ref type:type styles:styles attributes:attributes events:events weexInstance:weexInstance];
    if (self ){
        //indicate offscreen gcanvas, while set offscreen=1 in styles
        if( styles[@"offscreen"] ){
            self.isOffscreen = [styles[@"offscreen"] integerValue] == 1;
        }
        
        //get style from flexCssNode
        CGSize size = [[UIScreen mainScreen] bounds].size;
        float x=0, y=0;
        if (!isnan(_flexCssNode->getStylePositionLeft())) {
            x = _flexCssNode->getStylePositionLeft();
        }
        if(!isnan(_flexCssNode->getStylePositionTop())){
            y = _flexCssNode->getStylePositionTop();
        }
        if(!isnan(_flexCssNode->getStyleWidth())){
            size.width = _flexCssNode->getStyleWidth();
        }
        if(!isnan(_flexCssNode->getStyleHeight())){
            size.height = _flexCssNode->getStyleHeight();
        }
        self.viewSize = size;
        float r = self.weexInstance.pixelScaleFactor;
        self.componetFrame = CGRectMake(x, y, size.width/r, size.height/r);
        self.needChangeEAGLContenxt = YES;
    }
    
    return self;
}


- (void)dealloc{
    [EAGLContext setCurrentContext:nil];
}

- (UIView *)loadView{
    if( !self.glkview ){
        GLKView *glkview = [[GLKView alloc] initWithFrame:CGRectMake(0, 0, self.viewSize.width, self.viewSize.height)];
        glkview.enableSetNeedsDisplay = YES;
        glkview.userInteractionEnabled = YES;
        glkview.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        glkview.drawableStencilFormat = GLKViewDrawableStencilFormat8;
        glkview.backgroundColor = [UIColor clearColor];
        
        self.glkview = glkview;
        
        [[NSNotificationCenter defaultCenter] postNotificationName:kGCanvasCompLoadedNotification object:nil userInfo:@{@"componentId":self.ref}];
    }
    return self.glkview;
}

- (NSString*)componentId{
    return self.ref;
}

- (CGFloat)devicePixelRatio{
   return self.calculatedFrame.size.width * [UIScreen mainScreen].nativeScale / self.componetFrame.size.width ;
}

@end
