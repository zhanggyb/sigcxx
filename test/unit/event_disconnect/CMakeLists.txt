file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_event_disconnect ${sources} ${headers})
target_link_libraries(test_event_disconnect cppevent gtest common)
