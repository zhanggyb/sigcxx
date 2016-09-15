file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_event_connect ${sources} ${headers})
target_link_libraries(test_event_connect cppevent gtest common)
