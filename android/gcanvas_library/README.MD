# 模块说明
此模块为最终输出到端内gcanvas的模块，产物为gcanvas_library aar文件，aar内包含c++层实现和java层实现，
其中c++层文件含gcanvas.so, freetype.so，而java层主要是gcanvas view上层类接口。


# build说明

### AAR
./gradlew :android:gcanvas_library:assembleDebug (debug包)
./gradlew :android:gcanvas_library:assembleRelease (release包)






