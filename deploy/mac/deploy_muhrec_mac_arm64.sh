DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/MuhRec.app"
REPOSPATH=$WORKSPACE
QTPATH=$QTBINPATH

ARCH=`uname -m`

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

# `$CPCMD $REPOSPATH/install/lib/libImagingAlgorithms.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libModuleConfig.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libQtAddons.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libQtImaging.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libReaderConfig.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libReaderGUI.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libQtModuleConfigure.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libReconFramework.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libReconAlgorithms.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libStdBackProjectors.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libStdPreprocModules.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libkipl.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libStdPreprocModulesGUI.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libInspectorModulesGUI.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libInspectorModules.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/install/lib/libFDKBackProjectors.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/ExternalDependencies/macos/arm64/lib/libNeXus.1.0.0.dylib $DEST/Contents/MacOS`
# `$CPCMD $REPOSPATH/ExternalDependencies/macos/arm64/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/MacOS`
# `$CPCMD /opt/local/lib/libhdf5.10.dylib    $DEST/Contents/MacOS`
# `$CPCMD /opt/local/lib/libhdf5_cpp.dylib $DEST/Contents/MacOS`
# `$CPCMD /opt/local/lib/libhdf5_hl.dylib  $DEST/Contents/MacOS`
# `$CPCMD /opt/local/lib/libsz.2.dylib        $DEST/Contents/MacOS`
# `$CPCMD /opt/local/lib/libarmadillo*.dylib $DEST/Contents/MacOS`


`$CPCMD $REPOSPATH/install/lib/libImagingAlgorithms.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libModuleConfig.dylib         $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtAddons.dylib             $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtImaging.dylib            $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderConfig.dylib         $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderGUI.dylib            $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtModuleConfigure.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReconFramework.dylib       $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReconAlgorithms.dylib      $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdBackProjectors.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdPreprocModules.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libkipl.dylib                 $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libStdPreprocModulesGUI.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libInspectorModulesGUI.dylib  $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libInspectorModules.dylib     $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libFDKBackProjectors.dylib    $DEST/Contents/Frameworks`
	
`$CPCMD $REPOSPATH/install/lib/muhrectomo.cpython-39-darwin.so      $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/imgalg.cpython-39-darwin.so          $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/advancedfilters.cpython-39-darwin.so $DEST/Contents/Frameworks`

`$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5.10.dylib    $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5_cpp.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5_hl.dylib  $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libsz.2.dylib        $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libarmadillo*.dylib $DEST/Contents/Frameworks`

if [ -e "$REPOSPATH/Applications/muhrecCLI" ]; then
	`$CPCMD $REPOSPATH/Applications/muhrecCLI $DEST/Contents/MacOS`
fi



