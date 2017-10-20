CPCMD="ln -s "
%CPCMD="cp "
DEST="build-muhrec3-Qt5-Release/muhrec3.app/Contents/MacOS"
pushd .
cd $DEST
rm -f *.1.0.0.dylib

`$CPCMD /Users/kaestner/repos/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/libImagingAlgorithms.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/tomography/trunk/src/build-ReconFramework-Qt5-Release/libReconFramework.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/tomography/trunk/src/build-StdBackProjectors-Qt5-Release/libStdBackProjectors.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/tomography/trunk/src/build-StdPreprocModules-Qt5-Release/libStdPreprocModules.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/tomography/trunk/src/build-StdPreprocModulesGUI-Qt5-Release/libStdPreprocModulesGUI.1.0.0.dylib .`

rm -f *.1.dylib
rm -f *.1.0.dylib

for f in `ls *.1.0.0.dylib`; do
	bn=`basename $f .1.0.0.dylib`
	echo $bn
	ln -s $f $bn.1.0.dylib
    ln -s $bn.1.0.dylib $bn.1.dylib
done

`$CPCMD /Users/kaestner/repos/tomography/trunk/src/muhrec3/resources/* $DEST/../Resources`

popd