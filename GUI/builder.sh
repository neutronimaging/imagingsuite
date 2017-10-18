# This file helps you to build the binaries in the KIPL repository

/Applications/Qt56/5.6/clang_64/bin/qmake qt/QtAddons/QtAddons.pro -r -spec macx-clang CONFIG+=x86_64 -o qt/build-QtAddons-Qt5-Release/Makefile

pushd .
cd qt/build-QtAddons-Qt5-Release/
make -f Makefile clean
make -f Makefile mocables all
popd .

/Applications/Qt56/5.6/clang_64/bin/qmake qt/QtModuleConfigure/QtModuleConfigure.pro -r -spec macx-clang CONFIG+=x86_64 -o qt/build-QtModuleConfigure-Qt5-Release/Makefile

pushd .
cd qt/build-QtModuleConfigure-Qt5-Release/
make -f Makefile clean
make -f Makefile mocables all
popd .
