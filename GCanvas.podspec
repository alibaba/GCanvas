Pod::Spec.new do |s|

  s.name         = "GCanvas"
  s.version      = "1.1.1"
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

  s.source_files = 'ios/BridgeModule/*.{h,m,mm}',
                   'core/src/export.h',
                   'core/src/GCanvas.{hpp,cpp}',
                   'core/src/GCanvasWeex.{hpp,cpp}',
                   'core/src/GCanvasManager.{h,cpp}',
                   'core/src/gcanvas/*.*',
                   'core/src/gcanvas/shaders/*.*',
                   'core/src/support/DynArray.h',
                   'core/src/support/Encode.{h,cpp}',
                   'core/src/support/GLUtil.{h,cpp}',
                   'core/src/support/Log.{h,cpp}',
                   'core/src/support/Util.{h,cpp}',
                   'core/src/platform/iOS/*.*'
                   
  s.public_header_files =  'ios/BridgeModule/*.h', 
                           'core/src/export.h',
                           'core/src/GCanvas.hpp',
                           'core/src/GCanvasWeex.hpp',
                           'core/src/GCanvasManager.h',
                           'core/src/gcanvas/**/*.h',
                           'core/src/support/DynArray.h',
                           'core/src/support/Encode.h',
                           'core/src/support/GLUtil.h',
                           'core/src/support/Log.h',
                           'core/src/support/Util.h',
                           'core/src/platform/iOS/*.h'


  s.user_target_xcconfig  = { 'FRAMEWORK_SEARCH_PATHS' => "'$(PODS_ROOT)/GCanvas'",  
                              'HEADER_SEARCH_PATHS' => "'$(PODS_ROOT)/GCanvas'"}

  s.requires_arc = true

  s.frameworks = 'Foundation','UIKit','GLKit', 'CoreMedia', 'AVFoundation', 'OpenGLES', 'CoreText'

  s.library = 'c++'

  s.pod_target_xcconfig = { 'OTHER_CFLAGS'  => '-DIOS -DGCANVAS_WEEX', 'GCC_PREPROCESSOR_DEFINITIONS' => 'IOS' }

end
