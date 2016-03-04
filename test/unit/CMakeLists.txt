
# CMake file for libCppEvent project
#

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(delegate)
add_subdirectory(basic)
add_subdirectory(connect)
add_subdirectory(disconnect_on_fire)
add_subdirectory(derived)
add_subdirectory(performance)
add_subdirectory(thread_safe)

if(WITH_QT5)
	add_subdirectory(qt5)
endif()
