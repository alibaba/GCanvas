Pod::Spec.new do |s|

  s.name         = "GCanvas"
  s.version      = "1.1.0"
  s.summary      = "GCanvas Source."

  s.description  = <<-DESC
                   A cross-platform fast Canvas render engine.
                   DESC

  s.homepage     = 'https://github.com/alibaba/GCanvas'
  s.license = {
    :type => 'Copyright',
    :text => <<-LICENSE
           Alibaba Inc. Copyright
    LICENSE
  }

  s.author       = { 'jwxbond' => 'jwxbond@gmail.com' }

  s.platform     = :ios

  s.ios.deployment_target = '9.0'

 # s.source =  { :path => '.' }
  s.source =  { :git => "https://github.com/alibaba/GCanvas.git", :tag => s.version }

  s.source_files = 'GCanvas/ios/BridgeModule/*.{h,m,mm}',
                   'GCanvas/core/src/export.h',
                   'GCanvas/core/src/GCanvas.{hpp,cpp}',
                   'GCanvas/core/src/GCanvasWeex.{hpp,cpp}',
                   'GCanvas/core/src/GCanvasManager.{h,cpp}',
                   'GCanvas/core/src/gcanvas/**/*.*',
                   'GCanvas/core/src/support/DynArray.h',
                   'GCanvas/core/src/support/Encode.{h,cpp}',
                   'GCanvas/core/src/support/GLUtil.{h,cpp}',
                   'GCanvas/core/src/support/Log.{h,cpp}',
                   'GCanvas/core/src/support/Util.{h,cpp}',
                   'GCanvas/core/src/platform/iOS/*.{h,m,mm}',
                   
  s.public_header_files =  'GCanvas/ios/BridgeModule/*.h', 
                           'GCanvas/core/src/export.h',
                           'GCanvas/core/src/GCanvas.hpp',
                           'GCanvas/core/src/GCanvasWeex.hpp',
                           'GCanvas/core/src/GCanvasManager.h',
                           'GCanvas/core/src/gcanvas/**/*.h',
                           'GCanvas/core/src/support/DynArray.h',
                           'GCanvas/core/src/support/Encode.h',
                           'GCanvas/core/src/support/GLUtil.h',
                           'GCanvas/core/src/support/Log.h',
                           'GCanvas/core/src/support/Util.h',
                           'GCanvas/core/src/platform/iOS/*.h',


  s.user_target_xcconfig  = { 'FRAMEWORK_SEARCH_PATHS' => "'$(PODS_ROOT)/GCanvas'",  
                              'HEADER_SEARCH_PATHS' => "'$(PODS_ROOT)/GCanvas'"}

  s.requires_arc = true

  s.frameworks = 'Foundation','UIKit','GLKit', 'CoreMedia', 'AVFoundation', 'OpenGLES', 'CoreText'

  s.library = 'c++'

  s.pod_target_xcconfig = { 'OTHER_CFLAGS'  => '-DIOS -DGCANVAS_WEEX', 'GCC_PREPROCESSOR_DEFINITIONS' => 'IOS' }

end
