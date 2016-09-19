# CMake file for sigcxx project
#

file(GLOB Header_Files "${PROJECT_SOURCE_DIR}/include/sigcxx/*.hpp")
file(GLOB Source_Files "*.cpp")

add_library (sigcxx ${Header_Files} ${Source_Files})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(NOT DEFINED BUILD_STATIC_LIBRARY)
  set_target_properties(sigcxx PROPERTIES VERSION 1 SOVERSION 1)
endif()

install(TARGETS sigcxx DESTINATION lib)
