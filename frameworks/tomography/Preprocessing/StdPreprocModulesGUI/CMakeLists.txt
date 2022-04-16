cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "StdPreprocModulesGUI")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts PrintSupport REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts PrintSupport)

message("Using Qt${QT_VERSION_MAJOR}")

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../Framework/ReconFramework/include/
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../GUI/qt/QtAddons/
    ../../../../GUI/qt/QtModuleConfigure/
    ../StdPreprocModules/include
    ../../../../../ExternalDependencies/macos/include
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )

link_directories(../../../../../lib /opt/local/lib )

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
)

# target_compile_definitions(${TARGET_NAME} PRIVATE STDPREPROCMODULES_LIBRARY HAVE_NEXUS)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
  )

  message(${CMAKE_HOST_SYSTEM_PROCESSOR})
  if (APPLE)
      if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
          target_link_directories(${TARGET_NAME} PRIVATE
              /opt/local/lib
              ../../../../../ExternalDependencies/macos/arm64/lib
              )
      else()
          target_link_directories(${TARGET_NAME} PRIVATE
              /opt/local/lib
              ../../../../external/mac/lib/
              )
      endif()
  
      target_link_libraries(${TARGET_NAME}
          PRIVATE
          Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::Charts
 #         Qt${QT_VERSION_MAJOR}::PrintSupport
          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
          QtAddons
          QtModuleConfigure
          StdPreprocModules
          xml2
          fftw3
          fftw3f
          cfitsio
          tiff
          armadillo
          lapack
          blas
          NeXus.1.0.0
          NeXusCPP.1.0.0
      )
  endif()

  target_compile_definitions(${TARGET_NAME} PRIVATE STDPREPROCMODULESGUI_LIBRARY)

  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
