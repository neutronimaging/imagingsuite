call build_core_kipl.bat
call build_GUI.bat # needs to be here due to GUI components in modules
call build_core_modules.bat
call build_core_algorithms.bat

call build_frameworks_tomography.bat
call build_frameworks_imageprocessing.bat

rem call build_applications_NIQA.bat
rem call build_applications_imageviewer.bat
call build_applications_kiptool.bat
call build_applications_muhrec.bat
call build_applications_muhrecCLI.bat
call build_applications_CLItools.bat
