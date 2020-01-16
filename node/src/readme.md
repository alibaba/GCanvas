### 环境搭建

  * node 版本 >=10.0
  * 系统环境：ubuntu >=18:04
 *  工具链:cmake
 *  依赖安装
       * sudo apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev
       * [freetype字体库](http://www.linuxfromscratch.org/blfs/view/svn/general/freetype2.html)

### Node-canvas  源代码构建以及测试运行
 默认已经使用docker环境进行node-canvas的源代码构建和运行，node-canvas使用node-addon的binding的api以及使用cmake-js进行构建。  

构建步骤如下
  
 ``` 
  npm install cmake-js -g //安装cmake-js
  npm install // 安装项目所需要的依赖
  npm run dev // 构建node-addon的binding中间键
 ``` 

运行步骤

 ``` 
   case=app.js npm run test //运行example下的js文件，输出运行结果，可以从导出的png图片中看
 ```