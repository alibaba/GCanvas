if [ ! -d "build" ]; then
    mkdir build  && cd build && touch test.cc && cd. .
fi


if [ -z "$1" ]
  then
    echo "you should input the casename match the js file"
    exit 1;
fi

node app.js $1