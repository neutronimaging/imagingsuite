DIRECTORY="/Users/kaestner/Applications"
REPOSPATH="/Users/kaestner/repos"
QTPATH="/Applications/Qt541/5.4/clang_64/"

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r ~/repos/imagingtool/trunk/build-imagingtool-Qt5-Release/imagingtool.app $DIRECTORY

pushd .
CPCMD="cp"
DEST="$DIRECTORY/imagingtool.app"
cd $DEST/Contents
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

if [ ! -d "./PlugIns" ]; then
 mkdir ./PlugIns
fi

if [ ! -d "./PlugIns/platforms" ]; then
 mkdir ./PlugIns/platforms
fi

if [ ! -f "./PlugIns/platforms/libqcocoa.dylib" ]; then 
	cp $QTPATH/plugins/platforms/libqcocoa.dylib $DEST/Contents/PlugIns/platforms/
fi

if [ ! -d "./PlugIns/printsupport" ]; then
 mkdir ./PlugIns/printsupport
fi

if [ ! -f "./PlugIns/printsupport/libcocoaprintersupport.dylib" ]; then 
	cp $QTPATH/plugins/printsupport/libcocoaprintersupport.dylib $DEST/Contents/PlugIns/printsupport/
fi

if [ ! -d "./PlugIns/accessible" ]; then
 mkdir ./PlugIns/accessible
fi

if [ ! -f "./PlugIns/accessible/libqtaccessiblewidgets.dylib" ]; then 
	cp $QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib $DEST/Contents/PlugIns/accessible/
fi

`$CPCMD $REPOSPATH/kiptool/trunk/ProcessFramework/build-ProcessFramework-Qt5-Release/libProcessFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/modules/trunk/build-ReaderConfig-Qt5-Release/libReaderConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/modules/trunk/build-ReaderGUI-Qt5-Release/libReaderGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-ReconFramework-Qt5-Release/libReconFramework.1.0.0.dylib $DEST/Contents/Frameworks`

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
# cp /Users/kaestner/repos/imagingtool/trunk//* ./Resources

popd
sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist



$QTPATH/bin/macdeployqt $DEST
