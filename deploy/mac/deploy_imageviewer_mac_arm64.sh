DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/ImageViewer.app"
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

cp -r $REPOSPATH/install/applications/ImageViewer.app $DIRECTORY

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

`$CPCMD $REPOSPATH/install/lib/libModuleConfig.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderConfig.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libReaderGUI.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtAddons.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libkipl.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtModuleConfigure.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libQtImaging.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/install/lib/libImagingAlgorithms.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/ExternalDependencies/macos/$ARCH/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5.10.dylib    $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5_cpp.dylib $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libhdf5_hl.dylib  $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libsz.2.dylib        $DEST/Contents/Frameworks`
`$CPCMD /opt/local/lib/libarmadillo*.dylib $DEST/Contents/Frameworks`

rm -f ./MacOS/*.dylib

cd Frameworks
rm -f *.1.0.dylib
rm -f *.1.dylib

for f in `ls *.1.0.0.dylib`; do
	ln -s $f "`basename $f .1.0.0.dylib`.1.0.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.1.dylib"
	ln -s $f "`basename $f .1.0.0.dylib`.dylib"
done

if [ -e "/opt/local/lib/libzstd.1.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.1.dylib $DEST/Contents/Frameworks`
fi

if [ -e "/opt/local/lib/libzstd.9.dylib" ]; then
	`$CPCMD /opt/local/lib/libzstd.9.dylib $DEST/Contents/Frameworks`
fi

popd

cd $QTBINPATH
echo "Do deploy..."
./macdeployqt $DEST #-dmg

cd $DEST/Contents/MacOS
echo 
echo Reassign dependencies
echo
pwd

install_name_tool -add_rpath @executable_path/../Frameworks ImageViewer
cd ../Frameworks
# # kipl
# install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib libkipl.1.0.0.dylib
# install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libtiff.5.dylib libkipl.1.0.0.dylib
# install_name_tool -change libarmadillo.10.dylib @executable_path/../Frameworks/libarmadillo.10.dylib libkipl.1.0.0.dylib

# # imageviewer
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib imageviewer
# install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib imageviewer
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib imageviewer
# install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib imageviewer
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib imageviewer
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib imageviewer
# install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib imageviewer
# install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib imageviewer
# install_name_tool -change libcfitsio.1.dylib @executable_path/../Frameworks/libcfitsio.dylib imageviewer

# cd ../Frameworks
# # QtAddons
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# # ImagingAlgorithms
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# # ReaderConfig
# install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib libReaderConfig.1.0.0.dylib
# install_name_tool -change libcfitsio.1.dylib @executable_path/../Frameworks/libcfitsio.dylib libReaderConfig.1.0.0.dylib

# #nexus_related
install_name_tool -change libNeXus.1.dylib @rpath/Frameworks/libNeXus.1.dylib libkipl.dylib
install_name_tool -change libNeXusCPP.1.dylib @rpath/Frameworks/libNeXusCPP.1.dylib libkipl.dylib
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib libNexusCPP.1.0.0.dylib

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


rm -rf /tmp/imageviewer

if [ ! -d "/tmp/imageviewer" ]; then
  mkdir /tmp/imageviewer
fi

if [ ! -e "tmp/imageviewer/Applications" ]; then
	ln -s /Applications /tmp/imageviewer
fi

cp -r $DEST /tmp/imageviewer

hdiutil create -volname ImageViewer -srcfolder /tmp/imageviewer -ov -format UDZO $DIRECTORY/ImageViewer_$ARCH-build-$GITVER-`date +%Y%m%d`.dmg
