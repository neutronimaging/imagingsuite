call set_vars.bat

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc2015

echo "Build tests"
pushd .
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

	%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\core\kipl\UnitTests\%2\%2.pro -o Makefile
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release
)
goto :eof

REM	if [ -e "$REPOSPATH/core/kipl/UnitTests/$f/$f.pro" ]
REM	then 
REM		mkdir -p $DEST/build-$f
REM		cd $DEST/build-$f
REM
REM                $QTBINPATH/qmake -makefile -r $SPECSTR -o Makefile ../../imagingsuite/core/kipl/UnitTests/$f/$f.pro
REM        make -f Makefile clean
REM        make -f Makefile mocables all
REM         make -f Makefile
REM	fi

REM done