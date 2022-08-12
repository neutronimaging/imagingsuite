echo %QTBINPATH%

call build_core_kipl.bat 
call build_core_algorithms.bat

IF EXIST %WORKSPACE%/ToFImaging (
echo ToFImaging exists
call %WORKSPACE%/ToFImaging/build/windows/build_tofimagingalgorithms.bat
) ELSE (
echo ToFImaging does not exist
)
call build_core_modules.bat
call build_GUI.bat

call build_frameworks_tomography.bat

call build_applications_imageviewer.bat
call build_applications_muhrec.bat

call build_applications_muhrecCLI.bat
call build_applications_CLItools.bat
