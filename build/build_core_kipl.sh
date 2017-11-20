#!/bin/bash
QTBINPATH=/Applications/Qt59/5.9.2/clang_64/bin/
REPOSPATH=/Users/kaestner/git/imagingsuite

DEST=/Users/kaestner/git/builds

mkdir -p $DEST/build-kipl
cd $DEST/build-kipl

$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/core/kipl/kipl/qt/kipl.pro
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

		$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/core/kipl/UnitTests/$f/$f.pro
        make -f Makefile clean
        make -f Makefile mocables all
        make -f Makefile
	fi

done

echo "Tests built"
