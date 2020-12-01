/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <napi.h>
#include <iostream>


#include "CairoCanvas.h"
#include "CairoCanvasGradient.h"
#include "CairoCanvasPattern.h"
#include "CairoCanvasRenderingContext2d.h"
#include "CairoImage.h"
#include "CairoImageData.h"

#ifndef __APPLE__

#include "Canvas.h"
#include "Image.h"
#include "TextMetrics.h"
#include "./webgl/WebGLShader.h"
#include "./webgl/WebGLBuffer.h"
#include "./webgl/WebGLProgram.h"
#include "./webgl/WebGLTexture.h"
#include "./webgl/WebGLFrameBuffer.h"
#include "./webgl/WebGLActiveInfo.h"
#include "./webgl/WebGLUniformLocation.h"
#include "./webgl/WebGLRenderBuffer.h"

#endif

static bool useCairo = true;
bool checkUseCairo()
{
  #ifdef USE_CAIRO
    return true;
  #endif

  FILE *fstream = NULL;      
  if(NULL == (fstream = popen("lspci | grep VGA", "r")))      
  {     
    fprintf(stderr,"execute command failed: %s", strerror(errno));      
    return true;      
  }   

  bool hasGPUInfo = false;
  char gpuInfoBuffer[1024] = {0};
  while( NULL != fgets(gpuInfoBuffer,  sizeof(gpuInfoBuffer), fstream) ) 
  { 
    hasGPUInfo = true;
  }  
  pclose(fstream); 
  return !hasGPUInfo;
}

Napi::Object createCanvas(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() < 2)
  {
    Napi::TypeError::New(env, "createCanvas need width & height").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }
  
 
#ifdef __APPLE__
  std::cout << "createCanvas use cairo in MacOS" << std::endl;
  return cairocanvas::Canvas::NewInstance(env, info[0], info[1]);
#else
  useCairo =  checkUseCairo();

   if( info.Length() >= 3 && info[2].IsBoolean() )
  {
    std::cout << "createCanvas use cairo set in param" << std::endl;
    useCairo = info[2].As<Napi::Boolean>().ToBoolean();
  }

  if( useCairo ){
    std::cout << "createCanvas use cairo in Unix"  << std::endl;
    return cairocanvas::Canvas::NewInstance(env, info[0], info[1]);
  } else {
    std::cout << "createCanvas use gcanvas in Unix"  << std::endl;
    return NodeBinding::Canvas::NewInstance(env, info[0], info[1]);
  }
#endif 
}

Napi::Object createImage(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

#ifdef __APPLE__
  std::cout << "createImage use cairo in MacOS" << std::endl;
  return cairocanvas::Image::NewInstance(env);
#else
  useCairo =  checkUseCairo();

  if( useCairo ) {
    std::cout << "createImage use cairo in Unix"  << std::endl;
    return cairocanvas::Image::NewInstance(env);
  }else{
    std::cout << "createImage use gcanvas in Unix"  << std::endl;
    return NodeBinding::Image::NewInstance(env);
  }
#endif
}

void registerParseFont(const Napi::CallbackInfo &info )
{
  Napi::Env env = info.Env();
  if ( info.Length() == 1 && info[0].IsFunction() )
  {
    cairocanvas::Context2d::registerParseFont(info[0].As<Napi::Function>());
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  //export  all binding class

  //2d Cairo 
  cairocanvas::Canvas::Init(env, exports);
  cairocanvas::Context2d::Init(env, exports);
  cairocanvas::Pattern::Init(env);
  cairocanvas::Gradient::Init(env);
  cairocanvas::Image::Init(env, exports);
  cairocanvas::ImageData::Init(env);

#ifndef __APPLE__
  //2d gcanvas
  NodeBinding::Canvas::Init(env, exports);
  NodeBinding::Image::Init(env, exports);
  NodeBinding::Context2D::Init(env);
  NodeBinding::ContextWebGL::Init(env);
  NodeBinding::Gradient::Init(env);
  NodeBinding::ImageData::Init(env);
  NodeBinding::TextMetrics::Init(env);
  NodeBinding::Pattern::Init(env);

  //webgl
  NodeBinding::WebGLShader::Init(env);
  NodeBinding::WebGLProgram::Init(env);
  NodeBinding::WebGLBuffer ::Init(env);
  NodeBinding::WebGLTexture::Init(env);
  NodeBinding::WebGLFrameBuffer::Init(env);
  NodeBinding::WebGLRenderBuffer::Init(env);
  NodeBinding::WebGLActiveInfo::Init(env);
  NodeBinding::WebGLUniformLocation::Init(env); 
#endif

  //export functions
  exports.Set(Napi::String::New(env, "registerParseFont"), Napi::Function::New(env, registerParseFont));
  exports.Set(Napi::String::New(env, "createCanvas"), Napi::Function::New(env, createCanvas));
  exports.Set(Napi::String::New(env, "createImage"), Napi::Function::New(env, createImage));
  return exports;
}

NODE_API_MODULE(canvas, Init)
