#!/bin/bash

platformSupport=0
unamestr=`uname`

if [[ "$unamestr" == 'Darwin' ]] ;then
    platform='mac osx'
    platformSupport=1
elif [[ "$unamestr" == 'Linux' ]]; then
    platform='linux'
    platformSupport=2
    mkdir -p ~/.gAssets/fonts/ &&  cp ./node/third_party/font/wqy-microhei.ttc ~/.gAssets/fonts/wqy-microhei.ttc &&  cp ./node/third_party/font/.fontcache  ~/.gAssets/fonts/.fontcache
fi

if [ $platformSupport -eq 0 ]; then
    echo "not support at $unamestr"
    exit 0
else
    echo "start build at $platform"

    if [ $platformSupport -eq 1 ]; then
        # brew update
        brew install cairo pango libpng jpeg-turbo curl-openssl
    else 
        # apt-get update
        apt-get install -y cmake libglfw3-dev libgles2-mesa-dev libboost-all-dev xvfb wget curl nodejs
        apt-get install -y libcurl4-openssl-dev libjpeg-dev libcairo2-dev libpango1.0-dev libglib2.0-dev
    fi
    npm install cmake-js
    npm run compile
fi


