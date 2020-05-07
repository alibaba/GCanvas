
isLocal=false
if  [  -n "$1" ] ;then
      if [ "$1" = "true" ]; then
         isLocal=true
      fi
fi
if [ "$isLocal" = "true" ];then
    echo "local build"
else
    echo "remote build"
    mkdir -p ~/gcanvasAssets/fonts/
    git clone https://github.com/alibaba/GCanvas.git && cd ./GCanvas/ && git checkout node_canvas && cd .. && cp  -R ./GCanvas/core ./core/  && npm run compile && cp ./third_party/font/wqy-microhei.ttc ~/gcanvasAssets/fonts/wqy-microhei.ttc &&  cp ./third_party/font/.fontcache  ~/gcanvasAssets/fonts/.fontcache
fi