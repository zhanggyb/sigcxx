
# CMake file for libCppEvent project
#

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(connect)
add_subdirectory(thread_safe)

#add_executable(delegate delegate.cpp)
#target_link_libraries(delegate cppevent)

#add_executable(simple simple.cpp)
#target_link_libraries(simple cppevent)

#add_executable(performance performance.cpp)
#target_link_libraries(performance cppevent)

#add_executable(thread_safe thread_safe.cpp)
#target_link_libraries(thread_safe cppevent gtest)
