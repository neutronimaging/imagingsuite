call set_vars.bat
pushd .

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

mkdir %DEST%\build-ReconFramework
cd %DEST%\build-ReconFramework

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Framework\ReconFramework\qt\ReconFramework\ReconFramework.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-ReconAlgorithms
cd %DEST%\build-ReconAlgorithms

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Framework\ReconAlgorithms\ReconAlgorithms\ReconAlgorithms.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-StdBackProjectors
cd %DEST%\build-StdBackProjectors

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Backprojectors\StdBackProjectors\qt\StdBackProjectors\StdBackProjectors.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-FDKBackProjectors
cd %DEST%\build-FDKBackProjectors

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Backprojectors\FDKBackProjectors\qt\FDKBackProjectors\FDKBackProjectors.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-GenericBackProj
cd %DEST%\build-GenericBackProj

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Backprojectors\GenericBackProj\qt\GenericBackProj\GenericBackProj.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-IterativeBackProj
cd %DEST%\build-IterativeBackProj

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Backprojectors\IterativeBackProj\qt\IterativeBackProj\IterativeBackProj.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-InspectorModules
cd %DEST%\build-InspectorModules

$QTBINPATH\qmake -makefile -r $SPECSTR -o Makefile 
%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Preprocessing\InspectorModules\InspectorModules.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-InspectorModulesGUI
cd %DEST%\build-InspectorModulesGUI

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Preprocessing\InspectorModulesGUI\InspectorModulesGUI.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-StdPreprocModules
cd %DEST%\build-StdPreprocModules

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Preprocessing\StdPreprocModules\qt\StdPreprocModules\StdPreprocModules.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-StdPreprocModulesGUI
cd %DEST%\build-StdPreprocModulesGUI

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Preprocessing\StdPreprocModulesGUI\qt\StdPreprocModulesGUI\StdPreprocModulesGUI.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

mkdir %DEST%\build-genericpreproc
cd %DEST%\build-genericpreproc

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\imagingsuite\frameworks\tomography\Preprocessing\GenericPreProc\GenericPreProc.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release

echo "Build tests"

cd %REPOSPATH%\frameworks\tomography\UnitTests

FOR /D %%I IN (*) DO @call :testloopbody %REPOSPATH% %%I %DEST%

echo "Tests built"
popd 

goto :eof

:testloopbody
echo %2
if exist "%1\frameworks\tomography\UnitTests\%2\%2.pro" (
	mkdir %3\%2
	cd %3\%2

	%QTBINPATH%\qmake.exe -makefile ..\..\imagingsuite\frameworks\tomography\UnitTests\%2\%2.pro -o Makefile
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release clean
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release mocables all
	%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom\jom.exe -f Makefile.release
)
goto :eof
