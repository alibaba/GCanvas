Pod::Spec.new do |s|
  
  s.name         = "WeexGcanvas"
  s.version      = "1.1.0"
  s.summary      = "GCanvas Weex Bridge Source."

  s.description  = <<-DESC
                   GCanvas Weex Bridge Source.
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

  s.source_files  = "bridges/weex-gcanvas/ios/sdk/Sources/*.{h,m,mm}"

  s.public_header_files = 'bridges/weex-gcanvas/ios/sdk/Sources/*.h'

  s.user_target_xcconfig  = { 'FRAMEWORK_SEARCH_PATHS' => "'$(PODS_ROOT)/WeexGcanvas'" }

  s.requires_arc = true

  s.dependency "WeexPluginLoader"
  s.dependency "WeexSDK"
  s.dependency "GCanvas"
  s.dependency "SDWebImage"
end
