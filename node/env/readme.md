### node-canvas 开发以及运行环境搭建

node-canvas 使用docker搭建运行环境，此文件夹为构建docker环境的材料文件夹。搭建node-canvas的docker环境步骤如下：

* 安装docker
   
* 构建docker image 
   ```
    cd GCanvas/node/env    //进入env目录
    docker build -t gcanvas .
   ```
* 完成docker image的构建后，根据构建好的docker image启动容器,并且添加为docker添加gcanvas工程文件的卷，让gcanvas的工程文件映射到docker容器中去。

   ```
    cd ../../../      //退回到GCanvas的上级目录
    docker run -it -v $pwd/GCanvas/:/home/ --name node_canvas gcanvas "/bin/bash"
   ```

* 启动完成后,构建代码按照[代码构建](../src/readme.md)文档来进行构建即可 