#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-muhrecCLI
cd $DEST/build-muhrecCLI

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/CLI/muhrecCLI/muhrecCLI.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

# echo "Build tests"
# if [ -e "$REPOSPATH/applications/muhrec3/UnitTests" ]; then
# for f in `ls $REPOSPATH/applications/muhrec3/UnitTests`
# do
# 	echo "$REPOSPATH/applications/muhrec3/UnitTests/$f/$f.pro"
# 	if [ -e "$REPOSPATH/applications/muhrec3/UnitTests/$f/$f.pro" ]
# 	then 
# 		mkdir -p $DEST/build-$f
# 		cd $DEST/build-$f
# 
#                 $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/applications/muhrec3/UnitTests/$f/$f.pro
#         make -f Makefile clean
#         make -f Makefile mocables all
#         make -f Makefile
# 	fi
# 
# done
# 
# echo "Tests built"
# fi
