# CMake file for libCppEvent project
#

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(delegate delegate.cpp)
target_link_libraries(delegate cppevent)
