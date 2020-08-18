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
rem call build_frameworks_imageprocessing.bat

rem call build_applications_NIQA.bat
call build_applications_imageviewer.bat
rem call build_applications_kiptool.bat
call build_applications_muhrec.bat

call build_applications_muhrecCLI.bat
rem call build_application_kiptoolCLI.bat
call build_applications_CLItools.bat
