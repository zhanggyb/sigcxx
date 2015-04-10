# CMake file for libCppEvent project
#

file(GLOB Header_Files "${PROJECT_SOURCE_DIR}/include/cppevent/*.hpp")
file(GLOB Source_Files "*.cpp")

add_library (cppevent ${Header_Files} ${Source_Files})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(NOT DEFINED BUILD_STATIC_LIBRARY)
    set_target_properties(cppevent PROPERTIES VERSION 1 SOVERSION 1)
endif()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/cppevent" DESTINATION include)
install(TARGETS cppevent DESTINATION lib)
