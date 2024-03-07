# Windows build recipe

```bash
pip install --upgrade conan
conan install . --profile:host profiles\windows_msvc_17_release --profile:build profiles\windows_msvc_17_release
#conanbuild.bat
cd ../build-imagingsuite
cmake ..\imagingsuite\ -DCMAKE_PREFIX_PATH=%QTPATH%/lib/cmake/ -G="Visual Studio 17 2022" -B .
cmake --build . --config Release
#deactivate_conanbuild.bat
cd ./applications/Release
%QTPATH%/bin/windeployqt muhrec.exe
%QTPATH%/bin/windeployqt imageviewer.exe
"C:\Program Files\7-Zip\7z.exe" a ./MuhRec-Windows.zip "./applications/Release/*" -xr!t*.exe -xr!muhreccli.exe -xr!verticalslicer.exe -xr!Testing "./lib/Release/*.cp*"
```