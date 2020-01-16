
isLocal=false
if  [  -n "$1" ] ;then
      if [ "$1" = "true" ]; then
         isLocal=true
      fi
fi
if [ "$isLocal" = "true" ];then
    echo "local build"
    echo $PWD
    npm run dev
else
    echo "remote build"
    git clone https://github.com/alibaba/GCanvas.git && cp  -R ./GCanvas/core ./core/
fi