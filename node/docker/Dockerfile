FROM ubuntu:18.04

RUN apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev xvfb wget

RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2.tar.gz 


RUN apt-get install -y zip && tar -zxvf cmake-3.15.2.tar.gz 

ENV PATH="/cmake-3.15.2/bin/:${PATH}"

RUN apt-get install -y curl && curl -sL https://deb.nodesource.com/setup_10.x | bash -

RUN apt-get install -y nodejs 

RUN export DISPLAY=':99.0' && Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &

RUN wget https://launchpadlibrarian.net/364945399/freetype_2.8.1.orig.tar.gz && tar -xvf freetype_2.8.1.orig.tar.gz

RUN cd ./freetype-2.8.1 && ./configure && make && make install

RUN npm config set registry https://registry.npm.taobao.org && npm install -g cmake-js
#install curl-dev lib 
RUN apt-get install libcurl4-openssl-dev libjpeg-dev