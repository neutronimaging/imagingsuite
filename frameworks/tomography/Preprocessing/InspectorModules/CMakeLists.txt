

set(TARGET_NAME "InspectorModules")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../Framework/ReconFramework/include/)

set(INC_FILES
  CountNANs.h
  SaveProjections.h
  InspectorModules.h
  getimagesize.h
  ProjectionInspector.h
  inspectormodules_global.h
)

set(SRC_FILES
  CountNANs.cpp
  ProjectionInspector.cpp
  getimagesize.cpp
  InspectorModules.cpp
  SaveProjections.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# target_compile_definitions(${TARGET_NAME} PRIVATE INSPECTORMODULES_LIBRARY HAVE_NEXUS)
target_compile_definitions(${TARGET_NAME} PRIVATE INSPECTORMODULES_LIBRARY )

# add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

target_link_libraries(${TARGET_NAME}
PUBLIC
      kipl
      ImagingAlgorithms
      ModuleConfig
      ReconFramework
)

# Made the target link libraries os-inspecific
# if (WIN32)
# # target_link_directories(${TARGET_NAME} PRIVATE
# # ../../../../../ExternalDependencies/windows/lib
# # ../../../../../imagingsuite/external/lib64
# # ../../../../../imagingsuite/external/lib64/nexus
# # )

#   target_link_libraries(${TARGET_NAME}
#   PUBLIC
#         kipl
#         ImagingAlgorithms
#         ModuleConfig
#         ReconFramework
#   )
# endif()

# if (APPLE)
#     target_link_libraries(${TARGET_NAME}
#         PUBLIC
#         kipl
#         ImagingAlgorithms
#         ModuleConfig
#         ReconFramework
#     )
# endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
