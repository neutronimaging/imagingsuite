#CPCMD="ln -s "
CPCMD="cp "

platform='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
   DEST="imagingtool-build-desktop-Qt_4_8_1_in_PATH__System__Release/"
   REPOSPATH="/home/kaestner/repos"
   EXT="so.1.0.0"
   EXT1="so.1.0"
   EXT2="so.1"
elif [[ "$unamestr" == 'Darwin' ]]; then
   DEST="imagingtool-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Release/imagingtool.app/Contents/MacOS"
   REPOSPATH="/Users/kaestner/repos"
   EXT="1.0.0.dylib"
   EXT1="1.0.dylib"
   EXT2="1.dylib"
fi

pushd .
cd $DEST
rm -f *.$EXT

`$CPCMD $REPOSPATH/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libModuleConfig.$EXT .`
`$CPCMD $REPOSPATH/gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libQtAddons.$EXT .`
`$CPCMD $REPOSPATH/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/libReconFramework.$EXT .`
`$CPCMD $REPOSPATH/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/libkipl.$EXT .`

rm -f *.$EXT1
rm -f *.$EXT2

for f in `ls *.$EXT`; do
	bn=`basename $f .$EXT`
	echo $bn
	ln -s $f $bn.$EXT1
    ln -s $bn.$EXT1 $bn.$EXT2
done

popd
