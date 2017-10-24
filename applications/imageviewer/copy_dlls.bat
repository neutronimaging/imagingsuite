@echo ----------------- Deploy copy --------------
set REPOS=C:\%HOMEPATH%\git
set DEST=C:\%HOMEPATH%\imagingscience

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy %REPOS%\lib\QtAddons.dll .
copy %REPOS%\lib\kipl.dll .
copy %REPOS%\lib\ReaderConfig.dll .


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


copy %REPOS%\Applications\imageviewer.exe .

rem copy %REPOS%\tomography\trunk\src\muhrec3\resources resources

rem cd C:\Qt\Qt5.2.1\5.2.1\msvc2012_64_opengl\bin

cd C:\Qt\5.8\msvc2015_64\bin

windeployqt %DEST%\imageviewer.exe
copy Qt5PrintSupport.dll %DEST%

popd

