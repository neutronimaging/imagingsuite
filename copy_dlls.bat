@echo ----------------- Deploy copy --------------
set REPOS=C:\Users\kaestner\repos
set DEST=C:\Users\kaestner\repos\imagingtool\trunk\build-imagingtool-Release\release
pushd .
cd %DEST%

copy %REPOS%\qni\trunk\src\ImagingAlgorithms\build-ImagingAlgorithms-Qt5-Release\release\ImagingAlgorithms.dll .
copy %REPOS%\modules\trunk\ModuleConfig\build-ModuleConfig-Qt5-Release\release\ModuleConfig.dll .
copy %REPOS%\gui\trunk\qt\build-QtAddons-Qt5-Release\release\QtAddons.dll .
copy %REPOS%\kipl\trunk\kipl\build-kipl-Qt5-Release\release\kipl.dll .
copy %REPOS%\src\libs\recon2\trunk\ReconFramework\build-ReconFramework-Qt5-Release\release\ReconFramework.dll .

copy %REPOS%\external\lib64\libtiff.dll .
copy %REPOS%\external\lib64\libjpeg-62.dll .
copy %REPOS%\external\lib64\zlib1.dll .
copy %REPOS%\external\lib64\libfftw3-3.dll .
copy %REPOS%\external\lib64\libfftw3f-3.dll .
copy %REPOS%\external\lib64\libxml2-2.dll .
copy %REPOS%\external\lib64\libiconv.dll .

cd C:\Qt\Qt5.2.1\5.2.1\msvc2012_64_opengl\bin
windeployqt %DEST%\imagingtool.exe
copy Qt5PrintSupport.dll %DEST%

popd
