
FROM ubuntu:18.04

ADD ./sources.list /etc/apt/sources.list

RUN apt-get update && apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev xvfb wget


ADD cmake.zip ./cmake/

RUN apt-get install -y zip && cd ./cmake/ && unzip cmake.zip 


ENV PATH="/cmake/cmake/bin/:${PATH}"

RUN apt-get install -y curl && curl -sL https://deb.nodesource.com/setup_10.x | bash -

RUN apt-get install -y nodejs 


ADD ./freetype.tar ./freetype/

RUN cd ./freetype/freetype-2.8.1  && ./configure && make && make install

RUN npm config set registry https://registry.npm.taobao.org && npm install -g cmake-js
#install curl-dev lib 
RUN apt-get install libcurl4-openssl-dev 

ENV DISPLAY=':99.0'


