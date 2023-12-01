set REPOS=%WORKSPACE%
set DEST=%WORKSPACE%\deployed\muhrec
set BDEST=%WORKSPACE%\install

if not exist %DEST% mkdir %DEST%

pushd .
cd %DEST%
mkdir resources

copy %BDEST%\lib\*.dll .
copy %BDEST%\lib\*.cp* .

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