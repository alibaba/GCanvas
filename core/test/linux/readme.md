### GCanvas Linux BenchMark 套件说明文档

#### 如何搭建环境？
   * 操作系统：Linux Ubuntu-18.04
   * 编译工具
       * cmake
       * g++
       * make 
   * OpenGL ES 在屏环境 
      ``` 
      sudo apt-get install libglfw3-dev libgles2-mesa-dev
      ``` 
   
   * OpenGL ES 离屏环境
      ``` 
        sudo apt-get update && sudo apt-get install libglfw3-dev libgles2-mesa-dev // 安装mesa，glfw3
      ``` 
   * 安装Boost库
      ``` 
        sudo apt-get install libboost-all-dev 
      ``` 
### 如何构建？
  
  * 构建离屏环境
      ``` 
        cd GCanvas/core/test/linux/ && mkdir build && cd build // 创建build 目录
        cmake ../ && make //使用cmake生成make文件，然后进行make
        ls -al | grep "gcanvasTest" //查看构建输出的可运行文件
      ``` 
  * 构建在屏环境
      * 修改CMakeLists.txt,打开在屏编译，关闭离屏
        ```
         ./mainOnScreen.cpp
         #./mainOffScreen.cpp 
        ```
    
### 如何运行？
   * 运行gcanvasTest之前先运行w3c的标准测试案例，运行方式见[w3c测试套件](../w3c/readme.md)
   * 运行./gcanvasTest，进行可以看到case的名字和运行结果
    ```
      ------------------
      the case name is fillRect
      ------------------
      the correct ratio is 0.499136  
    ```
  
### 如何得到输出结果