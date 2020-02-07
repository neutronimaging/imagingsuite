#!/bin/bash

DEST_DEPENDENCY=$WORKSPACE/imagingsuite/external/src/lmfit/lmfit_install
DEST=$WORKSPACE/builds

mkdir $WORKSPACE/imagingsuite/external/src/lmfit/build
mkdir $DEST_DEPENDENCY
cd $WORKSPACE/imagingsuite/external/src/lmfit/build

cmake -DCMAKE_INSTALL_PREFIX=$DEST_DEPENDENCY .. 
make
make install
cp $DEST_DEPENDENCY/lib/* $WORKSPACE/lib
