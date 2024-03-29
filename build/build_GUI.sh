#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-QtAddons
cd $DEST/build-QtAddons

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/GUI/qt/QtAddons/QtAddons.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-readerGUI
cd $DEST/build-readerGUI

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/core/modules/ReaderGUI/ReaderGUI.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-QtModuleConfigure
cd $DEST/build-QtModuleConfigure

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/GUI/qt/QtModuleConfigure/QtModuleConfigure.pro

make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-QtImaging
cd $DEST/build-QtImaging

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/GUI/qt/QtImaging/QtImaging.pro

make -f Makefile clean
make -f Makefile mocables all
make -f Makefile



if [ -e "$REPOSPATH/GUI/qt/UnitTests" ]
then
    echo "Build tests"

    for f in `ls $REPOSPATH/GUI/qt/UnitTests`
    do
        echo "$REPOSPATHGUI/qt/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/GUI/qt/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile $SPECSTR -o Makefile ../../imagingsuite/GUI/qt/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi

    done

    echo "Tests built"
fi
