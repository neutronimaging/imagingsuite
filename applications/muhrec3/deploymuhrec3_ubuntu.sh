#CPCMD="ln -s "
CPCMD="cp "
REPOSPATH="/home/`whoami`/repos"

DEST="$REPOSPATH/tomography/muhrec3"
mkdir --parent $DEST
mkdir --parent $DEST/bin
mkdir --parent $DEST/Frameworks
mkdir --parent $DEST/resources

pushd .
cd $DEST/Frameworks
rm -f *.1.0.0.dylib

`$CPCMD $REPOSPATH/lib/libQtAddons.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libQtModuleConfigure.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libImagingAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libkipl.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libModuleConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReaderConfig.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReaderGUI.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReconFramework.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libReconAlgorithms.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libStdBackProjectors.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libStdPreprocModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libStdPreprocModulesGUI.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libInspectorModules.so.1.0.0 .`
`$CPCMD $REPOSPATH/lib/libInspectorModulesGUI.so.1.0.0 .`

`$CPCMD $REPOSPATH/lib/libFDKBackProjectors.so.1.0.0 .`

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
`$CPCMD $REPOSPATH/Applications/muhrec3 .`
cd ..
`$CPCMD $REPOSPATH/tomography/trunk/src/scripts/muhrec3 .`
chmod +x muhrec3

cp $REPOSPATH/tomography/trunk/src/muhrec3/resources/*.xml resources
popd


