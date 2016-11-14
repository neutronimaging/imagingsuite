DIRECTORY="/Users/kaestner/Applications"
REPOSPATH="/Users/kaestner/repos"
#QTPATH="/Applications/Qt54/5.4/clang_64/"
QTPATH="/Applications/Qt56/5.6/clang_64/"

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cp -r $REPOSPATH/Applications/QtKipTool.app $DIRECTORY

pushd .
CPCMD="cp"
DEST="$DIRECTORY/QtKipTool.app"
cd $DEST/Contents
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

`$CPCMD $REPOSPATH/lib/libProcessFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libAdvancedFilterModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libStatisticsModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libPorespaceModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libPCAModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib//libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`

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
#cp /Users/kaestner/repos/tomography/trunk/src/muhrec3/resources/* ./Resources

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

popd
sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist

$QTPATH/bin/macdeployqt $DEST

# cd $DEST/Contents/MacOS
# # QtnGITool
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib QtKipTool
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib QtKipTool
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib QtKipTool
# install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib QtKipTool
# install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib QtKipTool
# 
# cd ../Frameworks
# 
# # ModuleConfig
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib
# 
# # QtAddons
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib
# 
# # QtModuleConfigure
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib
# 
# # ProcessFramework
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libProcessFramework.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libProcessFramework.1.0.0.dylib
# 
# # ImagingAlgorithms
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib