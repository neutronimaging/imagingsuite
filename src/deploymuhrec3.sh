DIRECTORY="/Users/kaestner/Applications"

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r ~/repos/tomography/trunk/src/muhrec3-build-desktop-Qt_4_8_1__gcc__Release/muhrec3.app $DIRECTORY

pushd .
CPCMD="cp"
DEST="$DIRECTORY/muhrec3.app"
cd $DEST/Contents
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi


`$CPCMD /Users/kaestner/repos/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/src/libs/recon2/trunk/ReconFramework/ReconFramework-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libReconFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/src/libs/recon2/trunk/StdBackProjectors/StdBackProjectors-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdBackProjectors.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/src/libs/recon2/trunk/StdPreprocModules/StdPreprocModules-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /Users/kaestner/repos/tomography/trunk/src/StdPreprocModulesGUI-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/libStdPreprocModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`

rm -f ./MacOS/*.dylib

for f in `ls ./Frameworks/*.1.0.0.dylib`; do
	ln -s ../$f "./MacOS/`basename $f .1.0.0.dylib`.1.dylib"
done

cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
done
cd ..

if [ ! -d "./Resources" ]; then
	mkdir ./Resources	
fi
cp /Users/kaestner/repos/tomography/trunk/src/muhrec3/resources/* ./Resources

popd
sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist


/Users/kaestner/QtSDK/Desktop/Qt/4.8.1/gcc/bin/macdeployqt $DEST