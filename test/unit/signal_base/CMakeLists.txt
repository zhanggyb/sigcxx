file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_signal_base ${sources} ${headers})
target_link_libraries(test_signal_base sigcxx gtest common)
