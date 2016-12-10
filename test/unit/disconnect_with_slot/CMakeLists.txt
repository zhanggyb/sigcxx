file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_disconnect_with_slot ${sources} ${headers})
target_link_libraries(test_disconnect_with_slot sigcxx gtest common)
