DIRECTORY="/Users/kaestner/Applications"
REPOSPATH="/Users/kaestner/repos"

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r $REPOSPATH/kiptool/trunk/QtKipTool/qt/QtKipTool-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/QtKipTool.app $DIRECTORY

pushd .
CPCMD="cp"
DEST="$DIRECTORY/QtKipTool.app"
cd $DEST/Contents
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

`$CPCMD $REPOSPATH/kiptool/trunk/ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/libProcessFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/kiptool/trunk/AdvancedFilterModules/qt/AdvancedFilterModules-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/libAdvancedFilterModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/kiptool/trunk/PCAModules/PCAModules-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/libPCAModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`

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