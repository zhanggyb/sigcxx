file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_event_check_connection ${sources} ${headers})
target_link_libraries(test_event_check_connection cppevent gtest common)
