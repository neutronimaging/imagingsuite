#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH="/home/`whoami`/repos"

DEST="$REPOSPATH/tomography/muhrec3"
mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/Frameworks

pushd .
cd $DEST/Frameworks
rm -f *.1.0.0.dylib

`$CPCMD $REPOSPATH/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/ReconFramework/ReconFramework-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libReconFramework.so.1.0.0 .`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/StdBackProjectors/StdBackProjectors-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdBackProjectors.so.1.0.0 .`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/StdPreprocModules/StdPreprocModules-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModules.so.1.0.0 .`

`$CPCMD $REPOSPATH/tomography/trunk/src/StdPreprocModulesGUI-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModulesGUI.so.1.0.0 .`

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
`$CPCMD $REPOSPATH/tomography/trunk/src/muhrec3-build-Qt_4_8_1_in_PATH__System__Release/muhrec3 .`
cd ..
`$CPCMD $REPOSPATH/tomography/trunk/src/scripts/muhrec3 .`
chmod +x muhrec3
popd


