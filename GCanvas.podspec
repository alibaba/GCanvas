Pod::Spec.new do |s|

  s.name         = "GCanvas"
  s.version      = "0.1.1"
  s.summary      = "GCanvas Source."

  s.description  = <<-DESC
                   A cross-platform fast Canvas render engine.
                   DESC

  s.homepage     = 'https://github.com/alibaba/GCanvas'
  s.license = {
    :type => 'Copyright',
    :text => <<-LICENSE
           Alibaba-INC copyright
    LICENSE
  }

  s.author       = { 'jwxbond' => 'jwxbond@gmail.com' }

  s.platform     = :ios

  s.ios.deployment_target = '7.0'

 # s.source =  { :path => '.' }
  s.source =  { :git => "https://github.com/alibaba/GCanvas.git", :tag => s.version }

  s.source_files = 'GCanvas/ios/Classes/**/*.{h,m,mm}',
                   'GCanvas/core/src/GCanvas.{hpp,cpp}',
                   'GCanvas/core/src/GCanvasManager.{h,cpp}',
                   'GCanvas/core/src/gcanvas/shaders/*.glsl',
                   'GCanvas/core/src/gcanvas/GCanvas2dContext.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GConvert.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GFillStyle.h',
                   'GCanvas/core/src/gcanvas/GPath.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GPoint.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GShader.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GShaderManager.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GTextDefine.h',
                   'GCanvas/core/src/gcanvas/GTexture.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GTransform.h',
                   'GCanvas/core/src/gcanvas/GTriangulate.{h,cpp}',
                   'GCanvas/core/src/gcanvas/GWebglContext.{h,cpp}',
                   'GCanvas/core/src/memory/*.h',
                   'GCanvas/core/src/png/**/*.*',
                   'GCanvas/core/src/support/DynArray.h',
                   'GCanvas/core/src/support/Encode.{h,cpp}',
                   'GCanvas/core/src/support/Lesser.h',
                   'GCanvas/core/src/support/Log.{h,cpp}',
                   'GCanvas/core/src/support/Util.{h,cpp}'

  s.public_header_files = 'GCanvas/ios/Classes/**/*.h', 'GCanvas/core/src/gcanvas/GTextDefine.h'

  s.user_target_xcconfig  = { 'FRAMEWORK_SEARCH_PATHS' => "'$(PODS_ROOT)/GCanvas'" }

  s.requires_arc = true

  s.frameworks = 'Foundation', 'UIKit', 'GLKit', 'OpenGLES', 'CoreText'

  s.library = 'stdc++'

  s.pod_target_xcconfig = { 'OTHER_CFLAGS'  => '-DIOS' }

end
