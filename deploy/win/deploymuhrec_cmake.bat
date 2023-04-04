set REPOS=%WORKSPACE%
set DEST=%WORKSPACE%\deployed\muhrec
set BDEST=%WORKSPACE%\install

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy %BDEST%\lib\*.dll .
copy %BDEST%\lib\*.cp* .

@REM copy %BDEST%\lib\ImagingAlgorithms.dll .
@REM copy %BDEST%\lib\ModuleConfig.dll .
@REM copy %BDEST%\lib\QtAddons.dll .
@REM copy %BDEST%\lib\QtModuleConfigure.dll .
@REM copy %BDEST%\lib\kipl.dll .
@REM copy %BDEST%\lib\ReconFramework.dll .
@REM copy %BDEST%\lib\StdBackProjectors.dll .
@REM copy %BDEST%\lib\StdPreprocModules.dll .
@REM copy %BDEST%\lib\StdPreprocModulesGUI.dll .
@REM copy %BDEST%\lib\InspectorModulesGUI.dll .
@REM copy %BDEST%\lib\InspectorModules.dll .
@REM copy %BDEST%\lib\FDKBackProjectors.dll .
@REM copy %BDEST%\lib\ReaderConfig.dll .
@REM copy %BDEST%\lib\ReaderGUI.dll .
@REM copy %BDEST%\lib\QtImaging.dll .

@REM copy %REPOS%\imagingsuite\external\lib64\libtiff.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libjpeg-62.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\zlib1.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libfftw3-3.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libfftw3f-3.dll .


@REM copy %REPOS%\imagingsuite\external\lib64\nexus\NeXus.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\nexus\NexusCPP.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\hdf5\hdf5_cpp.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\hdf5\zlib.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\hdf5\szip.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libgcc_s_seh_64-1.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libgcc_s_seh-1.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libgfortran_64-3.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libquadmath-0.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libwinpthread-1.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\libblas.dll .
@REM copy %REPOS%\imagingsuite\external\lib64\liblapack.dll .

@REM copy %REPOS%\ExternalDependencies\windows\bin\libopenblas.dll .
@REM copy %REPOS%\ExternalDependencies\windows\bin\cfitsio.dll .
@REM copy %REPOS%\ExternalDependencies\windows\bin\libxml2.dll .


copy %BDEST%\applications\MuhRec.exe .
copy %BDEST%\applications\ImageViewer.exe .
@REM copy %BDEST%\MuhRecCLI.exe .

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