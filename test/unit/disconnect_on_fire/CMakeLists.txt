file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_disconnect_on_fire ${sources} ${headers})
target_link_libraries(test_disconnect_on_fire sigcxx gtest common)
