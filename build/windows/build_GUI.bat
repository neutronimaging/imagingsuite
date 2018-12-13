call set_vars.bat
pushd .

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

mkdir %DEST%\build-QtAddons
cd %DEST%\build-QtAddons

%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\GUI\qt\QtAddons\QtAddons.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release

mkdir %DEST%\build-QtModuleConfigure
cd %DEST%\build-QtModuleConfigure
 
%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\GUI\qt\QtModuleConfigure\QtModuleConfigure.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release

mkdir %DEST%/build-readerGUI
cd %DEST%/build-readerGUI

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\core\modules\ReaderGUI\ReaderGUI.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release

mkdir %DEST%/build-QtImaging
cd %DEST%/build-QtImaging

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\GUI\qt\QtImaging\QtImaging.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release


echo "Build tests"
cd %REPOSPATH%/GUI/qt/UnitTests

FOR /D %%I IN (*) DO @call :testloopbody %REPOSPATH% %%I %DEST%

echo "Tests built"
popd 

goto :eof

:testloopbody
echo %2
if exist "%1\GUI\qt\UnitTests\%2\%2.pro" (
	mkdir %3\%2
	cd %3\%2

	%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\core\kipl\UnitTests\%2\%2.pro -o Makefile
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release
)
goto :eof


