### Font Tools 使用
  
     * font Tools 工具编译
         ```
            g++ -o fonttool.out fonttool.cpp -I../../../../node/third_party/font/freetype2/ -lfreetype -lboost_system -lboost_filesystem
         ```

    * 打包fontcache
          ```
            ./fonttool.out buildCache `your ttc file path`
         ```
       eg:
         ```
              ls -a
              fontool.out  wqy-microhei.ttc  站酷酷黑_0.ttf
            ./fontool.out dumpCache .fontcache 
             
         ```
    
       * dump打包完成的fontcache 验证
             ```
                ./fontool.out dumpCache .fontcache 
            ```

             ```
            the face.source is /home/mingzong/.gAssets/fonts/./站酷酷黑_0.ttf 
            the face.psName is HuXiaoBoKuHei 
            the face.familyName is HuXiaoBo_KuHei 
            the face.styleName is Regular 
            the face.source is /home/mingzong/.gAssets/fonts/./wqy-microhei.ttc 
            the face.psName is WenQuanYiMicroHei 
            the face.familyName is WenQuanYi Micro Hei 
            the face.styleName is Regular 
             the face.source is /home/mingzong/.gAssets/fonts/./wqy-microhei.ttc 
            the face.psName is WenQuanYiMicroHeiMono 
            the face.familyName is WenQuanYi Micro Hei Mono 
            the face.styleName is Regular 
            [DEBUG] Number of cached typefaces: 3
            [DEBUG] Typeface: HuXiaoBo_KuHei, Regular. (HuXiaoBoKuHei)
            [DEBUG]     source type: TST_LOCAL
            [DEBUG]     source: /home/mingzong/.gAssets/fonts/./站酷酷黑_0.ttf
            [DEBUG]     index: 0
            [DEBUG]     face: (nil)
            [DEBUG] Typeface: WenQuanYi Micro Hei, Regular. (WenQuanYiMicroHei)
            [DEBUG]     source type: TST_LOCAL
            [DEBUG]     source: /home/mingzong/.gAssets/fonts/./wqy-microhei.ttc
            [DEBUG]     index: 0
            [DEBUG]     face: (nil)
            [DEBUG] Typeface: WenQuanYi Micro Hei Mono, Regular. (WenQuanYiMicroHeiMono)
            [DEBUG]     source type: TST_LOCAL
            [DEBUG]     source: /home/mingzong/.gAssets/fonts/./wqy-microhei.ttc
            [DEBUG]     index: 1
            [DEBUG]     face: (nil)
        ```