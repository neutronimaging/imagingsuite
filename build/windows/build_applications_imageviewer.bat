call set_vars.bat
pushd .

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

mkdir %DEST%\build-imageviewer
cd %DEST%\build-imageviewer


%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\applications\imageviewer\imageviewer\imageviewer.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release

REM echo "Build tests"

REM cd %REPOSPATH%\applications\muhrec\UnitTests

REM FOR /D %%I IN (*) DO @call :testloopbody %REPOSPATH% %%I %DEST%

REM echo "Tests built"
REM popd 

REM goto :eof

REM :testloopbody
REM echo %2
REM if exist "%1\applications\muhrec\UnitTests\%2\%2.pro" (
	REM mkdir %3\%2
	REM cd %3\%2

	REM %QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\applications\muhrec\UnitTests\%2\%2.pro -o Makefile
	REM %QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
	REM %QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
	REM %QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release
REM )
REM goto :eof
