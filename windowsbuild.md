# Windows build recipe

```bash
pip install --upgrade conan
conan install . --profile:host profiles\windows_msvc_17_release --profile:build profiles\windows_msvc_17_release
conan build .
cd ../build-imagingsuite/bin/Release
%QTPATH%/bin/windeployqt muhrec.exe
%QTPATH%/bin/windeployqt imageviewer.exe
"C:\Program Files\7-Zip\7z.exe" a ../../../MuhRec-Windows.zip "./*" -xr!t*.exe -xr!muhreccli.exe -xr!verticalslicer.exe -xr!Testing
```