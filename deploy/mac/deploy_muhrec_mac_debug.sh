DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/MuhRec.app"
REPOSPATH=$WORKSPACE
QTPATH=$QTBINPATH

GITVER=`git rev-parse --short HEAD`

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

echo $DIRECTORY
echo $QTPATH
echo $DEST
echo $REPOSPATH

cp -r $REPOSPATH/Applications/MuhRec.app $DIRECTORY

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
`$CPCMD $REPOSPATH/lib/libQtImaging.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderGUI.1.0.0.dylib $DEST/Contents/Frameworks`
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
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5.10.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_cpp.11.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_hl.10.dylib $DEST/Contents/Frameworks`
#`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_hl_cpp.100.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libsz.2.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libarmadillo*.dylib $DEST/Contents/Frameworks`

if [ -e "$REPOSPATH/Applications/muhrecCLI" ]; then
	`$CPCMD $REPOSPATH/Applications/muhrecCLI $DEST/Contents/MacOS`
fi



rm -f ./MacOS/*.dylib
cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

if [ -e "/opt/local/lib/libzstd.1.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.1.dylib $DEST/Contents/Frameworks`
fi

if [ -e "/opt/local/lib/libzstd.9.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.9.dylib $DEST/Contents/Frameworks`
fi

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.dylib"
done

cd ..

if [ ! -d "./Resources" ]; then
	mkdir ./Resources	
fi

cp $REPOSPATH/imagingsuite/applications/muhrec/Resources/* ./Resources

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

cd $QTBINPATH
echo "Do deploy..."
./macdeployqt $DEST #-dmg

cd $DEST/Contents/MacOS

install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib MuhRec

# muhrec
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib MuhRec
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib MuhRec
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib MuhRec
install_name_tool -change libQtImaging.1.dylib @executable_path/../Frameworks/libQtImaging.1.dylib MuhRec
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib MuhRec
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib MuhRec
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib MuhRec
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib MuhRec
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib MuhRec
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib MuhRec
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib MuhRec
install_name_tool -change libReaderGUI.1.dylib @executable_path/../Frameworks/libReaderGUI.1.dylib MuhRec

# muhrecCLI
if [ -e "muhrecCLI" ]; then
	install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib muhrecCLI
	install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib muhrecCLI
	install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib muhrecCLI
	install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib muhrecCLI
	install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib muhrecCLI
	install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib muhrecCLI
fi

cd ../Frameworks

# ModuleConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib

# ReconFramework
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReconFramework.1.0.0.dylib


# QtAddons
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# ReaderConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReaderConfig.1.0.0.dylib

# ReaderConfigGUI
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderGUI.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReaderGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderGUI.1.0.0.dylib

# QtImaging
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libQtImaging.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libQtImaging.1.0.0.dylib


# QtModuleConfigure
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libQtModuleConfigure.1.0.0.dylib

# ImagingAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

#nexus_related
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReconFramework.1.0.0.dylib
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

#FDKBackProjector
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libFDKBackProjectors.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libFDKBackProjectors.1.0.0.dylib

# Inspectors
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libInspectorModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib  @executable_path/../Frameworks/libModuleConfig.1.dylib  libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib  @executable_path/../Frameworks/libModuleConfig.1.dylib  libInspectorModules.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib  @executable_path/../Frameworks/libQtModuleConfigure.1.dylib  libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib  @executable_path/../Frameworks/libQtModuleConfigure.1.dylib  libInspectorModules.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib  @executable_path/../Frameworks/libQtAddons.1.dylib  libInspectorModulesGUI.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModulesGUI.1.0.0.dylib

#ReconAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconAlgorithms.1.0.0.dylib

# stdbackprojectors
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdBackprojectors.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdBackprojectors.1.0.0.dylib
# Preprocessing
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdPreprocModules.1.0.0.dylib
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libStdPreprocModules.1.dylib @executable_path/../Frameworks/libStdPreprocModules.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdPreprocModulesGUI.1.0.0.dylib

rm -rf /tmp/muhrec

if [ ! -d "/tmp/muhrec" ]; then
  mkdir /tmp/muhrec
fi

if [ ! -e "tmp/muhrec/Applications" ]; then
	ln -s /Applications /tmp/muhrec
fi

cp -r $DEST /tmp/muhrec

hdiutil create -volname MuhRec -srcfolder /tmp/muhrec -ov -format UDZO $DIRECTORY/MuhRec-MacOS_build-$GITVER-`date +%Y%m%d`.dmg
