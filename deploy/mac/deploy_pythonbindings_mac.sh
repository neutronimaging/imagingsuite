DIRECTORY=$WORKSPACE/deployed

DEST="$DIRECTORY/nitools"

mkdir -p $DEST
pushd .
cd $DEST
touch __init__.py
mkdir -p imgalg
cd imgalg
touch __init__.py
cp $WORKSPACE/install/lib/imgalg.cpython-39-darwin.so .
cp $WORKSPACE/install/lib/libImagingAlgorithms.dylib .
cp $WORKSPACE/install/lib/advfilters.cpython-39-darwin.so .
cp $WORKSPACE/install/lib/libAdvancedFilters.dylib .
cp $WORKSPACE/install/lib/libkipl.dylib .
cp $WORKSPACE/install/lib/muhrectomo.cpython-39-darwin.so .
cp $WORKSPACE/install/lib/libStdBackProjectors.dylib .
cp $WORKSPACE/install/lib/libFDKBackProjectors.dylib .
cp $WORKSPACE/install/lib/libReconFramework.dylib .
cp $WORKSPACE/install/lib/libModuleConfig.dylib .
cp $WORKSPACE/install/lib/libReaderConfig.dylib .

cp /opt/homebrew/opt/libtiff/lib/libtiff.6.dylib .
cp /opt/homebrew/opt/armadillo/lib/libarmadillo.12.dylib .
cp /opt/homebrew/opt/cfitsio/lib/libcfitsio.10.dylib .
# cp libNeXus.1.dylib .
# cp libNeXusCPP.1.dylib .
cp /opt/homebrew/opt/fftw/lib/libfftw3.3.dylib .
cp /opt/homebrew/opt/fftw/lib/libfftw3f.3.dylib .
cp /opt/homebrew/opt/openblas/lib/libopenblas.0.dylib .
cp /opt/homebrew/opt/arpack/libexec/lib/libarpack.2.dylib .
cp /opt/homebrew/opt/superlu/lib/libsuperlu.6.dylib .
chmod 666 *.dylib

cd ..
mkdir -p utils
cp $WORKSPACE/scripts/python/amglib/*.py utils

popd