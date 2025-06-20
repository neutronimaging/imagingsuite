
set(TARGET_NAME "MuhRec")
project(${TARGET_NAME} VERSION 4.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

if (APPLE)
file(COPY ${CMAKE_BINARY_DIR}/lib/ DESTINATION "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Frameworks")
endif()

include_directories(
    ../../core/kipl/kipl/include
    ../../core/modules/ModuleConfig/include
    ../../core/modules/ReaderConfig
    ../../core/algorithms/ImagingAlgorithms/include
    ../../GUI/qt/QtAddons
    ../../GUI/qt/QtImaging
    ../../GUI/qt/QtModuleConfigure
    ../../core/modules/ReaderGUI
    ../../frameworks/tomography/Framework/ReconFramework/include/
    ../../core/algorithms/ImagingAlgorithms/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )

set(SRC_FILES
    src/MuhrecInteractor.cpp
    src/moduleconfigprogressdialog.cpp
    src/PreProcModuleConfigurator.cpp
    src/muhrecmainwindow.cpp
#    src/ReconReport.cpp
    src/piercingpointdialog.cpp
    src/configuregeometrydialog.cpp
    src/preferencesdialog.cpp
    src/dialogtoobig.cpp
    src/recondialog.cpp
    src/fileconversiondialog.cpp
    src/referencefiledlg.cpp
    src/findskiplistdialog.cpp
    src/stdafx.cpp
    src/globalsettingsdialog.cpp
    src/viewgeometrylistdialog.cpp
    src/tiltwarningdialog.cpp
    src/main.cpp
)

set(INC_FILES
    src/MuhrecInteractor.h
    src/moduleconfigprogressdialog.h
    src/PreProcModuleConfigurator.h
    src/muhrecmainwindow.h
#    src/ReconReport.h
    src/piercingpointdialog.h
    src/configuregeometrydialog.h
    src/preferencesdialog.h
    src/dialogtoobig.h
    src/recondialog.h
    src/fileconversiondialog.h
    src/referencefiledlg.h
    src/findskiplistdialog.h
    src/stdafx.h
    src/globalsettingsdialog.h
    src/viewgeometrylistdialog.h
    src/tiltwarningdialog.h
)

set(UI_FILES
    src/configuregeometrydialog.ui
    src/muhrecmainwindow.ui
    src/dialogtoobig.ui
    src/piercingpointdialog.ui
    src/fileconversiondialog.ui
    src/preferencesdialog.ui
    src/findskiplistdialog.ui
    src/recondialog.ui
    src/globalsettingsdialog.ui
    src/referencefiledlg.ui
    src/moduleconfigprogressdialog.ui
    src/viewgeometrylistdialog.ui
    src/tiltwarningdialog.ui
)

# The MACOSX_BUNDLE_ICON_FILE variable is added to the Info.plist
# generated by CMake. This variable contains the .icns file name,
# without the path.
set(MACOSX_BUNDLE_ICON_FILE muh4_icon.icns)

# And the following tells CMake where to find and install the file itself.
set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/src/muh4_icon.icns")
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/muhrec_icon.rc")
set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

# qt_add_executable(${TARGET_NAME} MACOSX_BUNDLE  ${app_icon_macos})

message(MuhRec Qt${QT_VERSION_MAJOR})


qt_add_executable(${TARGET_NAME}
    MANUAL_FINALIZATION
    ${SRC_FILES}
    ${INC_FILES}
    ${UI_FILES}
    ${app_icon_macos}
    ${APP_ICON_RESOURCE_WINDOWS}
    src/tiltwarningdialog.h src/tiltwarningdialog.cpp src/tiltwarningdialog.ui
)

if (WIN32)

    target_link_libraries(${TARGET_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        # Qt${QT_VERSION_MAJOR}::Concurrent
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReaderConfig
        QtAddons
        ReaderGUI
        QtModuleConfigure
        ReconFramework
        QtImaging
    )
endif()



if (UNIX)
    target_link_libraries(${TARGET_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        # Qt${QT_VERSION_MAJOR}::Concurrent
        
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReaderConfig
        QtAddons
        ReaderGUI
        QtModuleConfigure
        ReconFramework
        QtImaging
    )
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_NAME MuhRec
    MACOSX_BUNDLE_GUI_IDENTIFIER ch.psi.muhrec
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${TARGET_NAME})
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/applications
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/applications/${TARGET_NAME}.app/Contents/Frameworks)
