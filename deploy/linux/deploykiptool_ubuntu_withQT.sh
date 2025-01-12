#!/bin/bash
#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH=$WORKSPACE
QT_PATH=$QTPATH
#declare -i QtV=${QT_PATH:end-11:end-8}
QtV=${QT_PATH:end-11:end-7}
QtVmain=${QT_PATH:end-11:end-9}


echo $QtV
echo $QtVmain
echo $QT_PATH


DEST="$REPOSPATH/deploy/kiptool"
#DEST="$HOME/kiptool"

mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/bin/platforms
mkdir --parent $DEST/Frameworks

pushd .
cd $DEST/Frameworks
rm -f *.so.1.0.0

`$CPCMD $REPOSPATH/lib/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libProcessFramework.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libReaderConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReaderGUI.so.1.0.0 .`

# Module libs
`$CPCMD $REPOSPATH/lib/libAdvancedFilterModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libAdvancedFilterModulesGUI.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libBaseModules.so.1.0.0  .`
`$CPCMD $REPOSPATH/lib/libBaseModulesGUI.so.1.0.0  .`
#`$CPCMD $REPOSPATH/lib/libPorespaceModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libClassificationModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libClassificationModulesGUI.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libImagingModules.so.1.0.0 .`
#`$CPCMD $REPOSPATH/lib/libPCAModules.so.1.0.0 .`
#`$CPCMD $REPOSPATH/lib/libStatisticsModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libImagingModulesGUI.so.1.0.0 .`

#qt stuff
`$CPCMD $QT_PATH/lib/libQt5Core.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5Gui.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5Widgets.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5DBus.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5PrintSupport.so.5.$QtV .`
`$CPCMD $QT_PATH/lib/libQt5XcbQpa.so.5.$QtV .`
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
`$CPCMD $REPOSPATH/Applications/KipTool .`
cd ..
`$CPCMD $REPOSPATH/imagingsuite/applications/kiptool/scripts/kiptool .`
chmod +x kiptool

fname=kiptool_`uname -s`_`uname -m`_`date +%Y%m%d`.tar.bz2

echo $fname

cd $DEST
cd ..
tar -jcvhf $fname 'kiptool'


popd


