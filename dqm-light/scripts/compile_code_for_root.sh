#!/bin/bash

if [ -r myexe ]; then
  /bin/rm -rf myexe
fi

if [ -r $1 ]; then
  echo $1 "will compile soon"
  g++ -g -std=c++0x -I /usr/include/root $1 `root-config --libs --glibs` -o myexe
  ls -ltF myexe
else
  echo "any file for compilation is missing"
fi

if [ -r myexe ]; then
  ./myexe
else 
  echo "executable is missing"
fi
