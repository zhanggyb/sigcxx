file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_event_base ${sources} ${headers})
target_link_libraries(test_event_base cppevent gtest common)
