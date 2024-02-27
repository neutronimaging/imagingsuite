# Windows build recipe

```bash
mkdir install
mkdir install\applications
mkdir install\lib
mkdir install\tests
mkdir deployed
pip install --upgrade conan
conan install . --profile:host profiles\windows_msvc_17_release --profile:build profiles\windows_msvc_17_release
conanbuild.bat
cd ../build-imagingsuite
cmake ..\imagingsuite\ -DCMAKE_INSTALL_PREFIX=../install/ -DCMAKE_PREFIX_PATH=%QTPATH%/lib/cmake/ -G="Visual Studio 17 2022" -B .
cmake --build . --target install --config Release
deactivate_conanbuild.bat
cd ../imagingsuite/deploy/win
deploymuhrec_cmake.bat
```