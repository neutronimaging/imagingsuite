#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-ProcessFramework
cd $DEST/build-ProcessFramework

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/imageprocessing/ProcessFramework/qt/ProcessFramework/ProcessFramework.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

echo "Build modules"

for f in `ls $REPOSPATH/frameworks/imageprocessing/modules`
do
    echo "$REPOSPATH/frameworks/imageprocessing/modules/$f/qt/$f/$f.pro"
    if [ -e "$REPOSPATH/frameworks/imageprocessing/modules/$f/qt/$f/$f.pro" ]
    then
        mkdir -p $DEST/build-$f
        cd $DEST/build-$f

        $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/imageprocessing/modules/$f/qt/$f/$f.pro
        make -f Makefile clean
        make -f Makefile mocables all
        make -f Makefile
fi

done

echo "Modules built"


if [ -e "$REPOSPATH/frameworks/imageprocessing/UnitTests" ]
then
    echo "Build tests"

    for f in `ls $REPOSPATH/frameworks/imageprocessing/UnitTests`
    do
        echo "$REPOSPATHGUI/frameworks/imageprocessing/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/frameworks/imageprocessing/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/imageprocessing/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi

    done

    echo "Tests built"
fi
