file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_delegate ${sources} ${headers})
target_link_libraries(test_delegate sigcxx gtest)
