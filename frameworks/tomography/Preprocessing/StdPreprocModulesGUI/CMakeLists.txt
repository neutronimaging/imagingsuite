

set(TARGET_NAME "StdPreprocModulesGUI")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

message("Using Qt${QT_VERSION_MAJOR}")

include_directories(
    ../../Framework/ReconFramework/include/
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/modules/ReaderConfig
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../GUI/qt/QtAddons/
    ../../../../GUI/qt/QtModuleConfigure/
    ../StdPreprocModules/include

    )

set(INC_FILES
    src/FullLogNormDlg.h
    src/SpotClean2Dlg.h
    src/StdPreprocModulesGUI_global.h
    src/WaveletRingCleanDlg.h
    src/adaptivefilterdlg.h
    src/bblognormdlg.h
    src/datascalerdlg.h
    src/generalfilterdlg.h
    src/medianmixringcleandlg.h
    src/morphspotcleandlg.h
    src/polynomialcorrectiondlg.h
    src/projectionfilterdlg.h
    src/stdpreprocmodulesgui.h
    src/ReplaceUnderexposedDlg.h
    src/targetver.h
)


set(SRC_FILES 
    src/FullLogNormDlg.cpp
    src/generalfilterdlg.cpp
    src/SpotClean2Dlg.cpp
    src/medianmixringcleandlg.cpp
    src/WaveletRingCleanDlg.cpp
    src/morphspotcleandlg.cpp
    src/adaptivefilterdlg.cpp
    src/polynomialcorrectiondlg.cpp
    src/bblognormdlg.cpp
    src/projectionfilterdlg.cpp
    src/datascalerdlg.cpp
    src/stdpreprocmodulesgui.cpp
    src/ReplaceUnderexposedDlg.cpp
)

set(UI_FILES
    src/FullLogNormDlg.ui
    src/generalfilterdlg.ui
    src/SpotClean2Dlg.ui
    src/medianmixringclean.ui
    src/WaveletRingCleanDlg.ui
    src/morphspotcleandlg.ui
    src/adaptivefilterdlg.ui
    src/polynomialcorrectiondlg.ui
    src/bblognormdlg.ui
    src/projectionfilterdlg.ui
    src/datascalerdlg.ui
    src/ReplaceUnderexposedDlg.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
  )

  message(${CMAKE_HOST_SYSTEM_PROCESSOR})
if (WIN32)
  target_link_libraries(${TARGET_NAME} 
  PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    kipl
    ImagingAlgorithms
    ModuleConfig
    ReaderConfig
    ReconFramework
    QtAddons
    QtModuleConfigure
    StdPreprocModules

    OpenBLAS::OpenBLAS

  )
endif()

if (APPLE)  
      target_link_libraries(${TARGET_NAME} PUBLIC
          Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::Charts
 #         Qt${QT_VERSION_MAJOR}::PrintSupport
          kipl
          ImagingAlgorithms
          ModuleConfig
          ReaderConfig
          ReconFramework
          QtAddons
          QtModuleConfigure
          StdPreprocModules
      )
endif()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${TARGET_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Charts
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        QtAddons
        QtModuleConfigure
        StdPreprocModules
        LibXml2::LibXml2 
        fftw::fftw
        TIFF::TIFF
        cfitsio::cfitsio
        armadillo::armadillo
        OpenBLAS::OpenBLAS
    )
  endif()

  target_compile_definitions(${TARGET_NAME} PRIVATE STDPREPROCMODULESGUI_LIBRARY)

  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
