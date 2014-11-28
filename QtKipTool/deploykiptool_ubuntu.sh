#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH="/home/`whoami`/repos"

DEST="$REPOSPATH/kiptool/kiptool"
mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/Frameworks

pushd .
cd $DEST/Frameworks
rm -f *.so.1.0.0

`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/ProcessFramework/build-ProcessFramework-Qt5-Release/libProcessFramework.so.1.0.0 .`

# Module libs
`$CPCMD $REPOSPATH/kiptool/trunk/AdvancedFilterModules/qt/build-AdvancedFilterModules-Qt5-Release/libAdvancedFilterModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/BaseModules/qt/build-BaseModules-Qt5-Release/libBaseModules.so.1.0.0  .`
`$CPCMD $REPOSPATH/kiptool/trunk/PorespaceModules/qt/build-PorespaceModules-Qt5-Release/libPorespaceModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/ClassificationModules/qt/build-ClassificationModules-Qt5-Release/libClassificationModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/ImagingModules/qt/build-ImagingModules-Qt5-Release/libImagingModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/PCAModules/qt/build-PCAModules-Qt5-Release/libPCAModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/StatisticsModules/qt/build-StatisticsModules-Qt5-Release/libStatisticsModules.so.1.0.0 .`


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
`$CPCMD $REPOSPATH/kiptool/trunk/QtKipTool/build-QtKipTool-Qt5-Release/QtKipTool .`
cd ..
`$CPCMD $REPOSPATH/kiptool/trunk/QtKipTool/scripts/kiptool .`
chmod +x kiptool

popd


