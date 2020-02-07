#!/bin/bash

DEST_DEPENDENCY=$WORKSPACE/imagingsuite/external/src/lmfit/lmfit_install
DEST=$WORKSPACE/builds

mkdir $WORKSPACE/imagingsuite/external/src/lmfit/build
mkdir $DEST_DEPENDENCY
cd $WORKSPACE/imagingsuite/external/src/lmfit/build

command -v cmake >/dev/null 2>&1 || { echo >&2 "I require cmake but it's not installed.  Skipping."; exit 0; }

cmake -DCMAKE_INSTALL_PREFIX=$DEST_DEPENDENCY .. 
make
make install
cp $DEST_DEPENDENCY/lib/* $WORKSPACE/lib
