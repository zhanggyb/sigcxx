# CMake project for libCppEvent
#

cmake_minimum_required(VERSION 2.8.11)

project(libCppEvent)
set(CMAKE_BUILD_TYPE_INIT "Debug")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(CMAKE_CXX_FLAGS	"/W4")
else()
set(CMAKE_CXX_FLAGS	"-Wall -std=c++11")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
