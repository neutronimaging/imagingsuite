﻿@echo ----------------- Deploy copy --------------
set REPOS=C:\%HOMEPATH%\git
set DEST=C:\%HOMEPATH%\imagingscience

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy %REPOS%\lib\ImagingAlgorithms.dll .
copy %REPOS%\lib\ModuleConfig.dll .
copy %REPOS%\lib\QtAddons.dll .
copy %REPOS%\lib\QtModuleConfigure.dll .
copy %REPOS%\lib\kipl.dll .
copy %REPOS%\lib\ReconFramework.dll .
copy %REPOS%\lib\StdBackProjectors.dll .
copy %REPOS%\lib\StdPreprocModules.dll .
copy %REPOS%\lib\StdPreprocModulesGUI.dll .
copy %REPOS%\lib\InspectorModulesGUI.dll .
copy %REPOS%\lib\InspectorModules.dll .
copy %REPOS%\lib\FDKBackProjectors.dll .

copy %REPOS%\imagingsuite\external\lib64\libtiff.dll .
copy %REPOS%\imagingsuite\external\lib64\libjpeg-62.dll .
copy %REPOS%\imagingsuite\external\lib64\zlib1.dll .
copy %REPOS%\imagingsuite\external\lib64\libfftw3-3.dll .
copy %REPOS%\imagingsuite\external\lib64\libfftw3f-3.dll .
copy %REPOS%\imagingsuite\external\lib64\libxml2-2.dll .
copy %REPOS%\imagingsuite\external\lib64\libiconv.dll .
copy %REPOS%\imagingsuite\external\lib64\cfitsio.dll .
copy %REPOS%\imagingsuite\external\lib64\nexus\NeXus.dll .
copy %REPOS%\imagingsuite\external\lib64\nexus\NexusCPP.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5_cpp.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\zlib.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\szip.dll .


copy %REPOS%\Applications\muhrec3.exe .

copy %REPOS%\imagingsuite\applications\muhrec3\resources resources

rem cd C:\Qt\5.8\msvc2015_64\bin
cd C:\Qt59\5.9.3\msvc2015_64\bin

windeployqt %DEST%\muhrec3.exe
copy Qt5PrintSupport.dll %DEST%

popd

