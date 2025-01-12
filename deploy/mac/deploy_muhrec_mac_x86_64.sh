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

cp -r $REPOSPATH/install/applications/MuhRec.app $DIRECTORY

pushd .
CPCMD="cp"

cd $DEST/Contents
echo "This is where it lands..."
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

if [ ! -d "./MacOS" ]; then
 mkdir ./MacOS
fi

`$CPCMD $REPOSPATH/install/lib/libImagingAlgorithms.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libModuleConfig.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtAddons.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtImaging.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderConfig.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderGUI.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtModuleConfigure.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReconFramework.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReconAlgorithms.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdBackProjectors.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdPreprocModules.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libkipl.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdPreprocModulesGUI.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libInspectorModulesGUI.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libInspectorModules.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libFDKBackProjectors.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5.10.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_cpp.11.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_hl.10.dylib  $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libsz.2.dylib       $DEST/Contents/Frameworks`
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

cd ..

if [ ! -d "./Resources" ]; then
	mkdir ./Resources	
fi

cp $REPOSPATH/imagingsuite/applications/muhrec/Resources/* ./Resources

sed -i.bak s+com.yourcompany+ch.psi+g $DEST/Contents/Info.plist
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
	if [ -f "$QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib" ] ; then
		cp $QTPATH/plugins/accessible/libqtaccessiblewidgets.dylib $DEST/Contents/PlugIns/accessible/
	fi
fi

pwd
ls PlugIns

cd $QTBINPATH
echo "Do deploy..."
./macdeployqt $DEST #-dmg

cd $DEST/Contents/MacOS

install_name_tool -add_rpath @executable_path/../Frameworks MuhRec
# install_name_tool -change @rpath/libkipl.dylib @rpath/../Frameworks/libkipl.dylib MuhRec
# install_name_tool -change @rpath/libReaderGUI.dylib @rpath/../Frameworks/libReaderGUI.dylib MuhRec
# install_name_tool -change @rpath/libQtModuleConfigure.dylib @rpath/../Frameworks/libQtModuleConfigure.dylib MuhRec
# install_name_tool -change @rpath/libReconFramework.dylib @rpath/../Frameworks/libReconFramework.dylib MuhRec
# install_name_tool -change @rpath/libQtImaging.dylib @rpath/../Frameworks/libQtImaging.dylib MuhRec
# install_name_tool -change @rpath/libQtImaging.dylib @rpath/../Frameworks/libQtImaging.dylib MuhRec
# install_name_tool -change @rpath/libModuleConfig.dylib @rpath/../Frameworks/libModuleConfig.dylib MuhRec
# install_name_tool -change @rpath/libReaderConfig.dylib @rpath/../Frameworks/libReaderConfig.dylib MuhRec
# install_name_tool -change @rpath/libImagingAlgorithms.dylib @rpath/../Frameworks/libImagingAlgorithms.dylib MuhRec
# install_name_tool -change @rpath/libQtAddons.dylib @rpath/../Frameworks/libQtAddons.dylib MuhRec
# install_name_tool -change @rpath/QtWidgets.framework @rpath/../Frameworks/QtWidgets.framework MuhRec
# install_name_tool -change @rpath/QtGui.framework @rpath/../Frameworks/QtGui.framework MuhRec
# install_name_tool -change @rpath/QtConcurrent.framework @rpath/../Frameworks/QtConcurrent.framework MuhRec
# install_name_tool -change @rpath/QtCore.framework @rpath/../Frameworks/QtCore.framework MuhRec
cd $DEST/Contents/Frameworks
# # ReaderConfig
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @rpath/../Frameworks/libNeXus.1.0.0.dylib libReaderConfig.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/../Frameworks/libNeXusCPP.1.0.0.dylib libReaderConfig.dylib

# Nexus 
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexus.1.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexusCPP.1.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libNexusCPP.1.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReconFramework.1.0.0.dylib
install_name_tool -change /usr/lib/libz.1.dylib @rpath/../Frameworks/libz.1.dylib libNexus.1.dylib
install_name_tool -change /usr/lib/libz.1.dylib @rpath/../Frameworks/libz.1.dylib libNexusCPP.1.dylib
install_name_tool -change libNeXus.1.dylib @rpath/../Frameworks/libNeXus.1.dylib libNexusCPP.1.dylib

install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @rpath/../Frameworks/libhdf5.10.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @rpath/../Frameworks/libhdf5.10.dylib libhdf5_hl.10.dylib

install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @rpath/../Frameworks/libhdf5_cpp.11.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @rpath/../Frameworks/libhdf5.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @rpath/../Frameworks/libhdf5_hl.10.dylib libNeXus.1.0.0.dylib
install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @rpath/../Frameworks/libsz.2.dylib libNeXus.1.0.0.dylib

install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @rpath/../Frameworks/libhdf5_cpp.11.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @rpath/../Frameworks/libhdf5.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @rpath/../Frameworks/libhdf5_hl.10.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @rpath/../Frameworks/libsz.2.dylib libNeXusCPP.1.0.0.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5.10.dylib libhdf5.10.dylib libhdf5.10.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @rpath/../Frameworks/libsz.2.dylib libhdf5.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib @rpath/../Frameworks/libz.1.dylib libhdf5.10.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @rpath/../Frameworks/libsz.2.dylib libhdf5_cpp.11.dylib
install_name_tool -change /usr/lib/libz.1.dylib @rpath/../Frameworks/libz.1.dylib libhdf5_cpp.11.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib  libhdf5_hl.10.dylib libhdf5_hl.10.dylib
install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @rpath/../Frameworks/libsz.2.dylib libhdf5_hl.10.dylib
install_name_tool -change /usr/lib/libz.1.dylib @rpath/../Frameworks/libz.1.dylib libhdf5_hl.10.dylib

rm -rf /tmp/muhrec

if [ ! -d "/tmp/muhrec" ]; then
  mkdir /tmp/muhrec
fi

if [ ! -e "tmp/muhrec/Applications" ]; then
	ln -s /Applications /tmp/muhrec
fi

cp -r $DEST /tmp/muhrec

hdiutil create -volname MuhRec -srcfolder /tmp/muhrec -ov -format UDZO $DIRECTORY/MuhRec-MacOS_x86_64_build-$GITVER-`date +%Y%m%d`.dmg
