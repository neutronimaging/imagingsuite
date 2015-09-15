#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH="/home/`whoami`/repos"

DEST="$REPOSPATH/tomography/muhrec3"
mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/Frameworks
mkdir --parent $DEST/resources

pushd .
cd $DEST/Frameworks
rm -f *.1.0.0.dylib

`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-ReconFramework-Qt5-Release/libReconFramework.so.1.0.0 .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-StdBackProjectors-Qt5-Release/libStdBackProjectors.so.1.0.0 .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-StdPreprocModules-Qt5-Release/libStdPreprocModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-StdPreprocModulesGUI-Qt5-Release/libStdPreprocModulesGUI.so.1.0.0 .`

`$CPCMD $REPOSPATH/tomography/trunk/src/build-InspectorModules-Qt5-Release/libInspectorModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-InspectorModulesGUI-Qt5-Release/libInspectorModulesGUI.so.1.0.0 .`

`$CPCMD $REPOSPATH/tomography/trunk/src/build-IterativeBackProj-Qt5-Release .`

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
`$CPCMD $REPOSPATH/tomography/trunk/build-muhrec3-Qt5-Release/muhrec3 .`
cd ..
`$CPCMD $REPOSPATH/tomography/trunk/src/scripts/muhrec3 .`
chmod +x muhrec3

cp $REPOSPATH/tomography/trunk/src/muhrec3/resources/*.xml resources
popd


