DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/nitools"

BREWPATH="`brew --prefix`"

mkdir -p $DEST
pushd .
cd $DEST
touch __init__.py
mkdir -p imgalg
cd imgalg
touch __init__.py
cp $WORKSPACE/install/lib/imgalg.cpython-*-darwin.so .
cp $WORKSPACE/install/lib/muhrectomo.cpython-*-darwin.so .
cp $WORKSPACE/install/lib/advfilters.cpython-*-darwin.so .
cp $WORKSPACE/install/lib/libAdvancedFilters.dylib .
cp $WORKSPACE/install/lib/libImagingAlgorithms.dylib .
cp $WORKSPACE/install/lib/libkipl.dylib .
cp $WORKSPACE/install/lib/libStdBackProjectors.dylib .
cp $WORKSPACE/install/lib/libFDKBackProjectors.dylib .
cp $WORKSPACE/install/lib/libReconFramework.dylib .
cp $WORKSPACE/install/lib/libModuleConfig.dylib .
cp $WORKSPACE/install/lib/libReaderConfig.dylib .

cp $BREWPATH/opt/libtiff/lib/libtiff.6.dylib .
cp $BREWPATH/opt/armadillo/lib/libarmadillo.12.dylib .
cp $BREWPATH/opt/cfitsio/lib/libcfitsio.10.dylib .
# cp $WORKSPACE/ExternalDependencies/macos/arm64/lib/libNeXus.1.0.0.dylib .
# cp $WORKSPACE/ExternalDependencies/macos/arm64/lib/libNeXusCPP.1.0.0.dylib .
cp /usr/local/lib/libNeXus.1.0.0.dylib .
cp /usr/local/lib/libNeXusCPP.1.0.0.dylib .

cp $BREWPATH/opt/fftw/lib/libfftw3.3.dylib .
cp $BREWPATH/opt/fftw/lib/libfftw3f.3.dylib .
cp $BREWPATH/opt/openblas/lib/libopenblas.0.dylib .
cp $BREWPATH/opt/arpack/libexec/lib/libarpack.2.dylib .
cp $BREWPATH/opt/superlu/lib/libsuperlu.6.dylib .
chmod 666 *.dylib


rm -f libNeXus.1.dylib libNeXusCPP.1.dylib
ln -s libNeXus.1.0.0.dylib libNeXus.1.dylib
ln -s libNeXusCPP.1.0.0.dylib libNeXusCPP.1.dylib

install_name_tool -add_rpath @executable_path muhrectomo.cpython-*-darwin.so
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib muhrectomo.cpython-*-darwin.so
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib muhrectomo.cpython-*-darwin.so
install_name_tool -change libNeXus.1.dylib @rpath/libNeXus.1.dylib advfilters.cpython-*-darwin.so
install_name_tool -change libNeXusCPP.1.dylib @rpath/libNeXusCPP.1.dylib advfilters.cpython-*-darwin.so

cd ..
mkdir -p utils
cp $WORKSPACE/scripts/python/amglib/*.py utils

popd