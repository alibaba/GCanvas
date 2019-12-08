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

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@protocol GCanvasViewProtocol

@property (nonatomic, strong, readonly) GLKView* glkview;
@property (nonatomic, assign, readonly) CGRect componetFrame;

/**
 * offscreen UIView<GCanvasViewProtocol>
 */
@property (nonatomic, assign, readonly) BOOL isOffscreen;

/**
 * GCnavas plugin initilaise or refresh
 */
@property (nonatomic, assign) BOOL needChangeEAGLContenxt;


@required
- (CGFloat)devicePixelRatio;
- (NSString*)componentId;



@end
