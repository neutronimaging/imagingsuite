DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/imageviewer.app"
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

cp -r $REPOSPATH/Applications/imageviewer.app $DIRECTORY

pushd .
CPCMD="cp"

cd $DEST/Contents
echo "This is where it lands..."
pwd
if [ ! -d "./Frameworks" ]; then
 mkdir ./Frameworks
fi


`$CPCMD $REPOSPATH/lib/libModuleConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderConfig.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libReaderGUI.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libQtAddons.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libkipl.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXus.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libNeXusCPP.1.0.0.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5.10.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_cpp.11.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libhdf5_hl.10.dylib $DEST/Contents/Frameworks`
`$CPCMD $REPOSPATH/imagingsuite/external/mac/lib/libsz.2.dylib $DEST/Contents/Frameworks`

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
# imageviewer
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib imageviewer
install_name_tool -change libQtAddons.1.dylib @executable_path/../Frameworks/libQtAddons.1.dylib imageviewer
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib imageviewer
install_name_tool -change libReaderConfig.1.dylib @executable_path/../Frameworks/libReaderConfig.1.dylib imageviewer
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib imageviewer
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib imageviewer
install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib imageviewer
install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib imageviewer
install_name_tool -change libcfitsio.1.dylib @executable_path/../Frameworks/libcfitsio.dylib imageviewer

cd ../Frameworks
# QtAddons
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libQtAddons.1.0.0.dylib

# ImagingAlgorithms
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libImagingAlgorithms.1.0.0.dylib

# ReaderConfig
install_name_tool -change libkipl.1.dylib @executable_path/../Frameworks/libkipl.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libImagingAlgorithms.1.dylib @executable_path/../Frameworks/libImagingAlgorithms.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib libReaderConfig.1.0.0.dylib
install_name_tool -change libcfitsio.1.dylib @executable_path/../Frameworks/libcfitsio.dylib libReaderConfig.1.0.0.dylib

#nexus_related
install_name_tool -change libNeXus.1.dylib @executable_path/../Frameworks/libNeXus.1.dylib libkipl.1.0.0.dylib
install_name_tool -change libNeXusCPP.1.dylib @executable_path/../Frameworks/libNeXusCPP.1.dylib libkipl.1.0.0.dylib
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


rm -rf /tmp/imageviewer

if [ ! -d "/tmp/imageviewer" ]; then
  mkdir /tmp/imageviewer
fi

if [ ! -e "tmp/imageviewer/Applications" ]; then
	ln -s /Applications /tmp/imageviewer
fi

cp -r $DEST /tmp/imageviewer

hdiutil create -volname ImageViewer -srcfolder /tmp/imageviewer -ov -format UDZO $DIRECTORY/ImageViewer_build-$GITVER-`date +%Y%m%d`.dmg
