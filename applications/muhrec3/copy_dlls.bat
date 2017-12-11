@echo ----------------- Deploy copy --------------
set REPOS=C:\%HOMEPATH%\git
set DEST=C:\%HOMEPATH%\MuhRec

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy C:\%HOMEPATH%\lib\ImagingAlgorithms.dll .
copy C:\%HOMEPATH%\lib\ModuleConfig.dll .
copy C:\%HOMEPATH%\lib\QtAddons.dll .
copy C:\%HOMEPATH%\lib\QtModuleConfigure.dll .
copy C:\%HOMEPATH%\lib\kipl.dll .
copy C:\%HOMEPATH%\lib\ReconFramework.dll .
copy C:\%HOMEPATH%\lib\StdBackProjectors.dll .
copy C:\%HOMEPATH%\lib\StdPreprocModules.dll .
copy C:\%HOMEPATH%\lib\StdPreprocModulesGUI.dll .
copy C:\%HOMEPATH%\lib\InspectorModulesGUI.dll .
copy C:\%HOMEPATH%\lib\InspectorModules.dll .
copy C:\%HOMEPATH%\lib\FDKBackProjectors.dll .

copy %REPOS%\external\lib64\libtiff.dll .
copy %REPOS%\external\lib64\libjpeg-62.dll .
copy %REPOS%\external\lib64\zlib1.dll .
copy %REPOS%\external\lib64\libfftw3-3.dll .
copy %REPOS%\external\lib64\libfftw3f-3.dll .
copy %REPOS%\external\lib64\libxml2-2.dll .
copy %REPOS%\external\lib64\libiconv.dll .
copy %REPOS%\external\lib64\cfitsio.dll .
copy %REPOS%\external\lib64\nexus\NeXus.dll .
copy %REPOS%\external\lib64\nexus\NexusCPP.dll .
copy %REPOS%\external\lib64\hdf5\hdf5.dll .
copy %REPOS%\external\lib64\hdf5\hdf5_cpp.dll .
copy %REPOS%\external\lib64\hdf5\zlib.dll .
copy %REPOS%\external\lib64\hdf5\szip.dll .


copy C:\%HOMEPATH%\Applications\muhrec3.exe .

copy %REPOS%\applications\muhrec3\src\resources resources

rem cd C:\Qt\Qt5.2.1\5.2.1\msvc2012_64_opengl\bin

cd C:\Qt\5.8\msvc2015_64\bin

windeployqt %DEST%\muhrec3.exe
copy Qt5PrintSupport.dll %DEST%

popd

