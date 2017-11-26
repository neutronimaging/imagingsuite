#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-kipl
cd $DEST/build-kipl

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/core/kipl/kipl/qt/kipl.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

echo "Build tests"

for f in `ls $REPOSPATH/core/kipl/UnitTests` 
do
	echo "$REPOSPATH/core/kipl/UnitTests/$f/$f.pro"
	if [ -e "$REPOSPATH/core/kipl/UnitTests/$f/$f.pro" ]
	then 
		mkdir -p $DEST/build-$f
		cd $DEST/build-$f

                $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/core/kipl/UnitTests/$f/$f.pro
        make -f Makefile clean
        make -f Makefile mocables all
        make -f Makefile
	fi

done

echo "Tests built"
