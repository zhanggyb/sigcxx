
# CMake file for sigcxx project
#

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(delegate)
add_subdirectory(trackable_unbind)
add_subdirectory(signal_base)
add_subdirectory(signal_connect)
add_subdirectory(signal_check_connection)
add_subdirectory(signal_disconnect)
add_subdirectory(signal_delete_this)
add_subdirectory(signal_virtual)
add_subdirectory(disconnect_on_fire)
add_subdirectory(disconnect_with_slot)
add_subdirectory(compare_boost_signal2)
add_subdirectory(thread_safe)

if (WITH_QT5)
    add_subdirectory(compare_qt5)
endif ()
