#!/bin/bash

if [[ -z "$BUILD_HOME" ]]; then
  echo BUILD_HOME should be defined !!!
  exit
else
  echo BUILD_HOME  ${BUILD_HOME}
fi

if [ -r myDQMlight ]; then
  /bin/rm -rf ./myDQMlight
fi

if [ -r $1 ]; then
  echo $1 "will compile soon"
  g++ -g -std=c++0x -I /usr/local/Cellar/root/5.34.32/include/root -I${BUILD_HOME}/gemdaq-testing/gemreadout/include $1 `root-config --libs --glibs` -o myDQMlight
  ls -ltF myDQMlight
else
  echo "any file for compilation is missing"
fi

if [ -r myDQMlight ]; then
  ./myDQMlight
else 
  echo "executable is missing"
fi
