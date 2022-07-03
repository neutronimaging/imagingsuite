set REPOS=%WORKSPACE%
set DEST=%WORKSPACE%\deployed\muhrec
set BDEST=c:\

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy %BDEST%\lib\ImagingAlgorithms.dll .
copy %BDEST%\lib\ModuleConfig.dll .
copy %BDEST%\lib\QtAddons.dll .
copy %BDEST%\lib\QtModuleConfigure.dll .
copy %BDEST%\lib\kipl.dll .
copy %BDEST%\lib\ReconFramework.dll .
copy %BDEST%\lib\StdBackProjectors.dll .
copy %BDEST%\lib\StdPreprocModules.dll .
copy %BDEST%\lib\StdPreprocModulesGUI.dll .
copy %BDEST%\lib\InspectorModulesGUI.dll .
copy %BDEST%\lib\InspectorModules.dll .
copy %BDEST%\lib\FDKBackProjectors.dll .
copy %BDEST%\lib\ReaderConfig.dll .
copy %BDEST%\lib\ReaderGUI.dll .
copy %BDEST%\lib\QtImaging.dll .

copy %REPOS%\imagingsuite\external\lib64\libtiff.dll .
copy %REPOS%\imagingsuite\external\lib64\libjpeg-62.dll .
copy %REPOS%\imagingsuite\external\lib64\zlib1.dll .
copy %REPOS%\imagingsuite\external\lib64\libfftw3-3.dll .
copy %REPOS%\imagingsuite\external\lib64\libfftw3f-3.dll .


copy %REPOS%\imagingsuite\external\lib64\nexus\NeXus.dll .
copy %REPOS%\imagingsuite\external\lib64\nexus\NexusCPP.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5_cpp.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\zlib.dll .
copy %REPOS%\imagingsuite\external\lib64\hdf5\szip.dll .
copy %REPOS%\imagingsuite\external\lib64\libgcc_s_seh_64-1.dll .
copy %REPOS%\imagingsuite\external\lib64\libgcc_s_seh-1.dll .
copy %REPOS%\imagingsuite\external\lib64\libgfortran_64-3.dll .
copy %REPOS%\imagingsuite\external\lib64\libquadmath-0.dll .
copy %REPOS%\imagingsuite\external\lib64\libwinpthread-1.dll .
copy %REPOS%\imagingsuite\external\lib64\libblas.dll .
copy %REPOS%\imagingsuite\external\lib64\liblapack.dll .

copy %REPOS%\ExternalDependencies\windows\bin\libopenblas.dll .
copy %REPOS%\ExternalDependencies\windows\bin\cfitsio.dll .
copy %REPOS%\ExternalDependencies\windows\bin\libxml2.dll .


copy %BDEST%\Applications\MuhRec.exe .
copy %BDEST%\Applications\ImageViewer.exe .
copy %BDEST%\lib\MuhRecCLI.exe .
rem copy %REPOS%\lib\verticalslicer.exe
rem copy %REPOS%\lib\multiframesplitter.exe

copy %REPOS%\imagingsuite\applications\muhrec\Resources resources

cd %QTBINPATH%

rem windeployqt %DEST%\verticalslicer.exe
rem windeployqt %DEST%\multiframesplitter.exe
windeployqt %DEST%\muhrec.exe
windeployqt %DEST%\ImageViewer.exe
rem windeployqt %DEST%\muhrecCLI.exe

copy Qt6PrintSupport.dll %DEST%
copy Qt6Charts.dll %DEST%
copy Qt6OpenGLWidgets.dll %DEST%
copy Qt6OpenGl.dll %DEST%


popd
for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set ldt=%%j
set ldt=%ldt:~0,8%
FOR /F "tokens=*" %%g IN ('git rev-parse --short HEAD') do (set tag=%%g)

if exist "C:\Program Files\7-Zip\7z.exe" (
    "C:\Program Files\7-Zip\7z.exe" a %DEST%\..\MuhRec-Windows_build%tag%-%ldt%.zip %DEST%
) else (
    echo 7zip doesn't exist
)