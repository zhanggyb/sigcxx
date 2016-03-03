# CMake project for libCppEvent
#

cmake_minimum_required(VERSION 2.8.11)

project(libCppEvent)

# You may need to set environment variable CMAKE_PREFIX_PATH, see http://doc.qt.io/qt-5/cmake-manual.html
option(WITH_QT5 "Build unit test to compare this with Qt5" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CMAKE_CXX_FLAGS	"/W4")
else()
  set(CMAKE_CXX_FLAGS	"-Wall -std=c++11")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/cppevent" DESTINATION include)
