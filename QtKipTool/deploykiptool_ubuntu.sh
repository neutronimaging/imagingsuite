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

`$CPCMD $REPOSPATH/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/libProcessFramework.so.1.0.0 .`

# Module libs
`$CPCMD $REPOSPATH/kiptool/trunk/AdvancedFilterModules/qt/AdvancedFilterModules-build-desktop-Qt_4_8_1_in_PATH__System__Release/libAdvancedFilterModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/BaseModules/qt/BaseModules-build-desktop-Qt_4_8_1_in_PATH__System__Release/libBaseModules.so.1.0.0  .`
`$CPCMD $REPOSPATH/kiptool/trunk/PorespaceModules/qt/PorespaceModules-build-desktop-Qt_4_8_1_in_PATH__System__Release/libPorespaceModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/kiptool/trunk/ClassificationModules/qt/ClassificationModules-build-desktop-Qt_4_8_1_in_PATH__System__Release/libClassificationModules.so.1.0.0 .`



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
`$CPCMD $REPOSPATH/kiptool/trunk/QtKipTool/qt/QtKipTool-build-desktop-Qt_4_8_1_in_PATH__System__Release/QtKipTool .`
cd ..
`$CPCMD $REPOSPATH/kiptool/trunk/QtKipTool/scripts/kiptool .`
chmod +x kiptool

popd


