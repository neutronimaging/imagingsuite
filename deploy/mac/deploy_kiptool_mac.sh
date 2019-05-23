DIRECTORY=$WORKSPACE/deployed

REPOSPATH=$WORKSPACE
QTPATH=$QTBINPATH/..
DEST="$DIRECTORY/KipTool.app"

GITVER=`git rev-parse --short HEAD`
echo $DIRECTORY
echo $QTPATH
echo $DEST
echo $REPOSPATH

echo "start"
if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

echo "Copy app"
cp -r $REPOSPATH/Applications/KipTool.app $DIRECTORY

pushd .
CPCMD="cp"

cd $DEST/Contents
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

`$CPCMD $REPOSPATH/lib/libProcessFramework.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libAdvancedFilterModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libAdvancedFilterModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libBaseModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libBaseModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libClassificationModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libClassificationModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libStatisticsModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libPorespaceModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libPCAModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingModules.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingModulesGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderGUI.1.0.0.dylib $DEST/Contents/Frameworks`

`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5.10.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_cpp.11.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_hl.10.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libsz.2.dylib $DEST/Contents/Frameworks`

rm -f ./MacOS/*.dylib

for f in `ls ./Frameworks/*.1.0.0.dylib`; do
	ln -s ../$f "./MacOS/`basename $f .1.0.0.dylib`.1.dylib"
done

cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

if [ -e "/opt/local/lib/libzstd.1.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.1.dylib $DEST/Contents/Frameworks`
fi

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
done

cd ..

if [ ! -d "./Resources" ]; then
	mkdir ./Resources	
fi
cp $REPOSPATH/imagingsuite/applications/kiptool/resources/* ./Resources

echo "copy plugins"
pwd
if [ ! -d "./PlugIns" ]; then
 mkdir ./PlugIns
fi

if [ ! -d "./PlugIns/platforms" ]; then
 mkdir ./PlugIns/platforms
fi

if [ ! -f "./PlugIns/platforms/libqcocoa.dylib" ]; then 
	if [ -f "$QTPATH/plugins/platforms/libqcocoa.dylib" ]; then 
		cp $QTPATH/plugins/platforms/libqcocoa.dylib $DEST/Contents/PlugIns/platforms/
	fi
fi

if [ ! -d "./PlugIns/printsupport" ]; then
	mkdir ./PlugIns/printsupport
fi

if [ ! -f "./PlugIns/printsupport/libcocoaprintersupport.dylib" ]; then 
	if [ -f "$QTPATH/plugins/printsupport/libcocoaprintersupport.dylib" ]; then
		cp $QTPATH/plugins/printsupport/libcocoaprintersupport.dylib $DEST/Contents/PlugIns/printsupport/
	fi
fi

if [ ! -d "./PlugIns/accessible" ]; then
 mkdir ./PlugIns/accessible
fi

if [ ! -f "./PlugIns/accessible/libqtaccessiblewidgets.dylib" ]; then 
	if [ -f "$QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib"] ; then
		cp $QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib $DEST/Contents/PlugIns/accessible/
	fi
fi

pwd
ls PlugIns

popd
sed -i.bak s+com.yourcompany+ch.imagingscience+g $DEST/Contents/Info.plist

cd $QTBINPATH
echo "Do deploy ..."
pwd
./macdeployqt $DEST

cd $DEST/Contents/MacOS

install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib KipTool
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib KipTool
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib KipTool
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib KipTool
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib KipTool
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib KipTool
install_name_tool -change libReaderGUI.1.dylib @executable_path/../Frameworks/libReaderGUI.1.dylib KipTool
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib KipTool
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib KipTool
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib KipTool

cd ../Frameworks
# ModuleConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib

# QtAddons
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# QtModuleConfigure
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libQtModuleConfigure.1.0.0.dylib

# ProcessFramework
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libProcessFramework.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libProcessFramework.1.0.0.dylib
 
# ImagingAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# AdvancedFilterModules
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libAdvancedFilterModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libAdvancedFilterModules.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libAdvancedFilterModules.1.0.0.dylib

# AdvancedFilterModulesGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib

# ReaderConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib

# ReaderConfigGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderGUI.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReaderGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderGUI.1.0.0.dylib

#nexus_related
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libProcessFramework.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libProcessFramework.1.0.0.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexus.1.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexusCPP.1.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libNexusCPP.1.dylib

install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libhdf5_hl.10.dylib

install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @executable_path/../Frameworks/libhdf5_cpp.11.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @executable_path/../Frameworks/libhdf5_hl.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libNeXus.1.0.0.dylib

install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @executable_path/../Frameworks/libhdf5_cpp.11.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @executable_path/../Frameworks/libhdf5_hl.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libNeXusCPP.1.0.0.dylib

install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5.10.dylib libhdf5.10.dylib libhdf5.10.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5.10.dylib

install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5_cpp.11.dylib

install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib  libhdf5_hl.10.dylib libhdf5_hl.10.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5_hl.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5_hl.10.dylib

# and more
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib

# ImagingModules and ImagingModulesGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libImagingModules.1.dylib @executable_path/../Frameworks/libImagingModules.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libImagingModulesGUI.1.0.0.dylib

# BaseModules and BasemodulesGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libBaseModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libBaseModules.1.0.0.dylib
#install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libImagingModules.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libBaseModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libBaseModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libBaseModules.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libBaseModules.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libBaseModulesGUI.1.0.0.dylib
#install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libImagingModulesGUI.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libBaseModules.1.dylib @executable_path/../Frameworks/libImagingModules.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libBaseModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libBaseModulesGUI.1.0.0.dylib

# ClassificationModules and ClassificationModulesGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libClassificationModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libClassificationModules.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libClassificationModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libClassificationModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libClassificationModules.1.0.0.dylib
#install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libClassificationModules.1.0.0.dylib
#install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libProcessFramework.1.dylib @executable_path/../Frameworks/libProcessFramework.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libClassificationModules.1.dylib @executable_path/../Frameworks/libClassificationModules.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libClassificationModulesGUI.1.0.0.dylib
install_name_tool -change libAdvancedFilterModules.1.dylib @executable_path/../Frameworks/libAdvancedFilterModules.1.dylib libClassificationModulesGUI.1.0.0.dylib

# AdvancedFilterModule and AdvancedFilterModuleGUI
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libAdvancedFilterModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libAdvancedFilterModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib
install_name_tool -change libAdvancedFilterModules.1.dylib @executable_path/../Frameworks/libAdvancedFilterModules.1.dylib libAdvancedFilterModulesGUI.1.0.0.dylib

rm -rf /tmp/kiptool
if [ ! -d "/tmp/kiptool" ]; then
  mkdir /tmp/kiptool
fi
if [ ! -e "tmp/kiptool/Applications" ]; then
	ln -s /Applications /tmp/kiptool
fi
cp -r $DEST /tmp/kiptool

hdiutil create -volname KipTool -srcfolder /tmp/kiptool -ov -format UDZO $DIRECTORY/KipTool_build-$GITVER-`date +%Y%m%d`.dmg