rm -f ./MacOS/*.dylib
cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

if [ -e "/opt/local/lib/libzstd.1.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.1.dylib $DEST/Contents/MacOS`
fi

if [ -e "/opt/local/lib/libzstd.9.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.9.dylib $DEST/Contents/MacOS`
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

# muhrec
install_name_tool -add_rpath @executable_path/../Frameworks MuhRec

#install_name_tool -add_rpath "@executable_path" MuhRec
#install_name_tool -add_rpath "@loader_path/../lib" MuhRec
#install_name_tool -change @rpath/libReaderGUI.dylib libReaderGUI.dylib MuhRec
# # install_name_tool -change @rpath/libQtModuleConfigure.dylib @executable_path/../Frameworks/libQtModuleConfigure.dylib MuhRec
# # install_name_tool -change @rpath/libReconFramework.dylib @executable_path/../Frameworks/libReconFramework.dylib MuhRec

# # install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib MuhRec
# # install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib MuhRec
# # install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib MuhRec
# # install_name_tool -change libQtImaging.1.dylib @executable_path/../Frameworks/libQtImaging.1.dylib MuhRec
# # install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib MuhRec
# # install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib MuhRec
# # install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib MuhRec
# # install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib MuhRec
# # install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib MuhRec
# # install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib MuhRec
# # install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib MuhRec
# # install_name_tool -change libReaderGUI.1.dylib @executable_path/../Frameworks/libReaderGUI.1.dylib MuhRec

# # muhrecCLI
# if [ -e "muhrecCLI" ]; then
# 	install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib muhrecCLI
# 	install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib muhrecCLI
# 	install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib muhrecCLI
# 	install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib muhrecCLI
# 	install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib muhrecCLI
# 	install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib muhrecCLI
# fi

cd ../Frameworks

# # kipl
# install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib libkipl.1.0.0.dylib
# install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libtiff.5.dylib libkipl.1.0.0.dylib
# install_name_tool -change libarmadillo.10.dylib @executable_path/../Frameworks/libarmadillo.10.dylib libkipl.1.0.0.dylib

# # ModuleConfig
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libModuleConfig.1.0.0.dylib

# # ReconFramework
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReconFramework.1.0.0.dylib


# # QtAddons
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# # ReaderConfig
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReaderConfig.1.0.0.dylib

# # ReaderConfigGUI
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderGUI.1.0.0.dylib
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libReaderGUI.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libReaderGUI.1.0.0.dylib

# # QtImaging
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libQtImaging.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libQtImaging.1.0.0.dylib


# # QtModuleConfigure
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtModuleConfigure.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libQtModuleConfigure.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libQtModuleConfigure.1.0.0.dylib

# # ImagingAlgorithms
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# #nexus_related
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReconFramework.1.0.0.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexus.1.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libNexusCPP.1.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libNexusCPP.1.dylib

# install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libhdf5_cpp.11.dylib
# install_name_tool -change /usr/local/Cellar/hdf5/1.8.16_1/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libhdf5_hl.10.dylib

# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @executable_path/../Frameworks/libhdf5_cpp.11.dylib libNeXus.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libNeXus.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @executable_path/../Frameworks/libhdf5_hl.10.dylib libNeXus.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libNeXus.1.0.0.dylib

# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib @executable_path/../Frameworks/libhdf5_cpp.11.dylib libNeXusCPP.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5.10.dylib @executable_path/../Frameworks/libhdf5.10.dylib libNeXusCPP.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib @executable_path/../Frameworks/libhdf5_hl.10.dylib libNeXusCPP.1.0.0.dylib
# install_name_tool -change  /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libNeXusCPP.1.0.0.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5.10.dylib libhdf5.10.dylib libhdf5.10.dylib
# install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5.10.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5.10.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib libhdf5_cpp.11.dylib
# install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5_cpp.11.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5_cpp.11.dylib

# install_name_tool -change /usr/local/opt/hdf5/lib/libhdf5_hl.10.dylib  libhdf5_hl.10.dylib libhdf5_hl.10.dylib
# install_name_tool -change /usr/local/opt/szip/lib/libsz.2.dylib @executable_path/../Frameworks/libsz.2.dylib libhdf5_hl.10.dylib
# install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libhdf5_hl.10.dylib

# #FDKBackProjector
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libFDKBackProjectors.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libFDKBackProjectors.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libFDKBackProjectors.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libFDKBackProjectors.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libFDKBackProjectors.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libFDKBackprojectors.1.0.0.dylib
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libFDKBackprojectors.1.0.0.dylib
# # Inspectors
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModules.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libInspectorModules.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModules.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libInspectorModules.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libInspectorModules.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib  @executable_path/../Frameworks/libModuleConfig.1.dylib  libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib  @executable_path/../Frameworks/libModuleConfig.1.dylib  libInspectorModules.1.0.0.dylib
# install_name_tool -change libQtModuleConfigure.1.dylib  @executable_path/../Frameworks/libQtModuleConfigure.1.dylib  libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libQtModuleConfigure.1.dylib  @executable_path/../Frameworks/libQtModuleConfigure.1.dylib  libInspectorModules.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib  @executable_path/../Frameworks/libQtAddons.1.dylib  libInspectorModulesGUI.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libInspectorModulesGUI.1.0.0.dylib

# #ReconAlgorithms
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReconAlgorithms.1.0.0.dylib

# # stdbackprojectors
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdBackprojectors.1.0.0.dylib
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib libStdBackprojectors.1.0.0.dylib

# # Preprocessing
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdPreprocModules.1.0.0.dylib
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libModuleConfig.1.dylib @executable_path/../Frameworks/libModuleConfig.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libReconFramework.1.dylib @executable_path/../Frameworks/libReconFramework.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libStdPreprocModules.1.dylib @executable_path/../Frameworks/libStdPreprocModules.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libQtModuleConfigure.1.dylib @executable_path/../Frameworks/libQtModuleConfigure.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libStdPreprocModulesGUI.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libStdPreprocModulesGUI.1.0.0.dylib

# pymuhrec
ln -s libNeXus.1.0.0.dylib libNeXus.1.dylib
install_name_tool -add_rpath "../Frameworks" muhrectomo.cpython-39-darwin.so
install_name_tool -change /Users/kaestner/git/ExternalDependencies/sources/build_nexus/src/libNeXus.1.dylib @rpath/libNeXus.1.dylib muhrectomo.cpython-39-darwin.so
install_name_tool -change /Users/kaestner/git/ExternalDependencies/sources/build_nexus/bindings/cpp/libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib muhrectomo.cpython-39-darwin.so

install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libStdBackProjectors.dylib
install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libStdBackProjectors.dylib
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libStdBackProjectors.dylib
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libStdBackProjectors.dylib
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libStdBackProjectors.dylib

install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libReconFramework.dylib 
install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libReconFramework.dylib
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libReconFramework.dylib 
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libReconFramework.dylib 
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libReconFramework.dylib   
install_name_tool -change @executable_path/../Frameworks/libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libReconFramework.dylib

install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libImagingAlgorithms.dylib 
install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libImagingAlgorithms.dylib
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libImagingAlgorithms.dylib 
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libImagingAlgorithms.dylib 
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libImagingAlgorithms.dylib   
install_name_tool -change @executable_path/../Frameworks/libxml2.2.dylib @rpath/libxml2.2.dylib libImagingAlgorithms.dylib

install_name_tool -change @executable_path/../Frameworks/libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libkipl.dylib          
install_name_tool -change @executable_path/../Frameworks/libNeXus.1.dylib @rpath/libNeXus.1.dylib libkipl.dylib 
install_name_tool -change @executable_path/../Frameworks/libxml2.2.dylib @rpath/libxml2.2.dylib libkipl.dylib
install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libkipl.dylib
install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libkipl.dylib
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libkipl.dylib                    
install_name_tool -change @executable_path/../Frameworks/libctiff.5.dylib @rpath/libtiff.5.dylib libkipl.dylib
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libkipl.dylib 
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libkipl.dylib   

install_name_tool -change @executable_path/../Frameworks/libicui18n.67.dylib @rpath/libicui18n.67.dylib libxml2.2.dylib
install_name_tool -change @executable_path/../Frameworks/libicuuc.67.dylib @rpath/libicuuc.67.dylib libxml2.2.dylib
install_name_tool -change @executable_path/../Frameworks/libicudata.67.dylib @rpath/libicudata.67.dylib libxml2.2.dylib
install_name_tool -change @executable_path/../Frameworks/libicuuc.67.dylib @rpath/libicuuc.67.dylib libicui18n.67.dylib 
install_name_tool -change @executable_path/../Frameworks/libicudata.67.dylib @rpath/libicudata.67.dylib libicui18n.67.dylib 
install_name_tool -change @executable_path/../Frameworks/libicudata.67.dylib @rpath/libicudata.67.dylib libicuuc.67.dylib

install_name_tool -change @executable_path/../Frameworks/libzstd.1.dylib @rpath/libzstd.1.dylib libtiff.5.dylib
install_name_tool -change @executable_path/../Frameworks/libjpeg.8.dylib @rpath/libjpeg.8.dylib libtiff.5.dylib
install_name_tool -change @executable_path/../Frameworks/libhdf5.200.dylib @rpath/libhdf5.200.dylib libarmadillo.10.dylib
install_name_tool -change @executable_path/../Frameworks/libhdf5_cpp.200.dylib @rpath/libhdf5_cpp.200.dylib libNeXus.1.0.0.dylib 
install_name_tool -change @executable_path/../Frameworks/libhdf5.200.dylib @rpath/libhdf5.200.dylib libNeXus.1.0.0.dylib 
install_name_tool -change @executable_path/../Frameworks/libhdf5.200.dylib @rpath/libhdf5.200.dylib libhdf5_cpp.200.dylib 
install_name_tool -change @executable_path/../Frameworks/libhdf5_cpp.200.dylib @rpath/libhdf5_cpp.200.dylib libNeXusCPP.1.0.0.dylib
install_name_tool -change @executable_path/../Frameworks/libhdf5_cpp.200.dylib @rpath/libhdf5_cpp.200.dylib libNeXusCPP.1.dylib    
install_name_tool -change @executable_path/../Frameworks/libhdf5.200.dylib @rpath/libhdf5.200.dylib libNeXusCPP.1.dylib

install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libModuleConfig.dylib
install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libModuleConfig.dylib
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libModuleConfig.dylib
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libModuleConfig.dylib
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libModuleConfig.dylib

install_name_tool -change @executable_path/../Frameworks/libfftw3f.3.dylib @rpath/libfftw3f.3.dylib libFDKBackProjectors.dylib   
install_name_tool -change @executable_path/../Frameworks/libfftw3.3.dylib @rpath/libfftw3.3.dylib libFDKBackProjectors.dylib 
install_name_tool -change @executable_path/../Frameworks/libcfitsio.dylib @rpath/libcfitsio.dylib libFDKBackProjectors.dylib     
install_name_tool -change @executable_path/../Frameworks/libtiff.5.dylib @rpath/libtiff.5.dylib libFDKBackProjectors.dylib       
install_name_tool -change @executable_path/../Frameworks/libarmadillo.10.dylib @rpath/libarmadillo.10.dylib libFDKBackProjectors.dylib

install_name_tool -change @executable_path/../Frameworks/libiconv.2.dylib @rpath/libiconv.2.dylib libxml2.2.dylib

rm -rf /tmp/muhrec

if [ ! -d "/tmp/muhrec" ]; then
  mkdir /tmp/muhrec
fi

if [ ! -e "tmp/muhrec/Applications" ]; then
	ln -s /Applications /tmp/muhrec
fi

cp -r $DEST /tmp/muhrec

hdiutil create -volname MuhRec -srcfolder /tmp/muhrec -ov -format UDZO $DIRECTORY/MuhRec-MacOS_arm64_build-$GITVER-`date +%Y%m%d`.dmg
