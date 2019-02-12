#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-kiptool
cd $DEST/build-kiptool

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/kiptool/qt/QtKipTool/QtKipTool.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

echo "Build tests"

if [ -e "$REPOSPATH/applications/kiptool/UnitTests" ]
then
    for f in `ls $REPOSPATH/applications/kiptool/UnitTests`
    do
        echo "$REPOSPATH/applications/kiptool/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/applications/kiptool/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/kiptool/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi
    done

echo "Tests built"
fi
