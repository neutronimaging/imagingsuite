CPCMD="ln -s "
#CPCMD="cp "
DEST="build-WidgetTests-Qt5-Release/WidgetTests.app/Contents/MacOS"
pushd .
cd $DEST
rm -f *.1.0.0.dylib

#`$CPCMD /Users/kaestner/repos/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/modules/trunk/build-ReaderConfig-Qt5-Release/libReaderConfig.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/modules/trunk/build-ReaderGUI-Qt5-Release/libReaderGUI.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.1.0.0.dylib .`
`$CPCMD /Users/kaestner/repos/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.1.0.0.dylib .`
# `$CPCMD /Users/kaestner/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim/usr/lib/libgomp.so.1.0.0 .`

rm -f *.1.dylib
rm -f *.1.0.dylib

for f in `ls *.1.0.0.dylib`; do
	bn=`basename $f .1.0.0.dylib`
	echo $bn
	ln -s $f $bn.1.0.dylib
    ln -s $bn.1.0.dylib $bn.1.dylib
done

#`$CPCMD /Users/kaestner/repos/tomography/trunk/src/muhrec3/resources/* $DEST/../Resources`

popd