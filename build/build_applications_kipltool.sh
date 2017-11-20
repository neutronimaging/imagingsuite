#!/bin/bash
QTBINPATH=/Applications/Qt59/5.9.2/clang_64/bin/
REPOSPATH=/Users/kaestner/git/imagingsuite

DEST=/Users/kaestner/git/builds

mkdir -p $DEST/build-muhrec
cd $DEST/build-muhrec

$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/applications/kiptool/qt/QtKipTool/QtKipTool.pro
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

            $QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/applications/kiptool/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi
    done

echo "Tests built"
fi
