#!/bin/bash
#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH="/home/`whoami`/repos"

DEST="$REPOSPATH/imagingtool/imagingtool"
mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/Frameworks
mkdir --parent $DEST/resources

pushd .
cd $DEST/Frameworks

target="Release"
platform='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
   REPOSPATH="/home/kaestner/repos"
   EXT="so.1.0.0"
   EXT1="so.1.0"
   EXT2="so.1"
elif [[ "$unamestr" == 'Darwin' ]]; then
   DEST="build-imagingtool-Qt5-$target/imagingtool.app/Contents/MacOS"
   REPOSPATH="/Users/kaestner/repos"
   EXT="1.0.0.dylib"
   EXT1="1.0.dylib"
   EXT2="1.dylib"
fi

pushd .
cd $DEST
rm -f *.$EXT


`$CPCMD $REPOSPATH/modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-$target/libModuleConfig.$EXT .`
`$CPCMD $REPOSPATH/modules/trunk/build-ReaderConfig-Qt5-$target/libReaderConfig.$EXT .`
`$CPCMD $REPOSPATH/modules/trunk/build-ReaderGUI-Qt5-$target/libReaderGUI.$EXT .`
`$CPCMD $REPOSPATH/gui/trunk/qt/build-QtAddons-Qt5-$target/libQtAddons.$EXT .`
`$CPCMD $REPOSPATH/tomography/trunk/src/build-ReconFramework-Qt5-$target/libReconFramework.$EXT .`
`$CPCMD $REPOSPATH/kipl/trunk/kipl/build-kipl-Qt5-$target/libkipl.$EXT .`
`$CPCMD $REPOSPATH/qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-$target/libImagingAlgorithms.$EXT .`

rm -f *.so
rm -f *.so.1
rm -f *.so.1.0

for f in `ls *.so.1.0.0`; do
	bn=`basename $f .so.1.0.0`
	echo $bn
	ln -s $f $bn.so.1.0
        ln -s $bn.so.1.0 $bn.so.1
	ln -s $bn.so.1 $bn.so
done

cd $DEST/bin
`$CPCMD $REPOSPATH/imagingtool/trunk/imagingtool-build-desktop-Qt_4_8_1_in_PATH__System__Release/imagingtool .`
cd ..
`$CPCMD $REPOSPATH/imagingtool/trunk/scripts/imagingtool .`
chmod +x imagingtool

#cp $REPOSPATH/tomography/trunk/src/muhrec3/resources/*.xml resources
popd


