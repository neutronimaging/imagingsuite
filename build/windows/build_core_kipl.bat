call set_vars.bat
pushd .


set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

alias jom = %QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe

mkdir %DEST%\build-kipl
cd %DEST%\build-kipl

%QTBINPATH%\qmake.exe ..\..\imagingsuite\core\kipl\kipl\qt\kipl.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

@REM jom -f Makefile clean
@REM jom -f Makefile mocables all
@REM jom -f Makefile release

echo "Build tests"

cd %REPOSPATH%\core\kipl\UnitTests\

FOR /D %%I IN (*) DO @call :testloopbody %REPOSPATH% %%I %DEST%

echo "Tests built"
popd 

goto :eof

:testloopbody
echo %2
if exist "%1\core\kipl\UnitTests\%2\%2.pro" (
	mkdir %3\%2
	cd %3\%2

	%QTBINPATH%\qmake.exe  ..\..\core\kipl\UnitTests\%2\%2.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

	@REM jom -f Makefile clean
	@REM jom -f Makefile mocables all
	@REM jom -f Makefile release
)
goto :eof
PAUSE
