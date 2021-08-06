#!/bin/bash
CPCMD="cp "
REPOSPATH=$WORKSPACE

QT_PATH="$QTPATH"
QtV=${QT_PATH:end-11:end-7}
QtVmain=${QT_PATH:end-11:end-9}

#declare -i QtV=${QT_PATH:22:2}

echo $QtV
echo $QtVmain
echo $QT_PATH


#DEST="$HOME/muhrec"
DEST="$REPOSPATH/deploy/muhrec"

mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/bin/platforms
mkdir --parent $DEST/Frameworks
mkdir --parent $DEST/resources


pushd .
cd $DEST/Frameworks
rm -f *.1.0.0.dylib

`$CPCMD $REPOSPATH/lib/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReaderConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReaderGUI.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReconFramework.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReconAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libStdBackProjectors.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libStdPreprocModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libStdPreprocModulesGUI.so.1.0.0 .`

#`$CPCMD $REPOSPATH/lib/libUnpublPreProcessing.so.1.0.0 .`
#`$CPCMD $REPOSPATH/lib/libUnpublPreProcessingGUI.so.1.0.0 .`
#`$CPCMD $REPOSPATH/lib/libUnpublishedImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libInspectorModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libInspectorModulesGUI.so.1.0.0 .`


`$CPCMD $REPOSPATH/lib/libFDKBackProjectors.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libQtImaging.so.1.0.0 .`

`$CPCMD $QT_PATH/lib/libQt5Core.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5Gui.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5Widgets.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5DBus.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5PrintSupport.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5XcbQpa.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5DBus.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5Charts.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libicui18n.so.56.1 .`
`$CPCMD $QT_PATH/lib/libicudata.so.56.1 .`
`$CPCMD $QT_PATH/lib/libicuuc.so.56.1 .`


rm -f *.so
rm -f *.so.1
rm -f *.so.1.0

for f in `ls *.so.1.0.0`; do
	bn=`basename $f .so.1.0.0`
	echo $bn
	ln -s $f $bn.so.1.0
        ln -s $bn.so.1.0 $bn.so.1
	ln -s $bn.so.1 $bn.so
done


rm -f *.so.5.$QtVmain
rm -f *.so.5
for f in `ls *.so.5.$QtV`; do
	bn=`basename $f .so.5.$QtV`
	echo $bn
	ln -s $f $bn.so.5.$QtVmain
        ln -s $bn.so.5.$QtVmain $bn.so.5
	ln -s $bn.so.5 $bn.so
done

rm -f *.so.56
for f in `ls *.so.56.1`; do
	bn=`basename $f .so.56.1`
	echo $bn
	ln -s $bn.so.56.1 $bn.so.56
	ln -s $bn.so.56 $bn.so
done


cd $DEST/bin/platforms
`$CPCMD $QT_PATH/plugins/platforms/libqxcb.so .`



cd $DEST/bin
`$CPCMD $REPOSPATH/Applications/muhrec .`
cd ..
`$CPCMD $REPOSPATH/imagingsuite/applications/muhrec/scripts/muhrec .`
chmod +x muhrec


cp $REPOSPATH/imagingsuite/applications/muhrec/Resources/*.xml resources
GITVER=`git rev-parse --short HEAD`

fname=MuhRec-Ubuntu_`uname -s`_`uname -m`_build-$GITVER-`date +%Y%m%d`.tar.bz2

echo $fname

cd $DEST
cd ..
tar -jcvhf $fname 'muhrec'

popd


