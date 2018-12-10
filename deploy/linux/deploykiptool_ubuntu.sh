#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH=$WORKSPACE

DEST="$REPOSPATH/deploy/kiptool"
mkdir --parent $DEST
mkdir --parent $DEST/bin
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
`$CPCMD $REPOSPATH/lib/libBaseModules.so.1.0.0  .`
`$CPCMD $REPOSPATH/lib/libPorespaceModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libClassificationModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libImagingModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libPCAModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libStatisticsModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libImagingModulesGUI.so.1.0.0 .`


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

cd $DEST/bin
`$CPCMD $REPOSPATH/Applications/QtKipTool .`
cd ..
`$CPCMD $REPOSPATH/imagingsuite/applications/kiptool/scripts/kiptool .`
chmod +x kiptool

popd


