#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-muhrec
cd $DEST/build-muhrec

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/imageviewer/imageviewer/imageviewer.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile



if [ -e "$REPOSPATH/applications/imageviewer/UnitTests" ]
echo "Build tests"
then
    for f in `ls $REPOSPATH/applications/imageviewer/UnitTests`
    do
        echo "$REPOSPATH/applications/imageviewer/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/applications/imageviewer/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/imageviewer/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi
    done

echo "Tests built"
fi
