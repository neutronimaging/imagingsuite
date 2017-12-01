#!/bin/bash
if [ `uname` == 'Linux' ]; then
    SPECSTR="-spec linux-g++"
else
    SPECSTR="-spec macx-clang CONFIG+=x86_64"
fi

REPOSPATH=$WORKSPACE/imagingsuite

DEST=$WORKSPACE/builds

mkdir -p $DEST/build-ReconFramework
cd $DEST/build-ReconFramework

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Framework/ReconFramework/qt/ReconFramework/ReconFramework.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-ReconAlgorithms
cd $DEST/build-ReconAlgorithms

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Framework/ReconAlgorithms/ReconAlgorithms/ReconAlgorithms.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-StdBackProjectors
cd $DEST/build-StdBackProjectors

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Backprojectors/StdBackProjectors/qt/StdBackProjectors/StdBackProjectors.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-FDKBackProjectors
cd $DEST/build-FDKBackProjectors

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Backprojectors/FDKBackProjectors/qt/FDKBackProjectors/FDKBackProjectors.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-GenericBackProj
cd $DEST/build-GenericBackProj

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Backprojectors/GenericBackProj/qt/GenericBackProj/GenericBackProj.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-IterativeBackProj
cd $DEST/build-IterativeBackProj

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Backprojectors/IterativeBackProj/qt/IterativeBackProj/IterativeBackProj.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

#mkdir -p $DEST/build-DummyModules
#cd $DEST/build-DummyModules
#
#$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/DummyModules
#make -f Makefile clean
#make -f Makefile mocables all
#make -f Makefile

#mkdir -p $DEST/build-MoreDummies
#cd $DEST/build-MoreDummies
#
#$QTBINPATH/qmake -makefile -r -spec macx-clang CONFIG+=x86_64 -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/MoreDummies
#make -f Makefile clean
#make -f Makefile mocables all
#make -f Makefile

mkdir -p $DEST/build-InspectorModules
cd $DEST/build-InspectorModules

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/InspectorModules/InspectorModules.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-InspectorModulesGUI
cd $DEST/build-InspectorModulesGUI

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/InspectorModulesGUI/InspectorModulesGUI.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-StdPreprocModules
cd $DEST/build-StdPreprocModules

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/StdPreprocModules/qt/StdPreprocModules/StdPreprocModules.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-StdPreprocModulesGUI
cd $DEST/build-StdPreprocModulesGUI

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/StdPreprocModulesGUI/qt/StdPreprocModulesGUI/StdPreprocModulesGUI.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile

mkdir -p $DEST/build-genericpreproc
cd $DEST/build-genericpreproc

$QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/Preprocessing/GenericPreProc/GenericPreProc.pro
make -f Makefile clean
make -f Makefile mocables all
make -f Makefile


if [ -e "$REPOSPATH/frameworks/tomography/UnitTests" ]
then
    echo "Build tests"

    for f in `ls $REPOSPATH/frameworks/tomography/UnitTests`
    do
        echo "$REPOSPATH/frameworks/tomography/UnitTests/$f/$f.pro"
        if [ -e "$REPOSPATH/frameworks/tomography/UnitTests/$f/$f.pro" ]
        then
            mkdir -p $DEST/build-$f
            cd $DEST/build-$f

            $QTBINPATH/qmake -makefile $SPECSTR -o Makefile ../../imagingsuite/frameworks/tomography/UnitTests/$f/$f.pro
            make -f Makefile clean
            make -f Makefile mocables all
            make -f Makefile
        fi

    done

    echo "Tests built"
fi
