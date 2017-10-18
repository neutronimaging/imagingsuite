# This file helps you to build the binaries in the Modules repository

# Build ModuleConfig Lib
/Applications/Qt56/5.6/clang_64/bin/qmake ModuleConfig/qt/ModuleConfig.pro -r -spec macx-clang CONFIG+=x86_64 -o ModuleConfig/build-ModuleConfig-Qt5-Release/Makefile
pushd .
cd ModuleConfig/build-ModuleConfig-Qt5-Release
make -f Makefile clean
make -f Makefile mocables all
popd

# Build ReaderConfig Lib
/Applications/Qt56/5.6/clang_64/bin/qmake ReaderConfig/ReaderConfig.pro -r -spec macx-clang CONFIG+=x86_64 -o build-ReaderConfig-Desktop_Qt_5_6_1_clang_64bit-Release/Makefile
pushd .
cd build-ReaderConfig-Desktop_Qt_5_6_1_clang_64bit-Release
make -f Makefile clean
make -f Makefile mocables all
popd .

# Build ReaderGUI Lib
/Applications/Qt56/5.6/clang_64/bin/qmake ReaderGUI/ReaderGUI.pro -r -spec macx-clang CONFIG+=x86_64 -o build-ReaderGUI-Desktop_Qt_5_6_1_clang_64bit-Release/Makefile
pushd .
cd ReaderGUI-Desktop_Qt_5_6_1_clang_64bit-Release/
make -f Makefile clean
make -f Makefile mocables all
popd .
