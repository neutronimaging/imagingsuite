#!/bin/sh

pushd .

mkdir -p ../../build/cmake
cd ../../build/cmake
cmake ../../imagingsuite/ -DPYTHON_EXECUTABLE=$(command -v python3) -DCMAKE_INSTALL_PREFIX=../../lib -DCMAKE_BUILD_TYPE=Release -DDYNAMIC_LIB=ON
cmake --build . --target install

cd ../../lib/lib
../../imagingsuite/frameworks/tomography/nexuslinkage.sh
for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done
popd .