
# CMake file for libCppEvent project
#

include_directories(${PROJECT_SOURCE_DIR}/test/gtest/include)
include_directories(${PROJECT_SOURCE_DIR}/test/common)

add_subdirectory(gtest)
add_subdirectory(common)
add_subdirectory(unit)
