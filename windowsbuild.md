# Windows build recipe

```bash
mkdir build-imagingsuite
mkdir install
mkdir install\applications
mkdir install\lib
mkdir install\tests
mkdir deployed
cd build-imagingsuite
pip install --upgrade conan
conan install ..\imagingsuite\ --profile:host ..\imagingsuite\profiles\windows_msvc_17_release --profile:build ..\imagingsuite\profiles\windows_msvc_17_release --output-folder=..\build-imagingsuite
conanbuild.bat
cmake ..\imagingsuite\ -DCMAKE_INSTALL_PREFIX=../install/ -DCMAKE_PREFIX_PATH=%QTPATH%/lib/cmake/ -G="Visual Studio 17 2022" -B .
cmake --build . --target install --config Release
deactivate_conanbuild.bat
cd ../imagingsuite/deploy/win
deploymuhrec_cmake.bat
```