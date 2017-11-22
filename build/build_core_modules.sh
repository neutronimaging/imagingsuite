#!/bin/bash
REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-moduleconfig
cd $DEST/build-moduleconfig

$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/core/modules/ModuleConfig/qt/ModuleConfig/ModuleConfig.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-readerconfig
cd $DEST/build-readerconfig

$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/core/modules/ReaderConfig/ReaderConfig.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-readerGUI
cd $DEST/build-readerGUI

$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/core/modules/ReaderGUI/ReaderGUI.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

if [ -e "$REPOSPATH/core/modules/UnitTests"]
then
    echo "Build tests"

    for f in `ls $REPOSPATH/core/modules/UnitTests`
    do
        echo "$REPOSPATH/core/modules/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/core/modules/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile -o Makefile ../../imagingsuite/core/kipl/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi

    done

    echo "Tests built"
fi
