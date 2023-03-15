# Windows build recipe

```bash
mkdir build
cd build
conan install ..\imagingsuite\ --profile ..\imagingsuite\profiles\windows_msvc_16_release --build=missing
activate.bat
C:\Qt\Tools\CMake_64\bin\cmake.exe ..\imagingsuite\ -DCMAKE_BUILD_TYPE=Release -DCMAKE_MODULE_PATH=\Users\ander\source\repos\conan-build\ -Wno-dev -DCMAKE_INSTALL_PREFIX="..\install\" -G="Visual Studio 16 2019"
cmake --build . --config Release
deactivate.bat
cd ..
conan install .. --build=all
```