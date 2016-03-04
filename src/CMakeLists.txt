# CMake file for libCppEvent project
#

file(GLOB Header_Files "${PROJECT_SOURCE_DIR}/include/cppevent/*.hpp" "${PROJECT_SOURCE_DIR}/include/cppevent/private/*.hpp")
file(GLOB Source_Files "*.cpp")

add_library (cppevent ${Header_Files} ${Source_Files})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(NOT DEFINED BUILD_STATIC_LIBRARY)
  set_target_properties(cppevent PROPERTIES VERSION 1 SOVERSION 1)
endif()

install(TARGETS cppevent DESTINATION lib)
