call set_vars.bat
pushd .

set REPOSPATH=%WORKSPACE%\imagingsuite
set DEST=%WORKSPACE%\builds
set SPECSTR=-spec win32-msvc

mkdir %DEST%\build-KipToool
cd %DEST%\build-KipTool

%QTBINPATH%\qmake.exe -makefile -r %SPECSTR% ..\..\applications\kiptool\qt\QtKipTool\QtKipTool.pro -o Makefile
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile clean
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile mocables all
%QTBINPATH%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile release



