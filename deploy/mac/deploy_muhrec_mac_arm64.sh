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
CPCMD="cp -f"

cd $DEST/Contents
echo "This is where it lands..."
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi

if [ ! -d "./MacOS" ]; then
 mkdir ./MacOS
fi

`$CPCMD $REPOSPATH/install/lib/libImagingAlgorithms.dylib    $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libAdvancedFilters.dylib      $DEST/Contents/Frameworks`
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
	
# `$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
# `$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`

`$CPCMD /usr/local/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /usr/local/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`

`$CPCMD /opt/homebrew/lib/libarmadillo.12.dylib $DEST/Contents/Frameworks`

`$CPCMD /opt/homebrew/opt/openblas/lib/libopenblas.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/homebrew/opt/arpack/lib/libarpack.2.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/homebrew/opt/superlu/lib/libsuperlu.6.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/homebrew/lib/libzstd.1.dylib        $DEST/Contents/Frameworks`
`$CPCMD /opt/homebrew/lib/libsz.2.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/homebrew/opt/gcc/lib/gcc/current/libgcc_s.1.1.dylib $DEST/Contents/Frameworks`
# `$CPCMD /Users/kaestner/anaconda3/lib/libgcc_s.1.1.dylib $DEST/Contents/Frameworks`

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

#sed -i.bak s+com.yourcompany+ch.psi+g $DEST/Contents/Info.plist
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

install_name_tool -change @executable_path/../Frameworks/libxml2.2.dylib @rpath/libxml2.2.dylib MuhRec
install_name_tool -change @executable_path/../Frameworks/libarmadillo.12.dylib @rpath/libarmadillo.12.dylib MuhRec
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib MuhRec
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib MuhRec
cd ../Frameworks

chmod 666 libarmadillo.12.dylib
chmod 666 libhdf5_hl.dylib libhdf5_cpp.dylib
ln -s libNeXus.1.0.0.dylib libNeXus.1.dylib
ln -s libNeXusCPP.1.0.0.dylib libNeXusCPP.1.dylib
`$CPCMD /opt/homebrew/opt/gcc/lib/gcc/current/libgcc_s.1.1.dylib $DEST/Contents/Frameworks` # must be here to avoid deletion
`$CPCMD /opt/homebrew/lib/libzstd.1.dylib        $DEST/Contents/Frameworks`

install_name_tool -change @loader_path/../../../../opt/openblas/lib/libopenblas.0.dylib @rpath/libopenblas.0.dylib libarmadillo.12.dylib
install_name_tool -change @loader_path/../../../../opt/arpack/libexec/lib/libarpack.2.dylib @rpath/libarpack.2.dylib libarmadillo.12.dylib
install_name_tool -change @loader_path/../../../../opt/superlu/lib/libsuperlu.6.dylib @rpath/libsuperlu.6.dylib libarmadillo.12.dylib
install_name_tool -change @loader_path/../../../../opt/libaec/lib/libsz.2.dylib @rpath/libsz.2.dylib libhdf5_cpp.310.dylib
install_name_tool -change @loader_path/../../../../opt/libaec/lib/libsz.2.dylib @rpath/libsz.2.dylib libhdf5.310.dylib

install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libReaderGUI.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libReaderGUI.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libReaderConfig.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libReaderConfig.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libImagingAlgorithms.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libImagingAlgorithms.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libkipl.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libkipl.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libQtAddons.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libQtAddons.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libQtModuleConfigure.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libQtModuleConfigure.dylib
#install_name_tool -change @executable_path/../Frameworks/libzstd.1.dylib @rpath/libzstd.1.dylib libtiff.6.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libReconFramework.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libReconFramework.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libModuleConfig.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libModuleConfig.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libQtImaging.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib libQtImaging.dylib

codesign --force --deep --sign - $DEST # needed to fix signature issue on M1

rm -rf /tmp/muhrec

if [ ! -d "/tmp/muhrec" ]; then
  mkdir /tmp/muhrec
fi

if [ ! -e "tmp/muhrec/Applications" ]; then
	ln -s /Applications /tmp/muhrec
fi

cp -r $DEST /tmp/muhrec

hdiutil create -volname MuhRec -srcfolder /tmp/muhrec -ov -format UDZO $DIRECTORY/MuhRec-MacOS_$ARCH-build-$GITVER-`date +%Y%m%d`.dmg
