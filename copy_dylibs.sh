#CPCMD="ln -s "
CPCMD="cp "
DEST="imagingtool-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/imagingtool.app/Contents/MacOS"
pushd .
cd $DEST
rm -f *.1.0.0.dylib

#`$CPCMD /Users/kaestner/repos/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libModuleConfig.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.1.0.0.dylib .`
#`$CPCMD /Users/kaestner/repos/gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libReconFramework.1.0.0.dylib .`
#`$CPCMD /Users/kaestner/repos/src/src/libs/recon2/trunk/StdBackProjectors/qt/StdBackProjectors-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdBackProjectors.1.0.0.dylib .`
#`$CPCMD /Users/kaestner/repos/src/src/libs/recon2/trunk/StdPreprocModules/qt/StdPreprocModules-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModules.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libkipl.1.0.0.dylib .`
# `$CPCMD /Users/kaestner/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim/usr/lib/libgomp.so.1.0.0 .`
#`$CPCMD /Users/kaestner/repos/tomography/trunk/src/StdPreprocModulesGUI-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModulesGUI.1.0.0.dylib .`

rm -f *.1.dylib
rm -f *.1.0.dylib

for f in `ls *.1.0.0.dylib`; do
	bn=`basename $f .1.0.0.dylib`
	echo $bn
	ln -s $f $bn.1.0.dylib
    ln -s $bn.1.0.dylib $bn.1.dylib
done

popd