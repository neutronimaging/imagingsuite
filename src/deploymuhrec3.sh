DIRECTORY="/Users/kaestner/Applications"
#QTPATH="/Applications/Qt531/5.3/clang_64"
QTPATH="/Applications/Qt541/5.4/clang_64/"
DEST="$DIRECTORY/muhrec3.app"
REPOSPATH="/Users/kaestner/repos"

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r ~/repos/tomography/trunk/src/build-muhrec3-Qt5-Release/muhrec3.app $DIRECTORY

pushd .
CPCMD="cp"

cd $DEST/Contents
echo "This is where it lands..."
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtAddons-Qt5-Release/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/libReconFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/StdBackProjectors/build-StdBackProjectors-Qt5-Release/libStdBackProjectors.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/StdPreprocModules/build-StdPreprocModules-Qt5-Release/libStdPreprocModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/kipl/trunk/kipl/build-kipl-Qt5-Release/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-StdPreprocModulesGUI-Qt5-Release/libStdPreprocModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`

if [ ! -d "./Resources" ]; then
 mkdir ./Resources
fi 
`$CPCMD $REPOSPATH/tomography/trunk/src/muhrec3.icns $DEST/Contents/Resources`

rm -f ./MacOS/*.dylib

#for f in `ls ./Frameworks/*.1.0.0.dylib`; do
#	ln -s ../$f "./MacOS/`basename $f .1.0.0.dylib`.1.dylib"
#done

cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.dylib"
done
cd ..

if [ ! -d "./Resources" ]; then
	mkdir ./Resources	
fi
cp /Users/kaestner/repos/tomography/trunk/src/muhrec3/resources/* ./Resources

sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist
echo "copy plugins"
pwd
if [ ! -d "./PlugIns" ]; then
 mkdir ./PlugIns
fi

if [ ! -d "./PlugIns/platforms" ]; then
 mkdir ./PlugIns/platforms
fi
cp $QTPATH/plugins/platforms/libqcocoa.dylib $DEST/Contents/PlugIns/platforms/

if [ ! -d "./PlugIns/printsupport" ]; then
 mkdir ./PlugIns/printsupport
fi
cp $QTPATH/plugins/printsupport/libcocoaprintersupport.dylib $DEST/Contents/PlugIns/printsupport/

if [ ! -d "./PlugIns/accessible" ]; then
 mkdir ./PlugIns/accessible
fi
cp $QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib $DEST/Contents/PlugIns/accessible/
pwd
ls PlugIns

cd $QTPATH/bin/
echo "Do deploy..."
./macdeployqt $DEST #-dmg
popd 