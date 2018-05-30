call set_vars.bat
pushd .

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

mkdir %DEST%\build-imagingalgorithms
cd %DEST%\build-imagingalgorithms

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\core\algorithms\ImagingAlgorithms\qt\ImagingAlgorithms\ImagingAlgorithms.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release

mkdir %DEST%/build-imagingqaalgorithms
cd %DEST%/build-imagingqaalgorithms

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\core\algorithms\ImagingQAAlgorithms\ImagingQAAlgorithms.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release


echo "Build tests"

cd %REPOSPATH%\core\modules\UnitTests\

FOR /D %%I IN (*) DO @call :testloopbody %REPOSPATH% %%I %DEST%

echo "Tests built"
popd 

goto :eof

:testloopbody
echo %2
if exist "%1\core\algorithms\UnitTests\%2\%2.pro" (
	mkdir %3\%2
	cd %3\%2

	%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\core\algorithms\UnitTests\%2\%2.pro -o Makefile
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release
)
goto :eof

