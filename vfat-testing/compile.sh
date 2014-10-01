mkdir build 
cd build
cmake ..
make clean
echo ${CXXFLAGS}
make CXXFLAGS="-O0 -g3 -fno-inline"
echo "Deleting Temporary File created during compilation"
cd ..
#rm -rf build 
