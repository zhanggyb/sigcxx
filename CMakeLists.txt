# CMake project for sigcxx
#

cmake_minimum_required(VERSION 2.8.11)

project(sigcxx)

# The project version number.
set(VERSION_MAJOR 1 CACHE STRING "Project major version number.")
set(VERSION_MINOR 0 CACHE STRING "Project minor version number.")
set(VERSION_PATCH 0 CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# You may need to set environment variable CMAKE_PREFIX_PATH, see http://doc.qt.io/qt-5/cmake-manual.html
option(BUILD_UNIT_TEST "Build unit test code" OFF)
option(WITH_QT5 "Build unit test to compare this with Qt5" OFF)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if (BUILD_DOCUMENTATION)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif ()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    add_custom_command(TARGET doc
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/doc ${CMAKE_CURRENT_BINARY_DIR}/html/doc
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Copy \"${CMAKE_CURRENT_SOURCE_DIR}/doc\" to \"${CMAKE_CURRENT_BINARY_DIR}/html\""
            VERBATIM)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif ()


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_CXX_FLAGS "/W4")
    else ()
        set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
    endif ()
else ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_CXX_FLAGS "/W4")
    else ()
        set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++11")
    endif ()
endif ()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)

if (BUILD_UNIT_TEST)
    add_subdirectory(test)
endif ()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/sigcxx" DESTINATION include)
