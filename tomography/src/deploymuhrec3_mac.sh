DIRECTORY=~/Applications
#QTPATH="/Applications/Qt531/5.3/clang_64"
#QTPATH="/Applications/Qt541/5.4/clang_64"
QTPATH=$QT_HOME
#QTPATH=~/Qt5.7.0/5.7/clang_64
DEST="$DIRECTORY/muhrec3.app"
REPOSPATH=~/repos

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r ~/repos/Applications/muhrec3.app $DIRECTORY

pushd .
CPCMD="cp"

cd $DEST/Contents
echo "This is where it lands..."
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReconFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReconAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libStdBackProjectors.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libStdPreprocModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libStdPreprocModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libInspectorModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libInspectorModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libFDKBackProjectors.1.0.0.dylib $DEST/Contents/Frameworks`

# if [ ! -d "./Resources" ]; then
#  mkdir ./Resources
# fi 
# `$CPCMD $REPOSPATH/tomography/trunk/src/muhrec3/resources/muh_icon.icns $DEST/Contents/Resources`

rm -f ./MacOS/*.dylib

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
cp ~/repos/tomography/trunk/src/muhrec3/resources/* ./Resources

sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist
echo "copy plugins"
pwd
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

pwd
ls PlugIns

cd $QTPATH/bin/
echo "Do deploy..."
./macdeployqt $DEST #-dmg

cd $DEST/Contents/MacOS
# muhrec3
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib muhrec3
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib muhrec3
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib muhrec3
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib muhrec3
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib muhrec3
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib muhrec3
cd ../Frameworks

# ModuleConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib

# QtAddons
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# QtModuleConfigure
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib

# ReconFramework
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReconFramework.1.0.0.dylib

# ImagingAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# Preprocessing
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModules.1.0.0.dylib

install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libStdPreprocModules.1.dylib @executable_path/../Frameworks/libStdPreprocModules.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libStdPreprocModulesGUI.1.0.0.dylib

# stdbackprojectors
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdBackprojectors.1.0.0.dylib

# FDK
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libFDKBackProjectors.1.0.0.dylib
#install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libFDKBackProjectors.1.0.0.dylib

# Inspectors
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModules.1.0.0.dylib
#install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libInspectorModules.1.0.0.dylib

install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModulesGUI.1.0.0.dylib
#install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libInspectorModulesGUI.1.0.0.dylib
