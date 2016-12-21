file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_signal_delete_this ${sources} ${headers})
target_link_libraries(test_signal_delete_this sigcxx gtest common)
