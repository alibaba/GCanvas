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
        npm install cmake-js
        # mkdir -p external/cmake
        # pushd external/cmake
        # wget https://cmake.org/files/v3.8/cmake-3.8.0-Linux-x86_64.sh
        # chmod +x cmake-*-Linux-x86_64.sh
        # ./cmake-*-Linux-x86_64.sh --exclude-subdir --skip-license
        # export PATH="${PWD}/bin:$PATH"
        # cmake -version
        # popd
        apt-get update && apt-get install -y cmake
        apt-get install -y libcurl4-openssl-dev
        apt-get install -y libglfw3-dev libgles2-mesa-dev libboost-all-dev xvfb wget curl nodejs --fix-missing
        apt-get install -y libjpeg-dev libcairo2-dev libpango1.0-dev libglib2.0-dev --fix-missing
    fi
fi


