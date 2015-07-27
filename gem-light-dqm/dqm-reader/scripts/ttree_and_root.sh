#!/bin/bash

if [ -r myexe ]; then
  /bin/rm -rf myTTreeDQMLight
fi

if [ -r $1 ]; then
  echo $1 "will compile soon"
  g++ -g -std=c++0x -I/usr/include/root -I${BUILD_HOME}/gemdaq-testing/gemreadout/include -I${BUILD_HOME}/dqm-light/include $1 `root-config --libs --glibs` -o myTTreeDQMLight
  ls -ltF myTTreeDQMLight
else
  echo "any file for compilation is missing"
fi

if [ -r myTTreeDQMLight ]; then
  ./myTTreeDQMLight
else 
  echo "executable is missing"
fi
