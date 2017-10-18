# This file helps you to build the binaries in the KIPL repository

/Applications/Qt56/5.6/clang_64/bin/qmake kipl/qt/kipl.pro -r -spec macx-clang CONFIG+=x86_64 -o kipl/build-kipl-Qt5-Release/Makefile
cd kipl/build-kipl-Qt5-Release/
make -f Makefile clean
make -f Makefile mocables all

