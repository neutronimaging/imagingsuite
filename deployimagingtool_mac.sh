DIRECTORY=~/Applications
REPOSPATH=~/repos
QTPATH=$QT_HOME
LIBDIR=~/repos/lib

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi



cp -r ~/repos/Applications/imagingtool.app $DIRECTORY

pushd .
CPCMD="cp"
DEST=$DIRECTORY/imagingtool.app
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

`$CPCMD $LIBDIR/libProcessFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libReaderConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libReaderGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $LIBDIR/libReconFramework.1.0.0.dylib $DEST/Contents/Frameworks`

rm -f ./MacOS/*.dylib

for f in `ls ./Frameworks/*.1.0.0.dylib`; do
	ln -s ../$f "./MacOS/`basename $f .1.0.0.dylib`.1.dylib"
done

pwd

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

cd $DEST/Contents/MacOS

install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib imagingtool
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib imagingtool
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib imagingtool
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib imagingtool
install_name_tool -change libReaderGUI.1.dylib @executable_path/../Frameworks/libReaderGUI.1.dylib imagingtool
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib imagingtool
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib imagingtool
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib imagingtool

cd ../Frameworks
# ModuleConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib

# QtAddons
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# QtModuleConfigure
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib

# ReconFramework
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReconFramework.1.0.0.dylib
 
# ImagingAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# Readers
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReaderGUI.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderGUI.1.0.0.dylib

